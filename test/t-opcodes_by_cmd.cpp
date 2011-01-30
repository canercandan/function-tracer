/* -*- mode: c++; c-basic-offset: 2; tab-width: 4; indent-tabs-mode: nil -*- */

#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cstdlib>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <stdexcept>
#include <cstdio>
#include <unistd.h>

#include "defines.h"

#if SYSTEM == 0 //LINUX
#include <sys/user.h>
typedef struct user_regs_struct u_regs;
# if ARCHI == 32
#  define EIP(regs)     ((regs).eip)
# else // 64
#  define EIP(regs)     ((regs).rip)
# endif // !ARCHI
#elif SYSTEM == 1 // BSD
#include <machine/reg.h>
typedef struct reg              u_regs;
# define EIP(regs)      ((regs).r_eip)
#else // OTHERS
#include <sys/user.h>
typedef struct user_regs_struct u_regs;
# define EIP(regs)      ((regs).eip))
#endif // !SYSTEM

int	main(int ac, char** av)
{
  if (ac < 2)
	throw std::runtime_error("no cmd");

  pid_t used_pid = fork();

  if (used_pid == -1)
    exit(EXIT_FAILURE);

  if (used_pid == 0) //! child
    {
      if (ptrace(PT_TRACE_ME, 0, NULL, NULL) == -1)
        {
          perror("traceme: ptrace(PTRACE_TRACEME, ...): Operation not permitted");
          exit(EXIT_FAILURE);
        }
      if (execvp(*(av + 1), av + 1) == -1)
        perror("execvp");
    }
  else //! parent
    {
      std::cout << "used pid: " << used_pid << std::endl;
      bool      toggle_call = false;

      int       iter_cnt = 0;
      int       toggle_0_cnt = 0;

      while (1)
        {
          int status;
          pid_t pid = wait4(used_pid, &status, WUNTRACED, 0);
          if (pid == -1 && errno != EINTR)
            {
              perror("wait4: ");
              break;
            }

          int signal = 0;
          if (WIFEXITED(status))
            {
              std::cout << "exit = " << WEXITSTATUS(status) << std::endl;
              break;
            }

          if (WIFSIGNALED(status))
            {
              std::cout << "\nWIFSIGNALED : Number of the signal : " << WTERMSIG(status) << std::endl;
              break;
            }

          if (WIFSTOPPED(status))
            {
              if (WSTOPSIG(status) == SIGTRAP)
                {
                  u_regs regs;
                  if (ptrace(PT_GETREGS, pid, (caddr_t)0, (int)&regs) == -1)
                    perror("ptrace(GETREGS) :");
                  else
                    {
                      long pc = ptrace(PT_READ_I, pid, (caddr_t)EIP(regs), 0);
                      if (pc == -1)
                        perror("ptrace(READ_I) : ");
                      else
                        {
                          if (toggle_call == false)
                            {
                              if ((pc & 0xFF) == 0xE8) // E8 cw (@ in 2-byte)
                                {
                                  std::cout << std::hex;
                                  std::cout << "instruction: " << pc << " ";
                                  std::cout << "eip: " << EIP(regs) << " ";
                                  std::cout << std::endl;
                                  toggle_call = true;
                                }
                            }
                          else
                            {
                              std::cout << std::hex;
                              std::cout << "function: " << pc << " ";
                              std::cout << "eip: " << EIP(regs) << " ";
                              std::cout << std::endl;
                              toggle_call = false;
                            }
                        }
                      toggle_0_cnt++;
                    }
                }
            }
          else
            {
              signal = WSTOPSIG(status);
            }

          if (ptrace(PT_STEP, pid, (caddr_t)1, signal) == -1)
            {
              perror("ptrace(STEP)) : ");
              exit(EXIT_FAILURE);
            }
          iter_cnt++;
        }
      std::cout << "CNT: " << std::dec << iter_cnt << std::endl;
      std::cout << "TOGGLE 0 CNT: " << std::dec << toggle_0_cnt << std::endl;
    }

  return 0;
}
