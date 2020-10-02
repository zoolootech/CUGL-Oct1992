/*------------------------------------------------------

                      Mohammad Khurrum
                            and
                      Dean Lance Smith
               Dept. of Electrical Engineering
                  Memphis State University
                      Memphis, TN 38152
                       (901) 678-3253
                       (901) 678-2175

 This program is an implementation of the ATS
 (Algorithmic Testing Sequence) algorithm.  It was
 compiled with TURBOC 1.5 on MS DOS.  The program tests
 RAM for any single or multiple stuck-at-0 or stuck-at-1
 faults.

 The inputs to the program are the starting and ending
 addresses of any part of the Random Access Memory.  The
 output is the address of the location where the fault
 occurs and also the type of fault that occurs (stuck-
 at-0 or stuck-at-1).  A message is displayed if no
 fault is detected.

 The ATS procedure works only if the decoder is
 noncreative wired-OR logic.  These restrictions do not
 apply to the ATS+ procedure.  See R. Nair, "Comments on
 An Optimal Algorithm for Testing Stuck-at Faults in
 Random Access Memories," IEEE Trans.  Comput., vol.  C-
 28, pp. 258-261.  Mar.  1979.

 The algorithms are written as two separate functions
 ats() and atst().  The ats() function has eight steps.
 The atst() function has thirteen steps.  Some of these
 steps write a pattern into memory and others read
 memory to check for faults.  RAM is divided into three
 sections, g0, g1 and g2.  For both algorithms g0, g1,
 g2, start and end are declared as huge pointers.  Huge
 pointers are 32-bit pointers that are unique to TurboC.

 The function hex() reads the starting and ending
 addresses from the keyboard in the form segment :
 offset where segment and offset are in hexadecimal.
 The function converts the addresses to binary values
 and far pointers.  A far (32-bit) pointer contains not
 only the offset but also a 16-bit segment address.  Far
 pointers permit multiple code segments and allow access
 to memories larger than 64K bytes.

 The functions st() and en() are 8086 dependent.  These
 functions make far pointers for the starting and ending
 addresses.  The functions clear() and deb() are MS DOS
 dependent.
 ------------------------------------------------------
*/

#include <stdio.h>
#include<dos.h>
#define MAXVAL 255 /* Maximum size of an input line. */

/*-----------------------------------------------------
                    THE MAIN PROGRAM

 The main routine prints a menu and permits the user to
 select an operation.  It then calls the test routine
 selected (ats() or atst()) or exits.
 ------------------------------------------------------
*/

