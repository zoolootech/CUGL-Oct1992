/* XSHOWR.C   VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:37:51 PM */
/*
%CC1 $1.C -X -O -E5500
%CLINK $1 XSHOWFNS WILDEXP -F CHARFUN STRFUN -S -E5500
*/
/* 
Description:

Display wildcard filelist, using wordstar-like commands:
	move forward and back in a file, by line, screen or 2K block;
	go to start or end of a file;
	return to last position in file;
	tab screen display back and forth;
	search for a string (wildcards and control chars allowed);
	single or continuous repeat of last command;
	display filelist and select a file from it for display;
	move one file forward or back in filelist.

See help_mess for instructions.

Output and input is through bios, to allow for special handling through
functions b_getchar() and b_putchar().

Note: 
	header file XSHOW.H
	function file XSHOWFNS.C/CRL


Speed tests:
				FCAS	XSRCH	XSHOW	Wordstar
			      (machine    (C code)	
			        code)		
Search of 108K file 
for "262 OF" in last block.	44"	85"	106"	 20"

X-QQ-1 on 108K file				41"
= time for read of each block
of file, sequentially, into buffer

D-QQ-1 on 108K file				131"
= rate of read and display of characters	800 char/second

XSHOW can be speeded up by ca 20% by use of in_line code instead of
function calls to pntr_change() in find() and search(), to use_check()
in display_file(), etc.


The following functions are more or less extensive modifications 
of code from public domain programs:
	search(), find(), and decode():
		adapted from Van Nuys SEARCH, by E. H. Mallory.
	wildexp(): 
		adapted from the Van Nuys and original BDS-C versions.


By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/


	/* page eject */

#include <bdscio.h>

#include <xshow.h>


main(argc, argv)
int argc;
char **argv;
{
	int y;
	int string_compare();

	*pattern = *input_pattern = *old_pattern = '\0';
	buffer[0] = '\0';
	start_addr = &buffer[1];
	mid_addr = start_addr + NSECS * SECSIZ;

	if (wildexp(&argc, &argv) == ERROR)
		argc = 0;

	for (x = argc - 1; x > 0; x--)
		if (bad_type(argv[x]))
		{
			for (y = x; y < (argc - 1); y++)
				argv[y] = argv[y + 1];
			argc--;
		}

	if (argc < 2)
	{
		help_mess();
		exit();
	}

	qsort(&argv[1], argc - 1, 2, &string_compare);

	for (x = 1; x < argc; x++)
	{
		setup(argv);
		if (setjmp(jbuf))
			error_reset();
		while (display_file(argc, argv))
			;
	}
}


        /*page eject*/

int display_file(argc, argv)
int argc;
char **argv;

