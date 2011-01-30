
  void test1(int b)
  {
    if (b > 0)
      test1(b - 1);
    return;
  }

  int test2(int a)
  {
    (void)a;
    return 0;
  }

  int	main()
  {
    test1(4);
    test2(0);
    return 0;
  }
