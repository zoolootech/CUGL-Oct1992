/*********************************************************************\
** .---------------------------------------------------------------. **
** |                                                               | **
** |                                                               | **
** |         Copyright (c) 1981, 1982, 1983 by Eric Martz.         | **
** |                                                               | **
** |                                                               | **
** |       Permission is hereby granted to use this source         | **
** |       code only for non-profit purposes. Publication of       | **
** |       all or any part of this source code, as well as         | **
** |       use for business purposes is forbidden without          | **
** |       written permission of the author and copyright          | **
** |       holder:                                                 | **
** |                                                               | **
** |                          Eric Martz                           | **
** |                         POWER  TOOLS                          | **
** |                    48 Hunter's Hill Circle                    | **
** |                      Amherst MA 01002 USA                     | **
** |                                                               | **
** |                                                               | **
** `---------------------------------------------------------------' **
\*********************************************************************/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*
	RAPREAD.C

		find_portion()             <- commented out
		open_read(new)
		read_command(line)
		io_port(fp)
		baud(fp)
		screen_status()

*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "rap.h"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* BEGIN COMMENTING OUT OF find_portion <<<<<<<<<<<<<<<<<<<<<<<<<<<

For information on the "boiler-plate" assembly code formerly included
in RAP, contact Eric Martz. In addition to find_portion() there are
two programs, MAKEMENU which makes a menu from a text file properly
demarkated into portions, and RAPMENU which RAPS a document of portions
selected in an arbitrary order in the menu made by MAKEMENU.

find_portion(){

/*
Old_portion IS THE PORTION FOR WHICH READING WAS MOST RECENTLY COMPLETED.
New_portion IS THE PORTION TO BE READ NEXT.
*/
/* UPON RETURN, NEXT CALL TO rapgets(...,Fpin) WILL GET THE FIRST LINE
	OF THE PORTION New_portion.
	
	POSSIBLE ENTRY CONDTIONS:
	
		Old_portion = 0; MUST READ FROM BEGINNING OF FILE UP TO New_portion.

		Old-portion POSITIVE BUT < New_portion; MUST READ FROM PORTION
		FOLLOWING Old_portion UP TO New_portion.
		WHEN END OF Old_portion (PORTIT_C) WAS ENCOUNTERED IN anotherline(),
		READING WAS CONTINUED UNTIL PORTIT_C.
		
		[IF Old_portion WAS >= New_portion, IT WAS ALREADY SET TO 0 AND
		FILE WAS REOPENED BY read_command().]
*/
	char discard[BIGBUF];
	int i, portion;

	i = In_main; /* used to hold subread linecnt - see anotherline() */
	In_main = In_linecnt;
	In_linecnt = i;
	Fpin = Fp_read;

	if (Old_portion AND (New_portion EQ (Old_portion+1)))
		/* READY TO CONTINUE WITH FILE ALREADY OPEN */
		return(YES);

	if (Old_portion EQ 0) Old_portion = -1;
	for (portion=(Old_portion+1); portion < New_portion; portion++) {
		discard[0] = NULL;
		while ((instr(0,discard,PORTIT_O)) EQ ERROR) {
			if (!rapgets(discard, Fpin)) {
notenough:
				fprintf(STDERR,"Not enough portions in input file.\n");
				exit(0);
			}
		}
		while ((instr(0,discard,PORTIT_C)) EQ ERROR)
			if (!rapgets(discard, Fpin)) goto notenough;
	}
}

