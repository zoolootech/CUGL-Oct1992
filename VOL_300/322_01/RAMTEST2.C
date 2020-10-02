/*
************************************************************

   Routines Programmed in C for Detecting Stuck-At Faults
		  in Semiconductor RAMs

			   by

		  Chaiyos Ruengsakulrach
                         and
                   Dean Lance Smith
	   Department of Electrical Engineering
		 Memphis State University
		    Memphis, TN 38152
		     (901)-678-3253
		     (901)-678-2175



       These test routines were written using the MATS and
  MATS+ algorithms which are capable of detecting multiple
  stuck-at faults in Random Access Memories (RAMs).  The
  MATS (Modified Algorithmic Test Sequence) detects stuck-at
  faults only for RAMs with wired-OR behavior.  The MATS+
  detects stuck-at faults for RAMS with either wired-OR or
  wired-AND behavior.


		       REFERENCE

  Nair, Ravindra, "Comments on 'An Optimal Algorithm for
  Testing Stuck-at Faults in Random Access Memories,'"
  IEEE Trans. on Computers, v. C-28, n. 3, Mar., 1979,
  pp. 258-261.

************************************************************

     These routines were compiled with a Turbo C compiler
  (Version 2.01) that runs on MS-DOS on an IBM PC/XT clone.

     Huge pointers were chosen for this particular
  application.  Huge pointers are 32 bits long and contain
  both a segment address and an offset.  The segment address
  ranges from 0000 to FFFF in hexadecimal.  The offset
  ranges from 0000 to 000F in hexadecimal.

     The following functions used in this program are already
  available in Turbo C.
  - Function CLRSCR() in the library conio.h clears the
    screen.
  - Function GOTOXY(x,y) in the library conio.h puts the
    cursor at column x, row y.
  - Function WHEREX() in the library conio.h gives horizontal
    cursor position within window.
  - Function WHEREY() in the library conio.h gives vertical
    cursor position within window.
  - Function MK_FP(segment,offset) creates a huge pointer
    from its component segment and offset parts.

************************************************************
*/

#include<stdio.h>                      /* library stdio.h */
#include<dos.h>                        /* library dos.h   */
#include<conio.h>                      /* library conio.h */
int chkloc(unsigned char huge *start,
	   unsigned char huge *end);  /* check whether input
					 addresses valid  */
unsigned char huge *getstart(void);    /* get the starting
					  address         */
unsigned char huge *getend(void);      /* get the ending
					  address         */
unsigned gethex(void);      /* get hexadecimal characters
			       and convert them to a
			       binary integer             */
unsigned mats(unsigned char huge *start,
	      unsigned char huge *end);      /* test RAM
					       using MATS */
unsigned matsp(unsigned char huge *start,
	       unsigned char huge *end);     /* test RAM
					      using MATS+ */
void error(unsigned char huge *rptr, char stuck_at,
	   char stage, unsigned char flaw);   /* report
					     fault result */
/*
************************************************************
		     function MAIN

  displays the menu on the screen and lets the user select
  a test option.

************************************************************
*/

main()
{
 unsigned char huge *begin;        /*  starting address  */
 unsigned char huge *final;        /*  ending address    */
 unsigned num_fault;               /*  number of faults  */
 char ch;                          /*  store the option  */
 int okay;        /* check whether input addresses valid */
 do
 {
  clrscr();                          /* clear the screen */
  gotoxy(1,8);           /* put cursor at column 1,row 8 */
  printf("    RAM TESTING : DETECTING STUCK-AT FAULTS  \n");
  printf("    =======================================\n\n");
  printf("    1.  MATS  ALGORITHM                      \n");
  printf("    2.  MATS+  ALGORITHM                     \n");
  printf("    3.  EXIT                             \n\n\n");
  printf("    Please type number to select algorithm "
	 "to test RAM ");
  ch =  getch();                       /* get the option */
  okay = 0;
  switch(ch)
  {
  case '1' :
    while( okay != 1 )
    {
      clrscr();
      printf("  RAM TESTING by using the MATS  \n");
      printf("  =============================\n\n");
      begin = getstart();       /* get starting adddress */
      final = getend();         /*  get ending address   */
      okay = chkloc(begin, final);      /* check starting
					   and ending
					   addresses OK? */
    }
    num_fault = mats(begin, final);         /* test RAM
					      using MATS */
    break;
  case '2' :
    while( okay != 1 )
    {
      clrscr();
      printf("  RAM TESTING by using the MATS+  \n");
      printf("  ==============================\n\n");
      begin = getstart();       /* get starting address  */
      final = getend();         /*  get ending address   */
      okay = chkloc(begin, final);      /* check starting
					   and ending
					   addresses OK? */
    }
    num_fault = matsp(begin, final);       /* test RAM
					     using MATS+ */
    break;
  }
  if( ch == '1' || ch == '2' )
  {
    if( num_fault == 0 )                /*   no fault   */
       printf("  There are no stuck-at faults in the RAM\n"
	      "  between %Fp and %Fp. \n",begin, final);
    else                                /* faults exist */
       printf("\n\n  Number of stuck-at faults occuring in"
	      " the RAM \n  between %Fp and %Fp is   %u.",
		begin, final, num_fault);
    printf("\n\n  Press any key to return to MENU \n");
    getch();
  }
 }
 while(ch != '3');
}

