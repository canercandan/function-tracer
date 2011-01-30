
#include <stdio.h>

#if defined(__amd64__) || defined(__ia64__) || defined(__x86_64__)
# define ARCHI 64
#else
# define ARCHI 32
#endif

  char* testyavuzzzzzzzzzzzzzzzz(char **av)
  {
    (void)av;
    return (0);
  }

  int testyaaaaaaaaaaaaaaaaaaaaaaaaaaaaaan(int ttttttttttttttttttttttt)
  {
    (void)ttttttttttttttttttttttt;
    //puts("oi");
    //putchar('d');
    return 0;
  }

  int	main()
  {
    while (1)
      {
	testyaaaaaaaaaaaaaaaaaaaaaaaaaaaaaan(4);
	testyavuzzzzzzzzzzzzzzzz(0);
	testyavuzzzzzzzzzzzzzzzz(0);
      }
    return 0;
  }
