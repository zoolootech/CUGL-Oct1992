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

#include "rap.h"

main(argc, argv)
	int argc;
	char **argv;
	{
	char filename[FILENAME], *ptr;
	int ignore_commands, arg, valid;
#include <fio2.h>

/* EXPLICIT CALLS TO FORCE LINK FROM NDIO, NOT DEFF */
	if (NO) getchar();
	if (NO) putchar();
	if (NO) fopen();
	if (NO) fclose();

	fprintf(STDERR,
"RAP Version 2.21  Copyright (c) 1981, 1982, 1983 by Eric Martz\n\n");

	if (argc  EQ  1) {
		fprintf(STDERR,
			"\nUSAGE:\n\nrap [name.ss] [-draft] [-p#] ");
		fprintf(STDERR,"source_text_file_name\n");
		fprintf(STDERR,
"\t[ ) OR )filename OR )/list OR )/punch OR )/port OR\
 )/screen]\n");
		fprintf(STDERR,
			"\n\t\t\t\t(more than one source_text_file_name OK)\n");
		exit(0);
	}

	init_defaults();
	ignore_commands = NO;

	/* PROCESS '-...' ARGUMENTS AND SS ARGUMENT */

#ifdef DEBUG
	if (argmatch("-DE*", &argc, argv, YES)) {
		Debug = TRUE;
		fprintf(STDERR,"Debug FLAG ON\n");
	}
#endif

	if (argmatch("-DR*", &argc, argv, YES)) {
		Font = 1;
		fprintf(STDERR,"\nDraft Font Selected.\n");
	}
	if (ptr = argmatch("-P*", &argc, argv, YES)) {
		if (ati(&Print_at, ptr+2)) {
			printf("Invalid starting page number after -p\n");
			exit(0);
		}
		fprintf(STDERR, "\nBegin printing at page %d.\n", Print_at);
	}
	if (ptr = argmatch("*.SS", &argc, argv, YES)) {
		if (badname(ptr)) exit(0);
		strcpy(Ss_name, ptr);
	}

	init_ss();

#ifdef DEBUG
	if (Debug) fprintf(STDERR,"After INIT_SS: _Outflag = %d\n", _Outflag);
#endif
	*Out_file = 0;
	if (argmatch("-I*", &argc, argv, YES)) {
		strcpy(Out_file, "INDEX");
		Index = YES;
	}
	if (argmatch("-C*", &argc, argv, YES)) {
		strcpy(Out_file, "CONTENTS");
		Tabcont = YES;
	}
	if (*Out_file) {
		First_text = NO;
		Print_at = 32000;
		_Outflag = 16; /* FILE_OUT */
		if (fcreat("TEMPOUT.$$$", _Dobuf) EQ ERROR) {
			fprintf(STDERR, "\nCan't create %s; disk full?\n",
			Out_file);
			exit(0);
		}
	}
	else dioinit(&argc, argv);	/* directed I/O */

	if (!_Outflag) _Outflag = LIST_OUT;

#ifdef DEBUG
	if (Debug) fprintf(STDERR,"After DIOINIT: _Outflag = %d\n", _Outflag);
#endif

	/* LOOP TO OPEN EACH FILENAME ARGUMENT IN TURN */
	for (arg=1; arg < argc; arg++) {
		strcpy (filename, argv[arg]);		/* get next argument */
nx:
		upper(filename);
		if(badname(filename)) exit(0);
		if (!Screen_size) fprintf(STDERR,
			"\nNow processing \"%s\"\n",filename );
		strcpy(Fn_main, filename);
		Nxname[0] = NULL;
		Fpin = fopen(filename,"r-");	
		if (Fpin EQ 0) {
			fprintf(STDERR,"Can not open \"%s\"\n", filename );
			continue;
		}
		Fp_main = Fpin;
		In_linecnt = 0;

		/* MAIN TEXT PROCESSING LOOP */
goback:
		while (anotherline(Inbuf)) {

#ifdef DEBUG
			if (Debug) checkbig("Main gets anotherline:",Inbuf);
#endif
			/* SKIP DRAFT INPUT LINES */
			if (Indraft AND !Printdraft AND
				(lefteq(Inbuf, ".ed") NE 3)
				) continue;

			valid = YES;
			if (Inbuf[0] EQ COMMAND
				AND
				(!ignore_commands OR (strcmp(Inbuf,".ic 0") EQ 0))) {

				ignore_commands = command (Inbuf, Nxname, &valid);
			}
			else	text (Inbuf);
			if (!valid) { /* process invalid command as text */
				valid = YES;
				text (Inbuf);
			}
		}
#ifdef DEBUG
		if (Debug) fprintf(STDERR,"Anotherline got EOF.\n");
#endif
		if (Fp_read EQ Fpin) {
			if (!Screen_size) fprintf(STDERR,
				"\t...done reading \"%s\"\n", Fn_read);
			Fpin = Fp_main;
			fpfree(Fp_read);
			Fp_read = 0;
/*
			New_portion = 0;
			Old_portion = 0;
*/
			In_linecnt = In_main;
			goto goback;
		}
		if (!Screen_size) fprintf(STDERR,
			"...done with \"%s\"\n", filename );
		fpfree(Fpin);
		if (Nxname[0]) {
			strcpy(filename,Nxname);
			goto nx;
		}
	} /* END OF TEXT FILENAME ARGUMENT PROCESSING LOOP */

	if (Lineno OR Outbuf[0]) space(HUGE);
	dioflush();
	if (Deinit) puts(Deinit);

} /* end main() */

/********************************************************************/

#include <fio3.h>

/********************************************************************/

/********************************************************************
	initializes the global variables governing the execution of the
	 format commands.
********************************************************************/
init_defaults() {
	char *pnull;
	
	/* NOTE: BDSC V. 1.5 ZEROS ALL EXTERNALS, THEREFORE ALL WHICH
	MUST BE SET TO ZERO ARE OMITTED FROM THE LIST BELOW */

	pnull = "";

#ifdef DEBUG
	Debug = NO;
#endif

	Bar = "- - - - - - - - - - - - - - -";
	Blank_c = ' ';
	Bo_on = pnull;
	Bo_off = pnull;
	Bo_shift = pnull;
	Bo_restore = pnull;
	Center_mode = pnull;
	Con_forms = 1;
	Exp_ctrl_chars = YES;
	Fill  =  YES;/* yes we want filled lines */
	First_text = YES;
	Flush_left = pnull;
	Flush_right = pnull;
	Font_c = pnull;
	Ignore_at = YES;
	Just_one = pnull;
	Just_two = pnull;
	Lsval  =  1;/* line spacing  =  1 */
	M1val  =  2;/* top and bottom margins */
	M2val  =  2;
	M3val  =  2;
	M4val  =  2;
	Newpag  =  1;
	Nextout = Outbuf;
	Pdata = -1;
	Pival  =  Inval;
	Plval  =  66;
	Post_ss  = YES;
	Post_ss_delim = '}';
	Pre_ss  = YES;
	Pre_ss_delim = '{';
	Printdraft = YES;
	Rev_lf = pnull;
	Rmval  =  78;/* right margin  =  page width  */
	Sanders = YES; /* assume sanders printer */
	Tabval =  5;
	Ul_on = pnull;
	Ul_off = pnull;
	Ul_restore = pnull;
	Ul_shift = pnull;
	
	strcpy(Footer,"|-#-|");
	strcpy(Ss_name, "DEFAULT.SS");

	Bottom  =  Plval - M3val - M4val;
}

;
#endif

	Bar = "- - - - - - - - - - - - - - -";
	Blank_c = 