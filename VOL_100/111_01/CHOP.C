/*
HEADER:		;
TITLE:		Chop;
VERSION:	1.1;
DATE:		10/27/1985;

DESCRIPTION:	"Removes sectors from the beginning of a file,
		and creates a new file containing the remaining data.";

KEYWORDS:	File, utility;
SYSTEM:		CP/M-80;
FILENAME:	CHOP.C;
AUTHORS:	Unknown;
COMPILERS:	BDS C;
*/
/************************************************************************

	Ver. 1.0: Author and date unknown.
	Version 1.1, October 27, 1985:
		Sign-on message modified to provide instructions
		if the user formats the command line incorrectly.
		John M. Smith, CUG librarian, Utilities IV diskette.

	Usage: A>CHOP file1 file2 n

		file1 is input, file2 is created, and
		"n" is the number of records to "chop".

*************************************************************************/

 int nskip, recnum;
 int infd, outfd;
 char buf[128];

main(argc,argv)
int argc; char *argv[];
{

 puts("\nFile Chopper, V1.1\n");

 if (argc != 4)
  { puts("Removes sectors from the front of a file.\n\n");
    puts("Usage: CHOP file1 file2 n\n\n");
    puts("file1 is input, file2 is created, and\n");
    puts("n is the number of sectors to remove.\n");
    exit(); }

 if ((nskip = atoi(argv[3])) == 0)
  { puts("No records to skip\n"); exit(); }

 /* open files */

 if ((infd = open(argv[1],0)) == -1)
  { printf("%s%s","File not found: ",argv[1]); exit(); }

 if ((outfd = creat(argv[2])) == -1)
  { printf("%s%s","Cannot create: ",argv[2]); exit(); }

 printf("Skipping %u records\n", nskip);
 for (recnum = 0; recnum < nskip; recnum++)
  { if (read(infd,buf,1) <= 0)
     { printf("%s %u","File read error at record",recnum); exit(); }
  }

 while (read(infd,buf,1) > 0)
  { if (write(outfd,buf,1) == -1)
     { printf("Output file error at record %u",recnum); exit(); }
  }

 puts("Done\n");

 close(outfd);

 }