main() {
 unsigned char huge *st();
 unsigned char huge *en();
 unsigned char huge *start; /* Starting and Ending    */
 unsigned char huge *end;   /* address.               */

 int c, q;     /* Characters being read from keyboard.*/
 int fault_count;

 do
  {menu();
   c = getchar();
   if(c == '1'|| c == '2') {
     start = st(); /* Get starting and ending address.*/
     end = en();
     if(start > end){
      do
       {message(start, end);
        start = st();
        end = en();}
      while(start>end);}
      message(start, end);
     if(c == '1')
       fault_count = ats(start, end); /* Run ATS test */
     else
       fault_count = atst(start, end);/* Run ATS+ test*/
     if(fault_count > 0)
       printf("\n%d Faults detected.\n",fault_count);
     else
       printf("\n\t\tNO FAULTS DETECTED-RAM TESTS OK!");
     printf("\n\n\nDepress any key to go back to the");
     printf(" menu..");
     getch();
         }
   else if(c == '3'){
     clear();
     type();
      }
else
      clear();                        /* clear screen.*/
 }
 while(c != '4');
}
/*--------------------MAIN ENDS-------------------------



                    THE ATS PROCEDURE

This routine tests RAM with the ATS algorithm.  The
routine detects any single or multiple stuck-at-faults
in RAM.  Control is transferred to an error routine if a
fault is detected.  The declarations for start, end, g0,
g1 and g2 are unique to TURBOC.  The algorithm is by J.
Knaizuk, Jr. and C.R.P Hartman, "An Optimal Algorithm
For Testing Stuck-at Faults in Random Access Memories,"
IEEE Trans.  Comput., vol. C-26, pp. 1141-1144, Nov.
1977.
-------------------------------------------------------
*/
ats(start, end)

 unsigned char huge *start;/* Starting and ending     */
 unsigned char huge *end;  /* addresses.              */

 {  /*       g0, g1 & g2 point to each partition.     */
 unsigned char huge *g0;
 unsigned char huge *g1;
 unsigned char huge *g2;

 unsigned char dummy;
 int fault;                            /* fault count */

   fault = 0;

   g0 = start;    /* Starting Address of partition 0. */
   g1 = start + 1;/* Starting Address of partition 1. */
   g2 = start + 2;/* Starting Address of partition 2. */
/*------------------------------------------------------
 Step 1.   Write 0 in all locations in partitions 1 & 2.
 -------------------------------------------------------
*/

 while((g1 <= end) || (g2 <= end))
   {
      *g1 = 0x00;
      g1 = g1 + 3;
      if(g2 <= end){
        *g2 = 0x00;
        g2 = g2 + 3;  }
   }
/*------------------------------------------------------
 Step 2.   Write 1's in all locations in partition 0.
 -------------------------------------------------------
*/

  while(g0 <= end)
      {
         *g0 = 0xff;
         g0 = g0 + 3;
       }

/*------------------------------------------------------
 Step 3.  Read all locations in partition 1.
 -------------------------------------------------------
*/

   g1 = start + 1;
   while(g1 <= end)
      {
         dummy = *g1;
         if(dummy != 0x00){
            error(dummy, g1, 1);
            fault = fault + 1;}
         g1 = g1 + 3;
      }

/*------------------------------------------------------
 Step 4.   Write 1's in all locations in partition 1.
 -------------------------------------------------------
*/

   g1 = start + 1;
   while(g1 <= end)
      {
      *g1 = 0xff;
       g1 = g1 + 3;
      }

/*------------------------------------------------------
 Step 5.   Read all locations in partition 2.
 -------------------------------------------------------
*/

   g2 = start + 2;
   while(g2 <= end)
      {  dummy = *g2;
         if(dummy != 0x00){
            error(dummy, g2, 1);
            fault = fault + 1;}
         g2 = g2 + 3;
      }


/*------------------------------------------------------
 Step 6.   Read all locations in partitions 0 and 1.
 -------------------------------------------------------
*/

   g0 = start;
   g1 = start + 1;
   while((g0 <= end) || (g1 <= end))
      {
         dummy = *g0;
         if(dummy != 0xff){
            error(dummy, g0, 0);
            fault = fault + 1;}
            g0 = g0 + 3;

         if(g1 <= end){
           dummy = *g1;
           if(dummy != 0xff){
             error(dummy, g1, 0);
             fault = fault + 1;}
           g1 = g1 + 3;  }
      }

/*------------------------------------------------------
 Step 7.   Write 0 into each location in partition 0 and
 read the location.
 -------------------------------------------------------
*/

   g0 = start;
   while(g0 <= end)
      {
         *g0 = 0x00;
         dummy = *g0;
         if(dummy != 0x00){
            error(dummy, g0, 1);
            fault = fault + 1;}
         g0 = g0 + 3;
      }

/*------------------------------------------------------
 Step 8.   Read all locations in partition 2.
 -------------------------------------------------------
*/

   g2 = start + 2;
   while(g2 <= end)
      {
         *g2 = 0xff;
         dummy = *g2;
         if(dummy != 0xff){
            error(dummy, g2, 0);
            fault = fault + 1;}
         g2 = g2 + 3;
      }


   return(fault);     /* Return the total # of faults.*/
}
/*------------------------------------------------------





                   THE ATS+ PROCEDURE

This routine tests RAM with the ATS+ algorithm.  The
routine detects any single or multiple stuck-at-faults
in RAM.  Control is transferred to an error routine if a
fault is detected.  The declarations for start, end, g0,
g1 and g2 are unique to Turbo C.  The algorithm is by R.
Nair, "Comments on An Optimal Algorithm for Testing
Stuck-at Faults in Random Access Memories," IEEE Trans.
Comput., vol.  C-28, pp.  258- 261.  Mar.  1979.
--------------------------------------------------------
*/
atst(start, end)

 unsigned char huge *start;/* Starting and ending     */
 unsigned char huge *end;  /* addresses.              */

 { /* g0, g1 and g2 partition memory into three parts.*/
 unsigned char huge *g0;
 unsigned char huge *g1;
 unsigned char huge *g2;

 unsigned char dummy;
 int fault;                            /* fault count */

   fault = 0;

   g0 = start;    /* Starting Address of partition 0. */
   g1 = start + 1;/* Starting Address of partition 1. */
   g2 = start + 2;/* Starting Address of partition 2. */
/*------------------------------------------------------
 Step 1.   Write 0 in all locations in partition 1.
 -------------------------------------------------------
*/

   while((g1 <= end) )
      {
         *g1 = 0x00;
         g1 = g1 + 3;
      }

/*------------------------------------------------------
 Step 2.   Write 0 in all locations in partition 2.
 -------------------------------------------------------
*/

   while((g2 <= end) )
      {
         *g2 = 0x00;
         g2 = g2 + 3;
      }

/*------------------------------------------------------
 Step 3.   Write 1's in all locations in partition 0.
 -------------------------------------------------------
*/

   while(g0 <= end)
      {
         *g0 = 0xff;
         g0 = g0 + 3;
      }

/*------------------------------------------------------
 Step 4.   Read all locations in partition 1.
 -------------------------------------------------------
*/

   g1 = start + 1;
   while(g1 <= end)
      {  dummy = *g1;
         if(dummy != 0x00){
            error(dummy, g1, 1);
            fault = fault + 1;}

         g1 = g1 + 3;
      }

/*------------------------------------------------------
 Step  5.   Write 1's in all locations in partition 1.
 -------------------------------------------------------
*/

   g1 = start + 1;
   while(g1 <= end)
      {
         *g1 = 0xff;
         g1 = g1 + 3;
      }





/*------------------------------------------------------
 Step 6.   Read all locations in partition 2.
 -------------------------------------------------------
*/

   g2 = start + 2;
   while(g2 <= end)
      {  dummy = *g2;
         if(dummy != 0x00){
            error(dummy, g2, 1);
            fault = fault + 1;}

         g2 = g2 + 3;
      }

/*------------------------------------------------------
 Step 7.   Write 1's in all locations in partition 2.
 -------------------------------------------------------
*/

   g2 = start + 2;
   while((g2 <= end) )
      {
         *g2 = 0xff;
         g2 = g2 + 3;
      }

/*------------------------------------------------------
 Step 8.   Read all locations in partition 0.
 -------------------------------------------------------
*/

   g0 = start;
   while( (g0 <= end))
      {  dummy = *g0;
         if(dummy != 0xff){
            error(dummy, g0, 0);
            fault = fault + 1;}
         g0 = g0 + 3;
      }

/*------------------------------------------------------
 Step 9.   Write 0 in all locations in partition 0.
 -------------------------------------------------------
*/

   g0 = start;
   while(g0 <= end)
      {
         *g0 = 0x00;
         g0 = g0 + 3;
      }

/*------------------------------------------------------
 Step 10.   Read all locations in partition 1.
 -------------------------------------------------------
*/

   g1 = start + 1;
   while(g1 <= end)
      {
         *g1 = 0xff;
         dummy = *g1;
         if(dummy != 0xff){
            error(dummy, g1, 0);
            fault = fault + 1;}
         g1 = g1 + 3;
      }

/*------------------------------------------------------
 Step 11.   Write 0 in all locations in partition 1.
 -------------------------------------------------------
*/

   g1 = start + 1;
   while(g1 <= end)
      {
         *g1 = 0x00;
         g1 = g1 + 3;
      }

/*------------------------------------------------------
 Step 12.   Read all locations in partition 2.
 -------------------------------------------------------
*/

   g2 = start + 2;
   while(g2 <= end)
      {  dummy = *g2;
         if(dummy != 0xff){
            error(dummy, g2, 0);
            fault = fault + 1;}

         g2 = g2 + 3;
      }

/*------------------------------------------------------
 Step 13.    Read all locations in partition 0.
 -------------------------------------------------------
*/

   g0 = start  ;
   while( (g0 <= end))
      {  dummy = *g0;
         if(dummy != 0x00){
            error(dummy, g0, 1);
            fault = fault + 1;}

         g0 = g0 + 3;
      }

   return(fault);              /* Return fault count. */

}

