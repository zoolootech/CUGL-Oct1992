/*									      */
/*  Program:	 Demo, Version 01/20/86 				      */
/*									      */
/*  Description: Demonstrate calling by EGA routines using FAR calls.	      */
/*									      */
/*  Author:	 Kent Cedola						      */
/*		 2015 Meadow Lake Court, Norfolk VA, 23518. 1-(804)-857-0613  */
/*									      */
/*  Language:	 Microsoft C 4.0+					      */
/*									      */

#include <stdio.h>
#include <mcega.h>


void InitGraphics()
{

  GPPARMSX();			       /* Sets up all global variables */

  if (GDTYPE == 4)		       /* Give monochrome user bad news */
    {
    perror("Sorry, must have a Color Display not monochrome!\n");
    exit(1);
    }
  else if (GDTYPE != 5) 		/* Tell non-EGA users no can run */
    {
    perror("Enhanced Color Adapter and Display not found!");
    exit(2);
    };

  if (GDMEMORY == 64)			/* We need lots of EGA memory	*/
    {
    perror("This program will work much better with 128k+ EGA memory!\n");
    perror("       Hit any key to continue!\n");
    getch();
    };

  GPINITX();			       /* We are now in graphic mode!  */


}

void TermGraphics()
{

  GPTERMX();			       /* Terminate graphic mode       */

}

main(argc, argv)
  int  argc;
  char **argv;
{

  InitGraphics();

  GPCOLORX(WHITE);
  GPMOVEX(0,0);
  GPBOXX(GDMAXCOL,GDMAXROW);

  GPCOLORX(RED);

  GPMOVEX(0, 0);
  GPLINEX(639, 349);

  getch();

  TermGraphics();
}
