#include <stdio.h>

  void    a(int);
  void    b(int);
  int     c(int);
  int     main(void);

  void    a(int aa)
  {
    b(aa+1);
    (void)c(aa+2);
  }

  void    b(int ba)
  {
    (void)c(ba+1);
  }

  int     c(int ca)
  {
    return ca;
  }

  int main(void)
  {
    a(1);
    b(2);
    (void)c(3);

    return 0;
  }
