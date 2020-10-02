/*  >  TEST.C
 *
 *  dprintf -- Test Program
 *  (C)  August 30  1989  Arkin Asaf
 *  All rights reserved
 *
 *  TEST compares the output of dprintf with that of printf:
 *    TEST      Produce dprintf output,
 *    TEST -    Produce printf output.
 */



int  main(int Argc, char **Argv)
{
  int  Cnt1,  Cnt2,  Cnt3;
  char  *String;
  int (*Func)(dprintf_fp, const char *,...);
  int  fake_dprintf(dprintf_fp, const char *, ...);

  if (Argc>1 && Argv[1][0]=='-')
    Func=fake_dprintf;
  else
    Func=dprintf;
  String="TheBrownFoxJumped";

  Func(putchar,"%10.4g  %10.4g\n",12.678,-3.4567);
  Func(putchar,"\n");

  Func(putchar,"%hd  %ld  %Lf\n",12,1234445L,454.656L);
  Func(putchar,"%n %u %n %u %n\n",&Cnt1,1234,&Cnt2,-1234,&Cnt3);
  Func(putchar,"%d  %d  %d\n",Cnt1,Cnt2,Cnt3);
  Func(putchar,"%c  %5c  %05c  %-5c  %5.2%\n",'a','b','c','d');
  Func(putchar,"%6.4s  %s  %06.4s  %-6.4s\n",String,String,String,String);
  Func(putchar,"%*.*s\n",6,4,String);
  Func(putchar,"\n");

  Func(putchar,"%d  %6.4d  %-6d  % 6d  %+6d  %+06d  %06d\n",547,23,1754,8467,2657,-64,-64);
  Func(putchar,"%o  %6.4o  %#6.4o  %#6o  %-#6o\n",45,45,45,45,45);
  Func(putchar,"%x  %6.4x  %#6.4x  %#6X  %-#6X\n",45,45,45,45,45);
  Func(putchar,"%p  %#p  %10.4p\n",&Cnt1,&Cnt1,&Cnt1);
  Func(putchar,"\n");

  Func(putchar,"%f  %10.2f  %5.f  %#5.f  %#5.2f  %+7.2f  %07.2f  %-+7.2f\n",365.56e4,4567.4746,343.56, 343.56,343.5, 4536.4,-4536.4,4536.4);
  Func(putchar,"%e  %10.2e  %5.e  %#5.e  %#5.2e  %+7.2E  %07.2E  %-+7.2E\n",365.56e4,4567.4746,343.56, 343.56,343.5, 4536.4,-4536.4,4536.4);
  Func(putchar,"%g  %10.2g  %5.g  %#5.g  %#5.2g  %+7.2G  %07.2G  %-+7.2f\n",365.56e4,4567.4746,343.56, 343.56,343.5, 4536.4,-4536.4,4536.4);
  Func(putchar,"\n");
  return 0;
}


int  fake_dprintf(dprintf_fp Func, const char *Format, ...)
{
  int  Return;
  va_list  Args;

  va_start(Args,Format);
  Return=vprintf(Format,Args);
  va_end(Args);
  return  Return;
}


