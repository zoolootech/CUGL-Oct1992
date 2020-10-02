#include <210ctdl.h>
/*****************************************************************************/
/*				expand.c				     */
/*									     */
/*			Expands a msg file.  V.1.2			     */
/*****************************************************************************/

/*****************************************************************************/
/*				history 				     */
/*									     */
/* 84Dec09 HAW	Now merely expands a file, rather than copying & expanding.  */
/* 84Jun22 HAW	Version 1.1 created -- handles any file expansion.	     */
/* 84Jun19 HAW	Created.						     */
/*****************************************************************************/

/*****************************************************************************/
/*				contents				     */
/*									     */
/*	copy_remainder()	Copies remainder of split msg into file      */
/*	first_part()		Remembers remainder of split msg	     */
/*	main()			Main controller for this program	     */
/*****************************************************************************/

/*****************************************************************************/
/*				External files				     */
/*									     */
/*				210modem.c				     */
/*				210rooma.c				     */
/*				210roomb.c				     */
/*				210log.c				     */
/*				210modem.c				     */
/*				210misc.c				     */
/*				210msg.c				     */
/*									     */
/*	The files cited above must be linked in -- additionally, this file   */
/*	must be compiled using the same -e address as Citadel.		     */
/*****************************************************************************/

/*****************************************************************************/
/*	copy_remainder()	copies the remainder of the split msg onto   */
/*				the "end" of the file			     */
/*****************************************************************************/
unsigned copy_remainder(offset, nfd)
unsigned offset;
int nfd;
{
    char *beg;
    int  count, i;

    beg = codend();
    count = 0;
    printf("Transfer %d sectors to cover split msg\n",
	      (offset) ? offset / SECTSIZE + 1 : 0);
    while (1) {
	if (offset < SECTSIZE) break;
	crypte(beg + SECTSIZE * count, SECTSIZE, 0);
	if (write(nfd, beg + SECTSIZE * count, 1) != 1)
	    exit(printf("\nError on write: %s", errmsg(errno())));
	count++;
	offset -= SECTSIZE;
    }
    if (offset == 0) return count;
    for (i = 0; beg[SECTSIZE * count + i] != 255; i++) ;
    for (i++; i < SECTSIZE; i++)
	beg[SECTSIZE * count + i] = 0;
    crypte(beg + SECTSIZE * count, SECTSIZE, 0);
    if (write(nfd, beg + SECTSIZE * count, 1) != 1)
	exit(printf("\nError on write: %s", errmsg(errno())));
    return count + 1;
}

/*****************************************************************************/
/*	first_part()	remembers remainder of first msg		     */
/*****************************************************************************/
unsigned first_part(startat)
char *startat;
{
    int  offset, i;
    char done, buf[SECTSIZE];

    offset = 0;
    done   = FALSE;
    do {
	if (read(msgfl, buf, 1) != 1)
	    exit(printf("\nError on read: %s", errmsg(errno())));
	crypte(buf, SECTSIZE, 0);
	i = 0;
	while (buf[i] != 255 && i < SECTSIZE)
	    startat[offset++] = buf[i++];
	done = !(i == SECTSIZE);
    } while (!done);
    seek(msgfl, 0, 0);
    return offset;
}

/*****************************************************************************/
/*	main()	the main controller					     */
/*****************************************************************************/
main()
{
    char     *msgFile, temp[BUFSIZ];
    int      s, i, offset, setup();
    char     *buf;
    unsigned bufs, bufad, oldsize, fudge;

    printf("%u buffer space\n", externs() - codend());
    printf("Citadel Message expansion program V1.2.\n");
    printf("HAVE YOU BACKED UP THE CTDLMSG.SYS FILE YET(Y/N)? ");
    if (toUpper(getCh()) != 'Y') {
	printf("\nTHEN DO SO, IDIOT!");
	exit();
    }
    printf("\n\nOne moment, please...\n");
    if (readSysTab()) {
	msgFile     = "a:ctdlmsg.sys";
	*msgFile   += msgDisk;
	openFile(msgFile, &msgfl);

	haveCarrier = FALSE;
	onConsole   = TRUE;
	whichIO     = CONSOLE;
	oldsize     = maxMSector;

	printf("\nOld size was %dK", maxMSector / 8);
	maxMSector = getNumber("\nNew size (in decimal!)", maxMSector/8, 65535);
	maxMSector *= 8;
	printf("\nThank you.  Working...");
	bufad = buf = codend() + (offset = first_part(codend()));
	bufs = externs() - 1 - bufad;
	if (bufs < SECTSIZE)
	    exit(printf("Not even 128 bytes are available!"));

	writeSysTab();		/* Restore again, don't have to reconfigure */

	seek(msgfl, oldsize, 0);	/* Get to EOF */
	while (read(msgfl, temp, 1) == 1) printf("Looping\n");
	fudge = copy_remainder(offset, msgfl);
	for (i = 0; i < SECTSIZE; i++) buf[i] = 0;
	crypte(buf, SECTSIZE, 0);
	i = maxMSector - oldsize - fudge;
	printf("And now %7d sectors left to initialize\n", i);
	for (; i; i--) {
	    printf("%7d\r", i);
	    if ((s = write(msgfl, buf, 1)) != 1)
		exit(printf("\nError on write: %s", errmsg(errno())));
	}
	close(msgfl);
	printf("\nFinished.  Don't need to reconfigure!  But don't forget");
	printf(" to change CTDLCNFG.SYS.");
    }
}
ctor;

	printf("\nOld size was %dK", 