/*
************************************************************
		    function CHKLOC
************************************************************
*/
int chkloc( unsigned char huge *start,
	    unsigned char huge *end )
{
 int okay;
 if( start > end )
 {
   okay = 0;
   printf("\n\n\n");
   printf("  **** INVALID ADDRESSES **** \n\n");
   printf("  THE ENDING ADDRESS MUST BE GREATER THAN \n"
	  "  OR EQUAL TO THE STARTING ADDRESS !!! \n\n\n");
   printf("  Press any key to reenter all the"
	  " addresses again... ");
   getch();
   return(okay);
 }
 else
 {
   okay = 1;
   return(okay);
 }
}

/*
************************************************************
		    function GETSTART

  creates a huge pointer 'start' that points to the
  first location to be tested in the RAM.  This pointer
  is returned to the calling function.

************************************************************
*/

unsigned char huge *getstart(void)
{
  unsigned char huge *start;
  unsigned seg, off;    /* segment and offset of address  */
  printf("  Please enter 4 hexadecimal characters for the"
	 " SEGMENT and\n  OFFSET addresses of the RAM to"
	 " be tested \n\n");
  printf("  STARTING ADDRESS =>   SEGMENT =  ");
  seg = gethex();
  printf("                        OFFSET  =  ");
  off = gethex();
  start = MK_FP(seg, off);   /* make huge pointer 'start' */
  return(start);
}

/*
************************************************************
		     function GETEND

  creates a huge pointer 'end' that points to the last
  location to be tested in the RAM.  This pointer is
  returned to the calling function.

************************************************************
*/

unsigned char huge *getend(void)
{
  unsigned char huge *end;         /*   ending address   */
  unsigned seg, off;               /* segment and offset */
  printf("  ENDING ADDRESS   =>   SEGMENT =  ");
  seg = gethex();
  printf("                        OFFSET  =  ");
  off = gethex();
  printf("\n\n");
  end = MK_FP(seg, off);      /* make huge pointer 'end' */
  return(end);
}

/*
************************************************************
		    function GETHEX
************************************************************
*/

unsigned gethex(void)
{
    int prex, prey;        /* previous location of cursor */
    unsigned num, n, i;
    char s[10];
/*--------------------------------------------------------*/
/*  get an ASCII string of hexadecimal characters from    */
/*  the keyboard ( maximum number of characters is 4 )    */
/*  Note that '\r' (carriage return) may need to be       */
/*  changed to '\n' (end of line) with some compilers     */
/*  and operating systems.                                */
/*--------------------------------------------------------*/
    i = 0;
    num = 0;
    while(  (num < 4) && ((s[i] = getch()) != '\r')  )
    {
	if(  ( s[i] >= '0' && s[i] <= '9' ) ||
	     ( s[i] >= 'a' && s[i] <= 'f' ) ||
	     ( s[i] >= 'A' && s[i] <= 'F' )  )
	{
	     printf("%c", s[i]);
	     i++;
	     num++;
	}
	else if( s[i] == '\b' )
	{
	     i--;
	     num--;
	     s[i] = '0';
	     prex = wherex() - 1;
	     prey = wherey();
	     gotoxy( prex, prey );
	     printf(" ");
	     gotoxy( prex, prey );
	 }
    }
    s[i] = '\0';                 /* end the string with
				    ' string terminator ' */
    printf("\n");
/*--------------------------------------------------------*/
/*  convert the ASCII string 's[]' to a binary integer    */
/*--------------------------------------------------------*/
    i = 0;
    n = 0;
    while( s[i] != '\0' )
    {
      if( s[i] >= '0' && s[i] <= '9')
	  n = (16 * n) + s[i] - '0';
      if( s[i] >= 'A' && s[i] <= 'F')
	  n = (16 * n) + 10 + s[i] - 'A';
      if( s[i] >= 'a' && s[i] <= 'f')
	  n = (16 * n) + 10 + s[i] - 'a';
      i++;
    }
    return(n);
}


/*
************************************************************
		     function MATS
************************************************************
*/

