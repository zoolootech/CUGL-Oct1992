/* CLOCK... a program to set the hardware clock on the AST
     Six Pack Card at address's 2c0 thru 2cA.  
          by Ben R. Wert 01-84 */

#include "stdio.h"

int num,a,innum,outnum;
int sec,minutes,hour,mday,month,wday,year,nibble,last;
int  ch;

gettime()  
{
     month     = inp(0x2c7);
     mday      = inp(0x2c6);
     wday      = inp(0x2c5);
     hour      = inp(0x2c4);
     minutes   = inp(0x2c3);
     sec       = inp(0x2c2);
     year      = inp(0x2ca);
     nibble    = inp(0x2c8);
     last      = inp(0x2c9);
}

showtime()
{
     printf("\nYear\tMonth\tDate\tDay\tHour\tMinutes\tSeconds\n");

     printf("%x \t %x \t %x \t %x \t %x \t %x \t %x\n", year,month,mday,
          wday,hour,minutes,sec);

}

main()
{
	scr_clr();

     gettime();

     showtime();

     printf("\nDo you wish to change the settings?...y/n  ");
     ch = toupper(getchar());

     if (ch == 'Y') 
     {
          putchar('\n');
          printf("Enter last digit of year ");
               scanf("%x",&year);
          printf("\nEnter current month ");
               scanf("%x",&month);
          printf("\nEnter current day ");
               scanf("%x",&mday);
          printf("\nEnter the # of weekday ");
               scanf("%x",&wday);
          printf("\nEnter current hour ");
               scanf("%x",&hour);
          printf("\nEnter current minutes ");
               scanf("%x",&minutes);
          printf("\nEnter current seconds ");
               scanf("%x",&sec);
		
	  outp(0x2ca,year);
          outp(0x2c7,month);
          outp(0x2c6,mday);
          outp(0x2c5,wday);
          outp(0x2c4,hour);
          outp(0x2c3,minutes);
          outp(0x2c2,sec);
     }

     putchar('\n');
     putchar('\n');
     gettime();
     showtime();
     exit();
}