/*----------------------------------------------------*/



/*------------------------------------------------------
 Prints a menu on the screen and prompt the user to
 select an option.
 -------------------------------------------------------
*/
menu()
 {

  clear();                         /*  Clear screen.  */

  printf("\n\n\n\n\n\t\t\tTHE RAM TESTER\n");
  printf("\n\n\n\t\t1. TEST RAM USING THE ATS ");
  printf("PROCEDURE\n");

  printf("\t\t2. TEST RAM USING THE ATS+ PROCEDURE\n");
  printf("\t\t3. A BRIEF DESCRIPTION OF THE PROGRAM\n");
  printf("\t\t4. EXIT THE RAM TESTER\n\n\n\n");
  printf("\tPlease enter the option number of your");
  printf(" choice.\n");

 }

/*------------------------------------------------------
 This function makes a far pointer and assigns it to
 start.
 -------------------------------------------------------
*/
unsigned char huge *st()
{

 unsigned char huge *start;

 int seg1; /* Segment value of the starting address.  */
 int off1; /* Offset value of the starting address.   */

   printf("\n Starting address segment = ");
   seg1 = hex();    /* Get segment of the starting
                       address.*/
   printf(" offset = ");
   off1 = hex();    /* Get offset of the starting
                       address.*/
   start = MK_FP(seg1, off1);/* Make far pointers.
                        MK_FP is a Turbo C macro that
                        makes far pointers from its
                        segment and offset components.*/
   return(start);      /* Return the starting address.*/
  }