unsigned mats( unsigned char huge *start,
	       unsigned char huge *end )
{
   unsigned char huge *ramptr;   /* pointer to current RAM
				    location              */
   unsigned char flaw;           /* contents of a faulty
				    memory word           */
   unsigned num_fault;
   num_fault = 0;
/*--------------------------------------------------------*/
/* begin stage A: write 0 into all locations.             */
/*--------------------------------------------------------*/
   for(ramptr = start; ramptr <= end; ramptr++)
       *ramptr = 0x00;
/*--------------------------------------------------------*/
/* begin stage B: read each location and write 1's into   */
/*                the location.                           */
/*--------------------------------------------------------*/
   for(ramptr = start; ramptr <= end; ramptr++)
   {
      if( (flaw = *ramptr) != 0x00 )      /* check the data
					     read         */
      {
	 num_fault++;          /* stuck-at-1 fault occurs */
	 error(ramptr, '1', 'B', flaw);   /* report error */
      }
      *ramptr = 0xFF;
    }
/*--------------------------------------------------------*/
/* begin stage C: read all locations.                     */
/*--------------------------------------------------------*/
    for(ramptr = start; ramptr <= end; ramptr++)
      if( (flaw = *ramptr) != 0xFF )      /* check the data
					     read         */
      {
	 num_fault++;          /* stuck-at-0 fault occurs */
	 error(ramptr, '0', 'C', flaw);   /* report error */
      }
/*------------------------------------------------------- */
    return(num_fault);
}

/*
************************************************************
		     function MATSP
************************************************************
*/

unsigned matsp( unsigned char huge *start,
		unsigned char huge *end )
{
   unsigned char huge *ramptr;   /* pointer to current RAM
				    location              */
   unsigned char flaw;           /* contents of a faulty
				    memory word           */
   unsigned num_fault;
   num_fault = 0;
/*--------------------------------------------------------*/
/* begin stage A: write 0 into all locations except       */
/*                the first location.                     */
/*--------------------------------------------------------*/
   ramptr = start  + 1;
   while( ramptr <= end )
   {
	*ramptr = 0x00;
	ramptr++;
   }
/*--------------------------------------------------------*/
/* begin stage B: write 1's into the first location.      */
/*--------------------------------------------------------*/
   ramptr = start;
   *ramptr = 0xFF;
/*--------------------------------------------------------*/
/* begin stage C: read each location except the first     */
/*                and write 1's into the location.        */
/*--------------------------------------------------------*/
   ramptr++;
   while( ramptr <= end )
   {
      if( (flaw = *ramptr) != 0x00 )     /* check the data
					    read          */
      {
	 num_fault++;          /* stuck-at-1 fault occurs */
	 error(ramptr, '1', 'C', flaw);   /* report error */
      }
      *ramptr = 0xFF;
      ramptr++;
   }
/*--------------------------------------------------------*/
/* begin stage D: read each location except the last      */
/* 	          and write 0 into the location.          */
/*                Note that if the first location is the  */
/*                same as the last location, the MATS+    */
/*                read the first location and then write  */
/*                0 into that location.                   */
/*--------------------------------------------------------*/
   ramptr = start;
   do
   {
      if( (flaw = *ramptr) != 0xFF )     /* check the data
					    read          */
      {
	 num_fault++;          /* stuck-at-0 fault occurs */
	 error(ramptr, '0', 'D', flaw);   /* report error */
      }
      *ramptr = 0x00;
      ramptr++;
   }
   while( ramptr < end );
/*--------------------------------------------------------*/
/* begin stage E: read the last location.  If the first   */
/*                location is the same as the last        */
/*                location, the MATS+ will skip this      */
/*                stage.                                  */
/*--------------------------------------------------------*/
   if( start != end )
     if( (flaw = *ramptr) != 0xFF )      /* check the data
					    read          */
     {
       num_fault++;            /* stuck-at-0 fault occurs */
       error(ramptr, '0', 'E', flaw);     /* report error */
     }
/*--------------------------------------------------------*/
/* begin stage F: read the first location.                */
/*--------------------------------------------------------*/
   ramptr = start;
   if( (flaw = *ramptr) != 0x00 )        /* check the data
					    read          */
   {
       num_fault++;            /* stuck-at-1 fault occurs */
       error(ramptr, '1', 'F', flaw);     /* report error */
   }
/*--------------------------------------------------------*/
   return(num_fault);
}

/*
************************************************************
		     function ERROR
************************************************************
*/

void error(unsigned char huge *rptr, char stuck_at,
	   char stage, unsigned char flaw)
{
   printf("  Stage %c in the algorithm \n",stage);
   if( stuck_at == '1' )
       printf("  RAM address %Fp is stuck-at-1. \n",rptr);
   else
       printf("  RAM address %Fp is stuck-at-0. \n",rptr);
   printf("  The contents of RAM address %Fp is %x \n\n",
	     rptr, flaw);
}
