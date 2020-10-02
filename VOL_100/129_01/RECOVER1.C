#include <210ctdl.h>
/*****************************************************************************/
/*				recover1.c				     */
/*									     */
/*		  Recovery program for recovering killed rooms		     */
/*	This only works for room slots which have only been killed; if the   */
/*	slot has been recycled (reclaimed by makeRoom), a different program  */
/*	will be needed for recovery.					     */
/*****************************************************************************/

/*****************************************************************************/
/*				history 				     */
/*									     */
/* 84Jun17 HAW	Seems to work correctly.				     */
/* 84Jun17 HAW	Created.						     */
/*****************************************************************************/

/*****************************************************************************/
/*				contents				     */
/*									     */
/*	main()			Main controller for this program	     */
/*****************************************************************************/

/*****************************************************************************/
/*				External fns				     */
/*									     */
/*	crypte()		210log.c	(indirectly)		     */
/*	Getch() 		210modem.c				     */
/*	getRoom()		210roomb.c				     */
/*	noteRoom()		210roomb.c				     */
/*	openFile()		210rooma.c				     */
/*	putRoom()		210roomb.c				     */
/*	readSysTab()		210rooma.c				     */
/*	setSpace()		210rooma.c				     */
/*	writeSysTab()		210rooma.c				     */
/*									     */
/*	The files cited above must be linked in -- additionally, this file   */
/*	must be compiled using the same -e address as Citadel.		     */
/*****************************************************************************/

/*****************************************************************************/
/*	main()	the main controller					     */
/*****************************************************************************/
main()
{
    int Index;

    printf("Got problems, eh?  Hang on a moment...\n");
    if (readSysTab()) {
	setSpace(homeDisk, homeUser);
	openFile("ctdlroom.sys", &roomfl);

	for (Index = AIDEROOM + 1;		/* First 3 ALWAYS exist */
	     Index < MAXROOMS; Index++) {
	    if (!(roomTab[Index].rtflags & INUSE) && roomTab[Index].rtname[0]) {
		printf("The room named %s may be recoverable. ",
			roomTab[Index].rtname);
		printf("Do you want me to try? ");
		if (toupper(getch()) == 'Y') {
		    getRoom(Index);
		    roomBuf.rbflags |= INUSE;
		    putRoom(Index);
		    noteRoom();
		}
		putChar('\n');
	    }
	}
	printf("\n\nAll finished!  Saving... No need to reconfigure.");
	writeSysTab();
    }
}
   */
/*****************************************************************************/

/***