/*------------------------------------------------------
 This function makes a far pointer and assigns it to
 end.
 -------------------------------------------------------
*/
unsigned char huge *en()
{

 unsigned char huge *end;

 int seg1;     /* Segment value of the ending address.*/
 int off1;     /* Offset value of the ending address. */

   printf("\n Ending address segment   = ");
   seg1 = hex(); /* Get segment of the ending address.*/

   printf(" offset = ");
   off1 = hex();  /* Get offset of the ending address.*/

   end = MK_FP(seg1, off1);    /* Make far pointers.  */
   return(end);          /* Return the ending address.*/
  }

/*------------------------------------------------------
 This function gets a four character hexadecimal number
 from the keyboard.  It expects lead zeros.  The string
 is converted to binary and returned. This function also
 checks to see if the segment/offset is 4 characters
 long and is a valid hexadecimal number.  It gives an
 error message and lets the user enter the
 segment/offset value again if the segment/offset is not
 4 characters or if any of the characters are not
 hexadecimal.
 -------------------------------------------------------
*/
 int hex()
 {

 int n;                           /* Converted value. */
 int count;                            /* Error flag. */
 char c;        /* Characters read from the keyboard. */

   n = 1;
 do
  {
   count = 0;
   while((((c=getch())>='0'&&c<='9') ||(c>='A'&&c<='F')
                   ||(c>= 'a'&& c<='f') )&&count <4)
      {
         putch(c);    /* Outputs character to screen. */
         count = count + 1;
         if(c >= '0' && c<= '9')
            n = 16*n + c - '0';
         if(c >= 'A' && c <= 'F')
            n = 16*n + 10 + c - 'A';
         if(c>='a' && c <= 'f')
            n = 16*n + 10 +c -'a';
      }
   if(count!=4) {
      printf("\n ERROR.   \nPlease enter a");
      printf(" 4-character hexadecimal number:");}
      }
   while(count<4);

   return(n);
}