>>>>>>>>>>>>>>>>>>>>>>>>>>>>> END COMMENTING OUT OF find_portion */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
open_read(new)
	char *new;
	{
	if (Fp_read) {
		fpfree(Fp_read);
		Fp_read = 0;
/*		Old_portion = 0;*/
	}
	if (!(Fp_read = fopen(new, "r-"))) {
		fprintf(STDERR,"File %s does not exist. Ignoring .re command ");
		return(NO);
	}
	strcpy(Fn_read, new);
	Fpin = Fp_read;
	In_main = In_linecnt;
	In_linecnt = 0;
	return(YES);
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
read_command(line)
	char *line;
	{
	char newname[FILENAME];
	int i;
/*	int newportion;*/

	i = findwords(line+3, Pp);
	if (i <= 0 OR i >= 3) { /* i must be 1 or 2 */
		fprintf(STDERR,"Invalid .re command ");
badread:
		fprintf(STDERR,"at line %d.\n", In_linecnt);
		return(0);
	}
	strcpy(newname, Pp[1]);
	upper(newname);
	if (badname(newname)) {
		fprintf(STDERR,"Invalid filename in .re command ");
		goto badread;
	}
/*	if (i EQ 2) {
		if (ati(&newportion, Pp[2]) OR newportion < 1) {
			fprintf(STDERR,"Invalid portion number \"%s\" in .re command ",
				Pp[2]);
			goto badread;
		}
	}
	else newportion = 0;
*/
	if (Fpin NE Fp_main) {
		fprintf(STDERR,"Error: second level .re command ");
		goto badread;
	}
	if ( /*!newportion AND*/ !(open_read(newname))) goto badread;
/*	else { /* portion */
		if (!Fp_read OR !(equal(newname, Fn_read))
		OR newportion <= Old_portion)
			if (!open_read(newname)) goto badread;
	}
	New_portion = newportion;
*/
	if (!Screen_size) {
		fprintf(STDERR,"\tNow reading \"%s\"", Fn_read);
/*		if (newportion) fprintf(STDERR," portion %d", New_portion);*/
		fprintf(STDERR,"\n");
	}
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
io_port(fp)
	FILE *fp;
	{
	int val[8], i;

	rapgets(Inbuf, fp);
	de_comment(Inbuf);
	blankout(Inbuf,",");
	if ((findwords(Inbuf, Pp)) NE 7) {
ioerr:
		fprintf(STDERR,"Error: invalid i/o port specifications\n");
		exit(0);
	}
	for (i=1; i<=7; i++) {
		if (ati(&val[i],Pp[i])) goto ioerr;
		if (val[i] < 0 OR val[i] >= 256) goto ioerr;
	}
	Pdata = val[1];
	Pstatus = val[2];
	Pimask = val[3];
	Pomask = val[4];
	Preset = val[5];
	Presetval = val[6];
	Pready = val[7];
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
baud(fp)
	FILE *fp;
	{
	int val[3], i;
	char buf[MAXLINE];

	rapgets(buf,fp);
	
	FOREVER {
		rapgets(Inbuf,fp);
		if (!*Inbuf) break;
		de_comment(Inbuf);
		blankout(Inbuf, ",");
		if (findwords(Inbuf,Pp) NE 2) {
berr:
			fprintf(STDERR,"Error in baud specification.\n");
			exit(0);
		}
		for (i=1; i<=2; i++) {
			if (ati(&val[i], Pp[i])) goto berr;
			if (val[i] <0 OR val[i] >255) goto berr;
		}
		outp(val[1],val[2]);
	}
	fprintf(STDERR,"%s\n",buf);
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
screen_status() {
	char buf[MAXLINE];
	
	if (!Screen_size) return(0);
	if (Lines_out < Screen_size) goto addone;
	Lines_out = 0;
	sprintf(buf,
		" At line %d of %s ",
		In_linecnt, (Fpin EQ Fp_main)? Fn_main: Fn_read);
	if (Rmval > 75) strcat(buf," space->next screen; ^C->quit ");
	pad(buf, 'l', Rmval - ((Rmval - 1 - strlen(buf))/2), '*');
	pad(buf, 'r', Rmval - 1, '*');
	fprintf(STDERR,"%s\n",buf);
	while(!bdos(CON_STATUS));
	if (bdos(CONIN) EQ CTRL_C) {
		Interrupted = YES;
		printf("\n\n\t--- RAP ABORTED WITH ^C ---\n");
		if (Deinit) puts (Deinit);
		dioflush();
		exit(1);
	}
	puts("\b \b"); /*  SOME CRT'S DO NOT HAVE DESTRUCTIVE BACKSPACE */
addone:
	Lines_out++;
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* END OF RAPREAD.C */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
al[1],val[2]);
	}
	fprintf(STDERR,"%s\n",buf);
}
/* - - - - - - - - - - - - - - - - - - - - - - - - -