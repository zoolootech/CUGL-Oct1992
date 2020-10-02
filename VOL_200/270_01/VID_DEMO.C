/* HEADER: CUG270;
   TITLE: Video Demonstration
   DATE: 04/11/1988
   DESCRIPTION: Demonstration of various video routines
   VERSION: 1.0
   KEYWORDS: VIDEO, BIOS FUNCTIONS
   FILENAME: VID_DEMO.C
   WARNINGS: Dependant on ROM BIOS
   SEE-ALSO: VIDEO.H
   SYSTEM: MS-DOS
   COMPILERS: Turbo C & Microsoft C v4.0
   AUTHORS: Marc Bosser

	Comments, questions, bugs & suggestions
        Monday, Wedndesday, Friday, Sunday
	Contact ME: Marc Bosser
		    1922 Julia Goldbach
		    Ronkonkoma, NY 11779

*/

		/************************************************/
		/*   File Name:VID_DEMO.C				   */
		/*   C File:3,101 bytes					   */
		/*   Object File:1,673 bytes				   */
		/*   Portability:VANILLA but depends on VIDEO.H */
		/*									   */
		/* CONTAINS: MAIN						   */
		/*									   */
		/* Total: 1 routine						   */
		/* External: cprintf, getch				   */
		/* User Defined: xy,fill,ewriteM			   */
		/************************************************/

#include "DOS.H"			/* NEEDED TO COMPILE VIDEO.H(int86)*/
#include "CONIO.H"			/* NEEDED TO COMPILE cprintf,getch */
#include "VIDEO.H"			/* CONTAINS ALL VIDEO FUNCTIONS	*/

/******************************************************************************/

#ifdef __TURBOC__
void main(void)
#else
main()
#endif

{
 unsigned short loop=1;
 unsigned int delay=0;
 unsigned short color=0;
 unsigned int y1=22;
 unsigned int x1=1;
 unsigned int length=60;
 unsigned int x2=24;

 fill(1,1,80,25,7);		/* this effectively clears the screen in gray */
 xy(1,1);
 cprintf("Lets fill the screen will cprintf\n\r");
 do {
    cprintf("ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл\n\r");
    /* In quotes are 79 ASCII characters 219 */
    loop++;
 } while (loop < 24);
 cprintf("HIT ANY KEY TO CONTINUE");
 getch();
 fill(1,1,80,25,7);
 xy(1,1);
 cprintf("Lets color the screen with fill");
 loop=1;
 do	{
	y1-=3;
	x1+=2;
	length-=2;
	color=loop*16;
	fill(y1,x1,length,x2,color);
	loop++;
	for (delay=0; delay < 32000; delay++) ;;
 } while (loop < 8);
 cprintf("\n\rHIT ANY KEY TO CONTINUE");
 getch();
 loop=1;
 do	{
	fill(1,1,80,25,7);
	xy(1,1);
	cprintf("Have you ever needed to turn the cursor off during input.\n\r");
	cprintf("Well this could be your chance all it  takes  is  a  call\n\r");
	cprintf("to cursoroff a routine that locates the  cursor  at  0,25\n\r");
	cprintf("a non existant position.\n\r");
	if (loop == 1)
		{
		xy(1,7);
		cprintf("That cursor hanging around is just unprofessional.\n\r");
		}
	cprintf("\n\nHIT ANY KEY TO CONTINUE");
	if (loop == 2) cursoroff();
	getch();
	loop++;
 } while (loop < 3);
 fill(1,1,80,25,7);
 xy(1,1);
 cprintf("Have you ever needed a function that can write any amount of any character.\n\r");
 cprintf("Now you have it, it is called ewriteM\n\r");
 loop=3;
 do	{
	ewriteM(1,loop,loop+44,70);
	loop++;
 } while (loop < 24);
}

/******************************************************************************/

				/**** END OF VID_DEMO.C ****/