/*-----------------------------------------------------
 Function message() displays a message to wait if the
 memory under test is large.
 ------------------------------------------------------
*/
message(start, end)

unsigned char huge *start; /* Starting and Ending    */
unsigned char huge *end;   /* address.               */

{  if(end > start + 512 )
      printf("\n\n Please wait.  \n");
   if(start > end){
     clear();
     printf("\n\nError in assigning addresses.\n");
     printf("Starting address > Ending address\n\n");}
 }

/*------------------------------------------------------
 This function is system dependent.  It clears the
 screen.
 -------------------------------------------------------
*/
clear()
{
 system("cls");                       /* clear screen.*/
}

/*------------------------------------------------------
 This function displays the data in file 'desc.dat' on
 the screen.  Twenty lines of text are displayed at a
 time.  The user can press any key to display more text.
 -------------------------------------------------------
*/
type()
{

 FILE *fp, *fopen();    /* File pointer declarations. */
 char line[MAXVAL];     /* Lines read from the file.  */
 int count, letter, i;  /* Flags to indicate error.   */

  count = 0;  i = 0;

/*               Open data file desc.dat.             */
 do
  {i = i + 1;
   if((fp = fopen("desc.dat", "r")) == NULL){
     printf("\nError opening desc.dat\n");
     printf("Insert new disk and press return\n");
     printf("or enter Q to quit\n\n");
     letter = getch();
     putch(letter); }}
  while((letter!='Q'&&letter!='q'&&fp==NULL)&&(i<3));
  if(fp==NULL && letter != 'Q'&& letter != 'q'){
     printf("\n\nSorry file not found...\n");
     getch();}
  if(fp != NULL){
     while(fgets(line, MAXVAL, fp) != NULL){
        count = count + 1;
        printf("%s", line);
        if(count == 20){
           getch();
           clear();
           count = 0;} }
     getch();
     fclose(fp);}
  return(1);
}

/*------------------------------------------------------
 This routine prints an error message during the
 execution of the ats() and atst() functions.  The user
 may decide whether to continue the calling routine,
 abort the program or look at memory using a debug
 program.
 -------------------------------------------------------
*/
error(a, b, error_type)

char a;                /* Contents of memory location.*/
unsigned char huge *b; /* Pointer to the memory
                          location.*/
int error_type;      /* Type of error, S-a-0 or S-a-1.*/
 {

  int j;          /* Character read from the keyboard.*/

   do
   {  clear();
      printf(" Please enter one of the options \n");
      printf("\n%x   = content at location %Fp", a, b);
      printf("\n Error in address (segment:offset) =");
      printf(" %Fp - stuck at %d\n\n", b, error_type);
      printf("\t 1. Ignore the error and continue.\n");
      printf("\t 2. Examine memory with debug. \n");
      printf("\t 3. Quit.  \n ");
      j = getch();
   }
   while(j!='1' && j!='2' && j!='3');

   if(j == '2')
      deb();
   if(j == '3')
      exit(1);

   return(1);    /* Return to calling routine to ignore
                    the fault and continue with the
                    test.  */

}

/*------------------------------------------------------
 Function deb() helps the user to use the MS DOS debuger
 (debug) to look at the memory.
 -------------------------------------------------------
*/
deb()
 {
   printf("\n\n\n");
   printf("At the '-'prompt enter '-dsegment:offset'");
   printf(" 'offset' of ");
   printf(" the block \nof memory to be viewed\n");
   printf("For example -d8000:0000 000f to view ");
   printf("locations or -q to quit\n\n");
   system("debug");
  }