{
	int temp;
	char *k_mess, *k_err_mess, *srch_mess;

	if ((disp_pntr != last_pntr) || (top_flag != last_flag) || (disp_sec != last_sec))
	{
		old_sec = last_sec;
		last_sec = disp_sec;
		old_flag = last_flag;
		last_flag = top_flag;
		old_pntr = last_pntr;
		last_pntr = disp_pntr;
	}

	if (!use_last && *file)
	{
		pause_mess();
		if ((c1 = toupper(b_getchar())) < 0x20)
			c1 += 0x40;
	}
	else
		c1 = old_c1;

	switch (c1)
	{
		/* help */
	case 'H' :
		help_mess();
		break;

		/* - line */
	case 'S' :
		gotop_display(0);
		if (backup(1))
		{
			disp_pntr = start_addr;
			disp_screen();
			break;
		}
		CUR_TOP;
		INSERT_LINE;
		disp_skip(1);
		CUR_BOTTOM;
		ERA_EOL;
		backup(1);
		break;

		/* + line */
	case 'D' :
		gobot_display(0);
		CUR_BOTTOM;
		ERA_EOL;
		disp_skip(1);
		break;

		/* - screen */
	case 'R' :
		backup(TLENGTH);
		gotop_display(1);
		break;

		/* + screen */
	case 'C' :
		if (gobot_display(1))
			gotop_display(1);
		break;

		/* - block */
	case 'E' :
		down_block();
		disp_screen();
		break;

		/* + block */
	case 'X' :
		up_block();
		disp_screen();
		break;

		/* tab forward */
	case 'F' :
		tabcount += TAB_SPACES;
		gotop_display(1);
		break;

		/* tab back */
	case 'A' :
		if ((tabcount -= TAB_SPACES) < 0)
			tabcount = 0;
		gotop_display(1);
		break;

		/* single repeat */
	case 'L' :
		use_last = 1;
		return 1;

		        /*page eject*/

		/* K prefix */
	case 'K' :
		if (*file)
			k_mess = ":::K-prefix mode: enter X,K,B,N,O,H....";
		else
		    k_mess = ":::K-prefix mode with NO OPEN FILE: enter --ONLY-- X,N,O,H....";
		if (!use_last)
		{

			delay_mess(k_mess);
			pause_mess();
			if ((c2 = toupper(b_getchar())) < 0x20)
				c2 += 0x40;
		}
		else
			c2 = old_c2;
		k_err_mess = '\0';
		switch (c2)
		{
			/* help */
		case 'H' :
			help_mess();
			break;

			/* display file list */
		case 'O' :
			file_list(argc, argv);
			break;

			/* exit */
		case 'X' :
			exit();

			/* open next file */
		case 'K' :
			if (*file && (argc > (x + 1)))
				return 0;
			else
			    k_err_mess = ":::No next file...";
			break;

			/* open previous file */
		case 'B' :
			if (*file && (x > 1))
			{
				x -= 2;
				return 0;
			}
			else
			    k_err_mess = ":::No previous file...";
			break;

			/* select file from file_list */
		case 'N' :
			delay_mess(":::Enter file_list ordinal (1 to %d) + -cr- terminator (current file = %d)...", argc - 1, x);
			pause_mess();
			n1 = atoi(gets(string1));
			if (n1 && n1 < argc)
			{
				x = n1 - 1;
				return 0;
			}
			else
			    k_err_mess = ":::File ordinal out of range...";
		}

		CUR_BOTTOM;
		ERA_EOL;
		printf(k_err_mess);
		break;

		        /*page eject*/

		/* Q prefix */
	case 'Q' :
		if (!use_last)
		{
			delay_mess(":::Q-prefix mode: enter Q,P,R,C,X,E,F,H....");
			pause_mess();
			if ((c2 = toupper(b_getchar())) < 0x20)
				c2 += 0x40;
		}
		else
			c2 = old_c2;
		switch (c2)
		{
			/* help */
		case 'H' :
			help_mess();
			break;

			/* search for string */
		case 'F' :
			if (!use_last)
			{
				if (read_pattern())
					break;
				gotop_display(0);
				set_srch(FIRST, SRCH_CHAR);
				set_srch(NEXT, SRCH_CHAR);
			}
			else
			    {
				set_srch(FIRST, NEXT);
			}
			CUR_BOTTOM;
			ERA_EOL;
			if (find() == FOUND)
			{
				set_srch(SRCH_CHAR, FIRST);
				srch_display();
				srch_mess = ":::Search string FOUND...";
			}
			else
			{
				set_srch(NEXT, SRCH_CHAR);
				if (disp_screen())
					gotop_display(1);
				srch_mess = ":::Search string NOTFOUND...";
			}
			CUR_UP;
			ERA_EOL;
			printf(srch_mess);
			break;

			/* multiple repeats */
		case 'Q' :
			use_last = -1;
			sleep_base = 2187;
			return 1;

			/* return to previous file position */
		case 'P' :
			disp_sec = old_sec;
			change_load(0);
			disp_pntr = old_pntr;
			top_flag = old_flag;
			gotop_display(1);
			break;

			/* start file */
		case 'R' :
			load_buffer();
			disp_screen();
			break;

			/* end file */
		case 'C' :
			change_load(end_sec / NSECS + 2);
			disp_screen();
			break;

			/* + or - n blocks */
		case 'X' :
		case 'E' :
			if (!use_last)
			{
				delay_mess(":::Enter # of blocks to be skipped + -cr- terminator...", argc - 1);
				pause_mess();
				n1 = atoi(gets(string1));
			}
			else
				n1 = old_n1;
			if (!n1)
				break;
			switch (c2)
			{
			case 'E' :
				down_block();
				temp = (1 - n1);
				break;

			case 'X' :
				up_block();
				temp = (n1 - 1);
				break;
			}
			if (temp)
				change_load(temp);
			disp_screen();
			break;
		}

		break;


		/*if no proper c1-switch character, then refresh display */
	default :
		gotop_display(1);
		break;
	}

	        /* page eject */

	/* tests on exit from switches */
	if (!use_last)
	{
		old_c1 = c1;
		old_c2 = c2;
		old_n1 = n1;
	}
	else
	    if (use_last > 0)
		use_last--;
	else
	    use_check();

	return 1;
}


void use_check()
{
	int sleep_temp;

	if (sleep_base)
	{
		sleep_temp = sleep_base;
		while (sleep_temp--)
			if (t || KBHIT)
			{
				use_input();
				break;
			}
	}
	if (!sleep_base)
		use_input();
}


void use_input()
{
	int temp;

	if (isdigit(temp = b_getchar()))
	{
		if (temp -= '0')
		{
			sleep_base = 3;
			while (--temp)
				sleep_base *= 3;
		}
		else
		    sleep_base = 0;
	}
	else
	    {
		t = temp;
		use_last = 0;
	}
}


        /* page eject */

void setup(argv)
char **argv;
{
	tabcount = 0;
	old_pntr = last_pntr = disp_pntr;
	old_flag = last_flag = top_flag;
	old_sec = last_sec = disp_sec;
	use_last = 0;
	c1 = c2 = n1 = 0;
	old_c1 = 'H';
	*string1 = '\0';

	close(fd);
	strcpy(file, argv[x]);
	if ((fd = open(file, 0)) == ERROR)
		error("Cannot open file.");
	seek(fd, 0, 2);
	end_sec = tell(fd);
	load_buffer();
	disp_screen();
}


void error_reset()
{
	old_c1 = 'K';
	*file = '\0';
	use_last = 0;
}


void file_list(argc, argv)
int argc;
char **argv;
{
	int i;

	CLEAR;
	for (i = 1; i < argc; i++)
	{
		if ((i % 60) == 1)
			printf("FILE_LIST:  Use ordinal number in <KS#> selection of file for display\n\n");
		printf("%3d  %-20s", i, argv[i]);
		if (!(i % 3))
			printf("\n");
		if (!(i % 60))
		{
			pause_mess();
			if ((t = b_getchar()) == CR)
				t = 0;
			else
				return;
			CLEAR;
		}
	}
	pause_mess();
}

