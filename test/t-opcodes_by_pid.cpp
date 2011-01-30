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

#define OP_E8CW(pc)     ((((pc) >> (8 * 1)) & 0xFF) == 0xE0)
#define OP_E8CD(pc)     ((((pc) >> (8 * 2)) & 0xFF) == 0xE0)
#define OP_9ACD(pc)     ((((pc) >> (8 * 2)) & 0xFF) == 0x9A)
#define OP_9ACP(pc)     ((((pc) >> (8 * 3)) & 0xFF) == 0x9A)
#define OP_FF(pc)       (((pc & 0xFF) == 0xFF) == 0xFF)
#define IS_OPCODE(pc)   (OP_E8CW(pc) || OP_E8CD(pc) || OP_9ACD(pc) || OP_9ACP(pc) || OP_FF(pc))

int	main(int ac, char** av)
{
  if (ac < 2)
	throw std::runtime_error("no pid");

  pid_t used_pid = atoi(av[1]);

  std::cout << "used pid: " << used_pid << std::endl;

  if (ptrace(PT_ATTACH, used_pid, NULL, NULL) == -1)
    {
	  perror("ptrace(ATTACH) : ");
	  exit(EXIT_FAILURE);
    }

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
              struct user_regs_struct   u_regs;
			  if (ptrace(PT_GETREGS, pid, (caddr_t)0, &u_regs) == -1)
				perror("ptrace(GETREGS) :");
			  else
                {
				  //int instruction = 0;
                  unsigned long addr = u_regs.eip;
                  long pc = ptrace(PT_READ_I, pid, (caddr_t)addr, 0);
                  if (pc == -1)
                    {
                      perror("ptrace(READ_I) : ");
                    }
                  else
                    {
                      if (IS_OPCODE(pc))
                        {
                            std::cout << "instruction: " << std::hex << pc << std::endl;
                          //std::cout << "opcode: " << std::hex << opcode << std::endl;
                        }
                    }
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
    }
  
  return 0;
}
