/*	B.C

	Version 1.1	23-Aug-80

	This program prints out a sorted directory listing similar
	to that that would be produced by a combination of STAT and
	LIST. It is a modification of BIGDIR.C (written by Richard
	Damon) designed for a Heath/Zenith H8 or H89/Z89. It is
	configured for an H8/DEC VT100 in a 3-drive CP/M 1.43
	environment, but it may be easily re-configured by changing
	the commented definitions. For example, to configure it for
	an H8/H19(ANSI) in a 2-drive CP/M 1.42 or 1.43:

	   1.	#define	EXIT_STRING	  "\n\033G\033<"
	   2.	#define	HEADER_IN_STRING  "\n\033[?2h\033p\033F"
	   3.	#define	HEADER_OUT_STRING "\n\033q"
	   4.	#define	NO_OF_DRIVES	  2
	   5.	#define	VERTICAL_LINE	  '\140'

	protocol:

		A>B	sorted directory of logged drive
		A>B X	sorted directory of drive X
		A>B *	sorted directory of all drives

	To configure it for non-Heath systems, the block which reads
	the directory will have to be changed to read the correct
	track with the correct skew. For standard CP/M, make the
	following changes in addition to any re-configuration:
	
	   1.	#define	CAPACITY	  241	
	   2.	#define	DIRECTORY_TRACK	  2
	   3.	#define	SECTORS_PER_TRACK 26
	   4.	#define	SKEW		  6

	   5.	replace the 'switch(sector)' block
		with:
			if(sector==1) sector=2;

	To configure it for 4 drives, make the following changes:

	   1.	#define	NO_OF_DRIVES	  4

	   2.	int index; (was 'char')

	   3.	in the setup and sort directory block:
		   1) change 'C' to 'D'
		   2) add the following immediately above:
			else if(index<NO_OF_EXTENTS*4)
			   directory[index].unused[1]='C';
*/

#include <b:stdef.c>
#include <b:system.c>

#define	ALL		    '*'
#define	CAPACITY	    90
#define	DIRECTORY_TRACK	    3
#define	ERASED		    0xE5
#define	EXIT_STRING	    "\n\033(B"		/* exit_graphics    */	
#define	FCB_LENGTH	    32
#define	HEADER_IN_STRING    "\n\033[1;7m\033(0"	/* reverse_video
						   & enter_graphics */
#define	HEADER_OUT_STRING   "\n\033[m"		/* normal_video	    */
#define	LENGTH_OF_DIRECTORY 16
#define NO_OF_DRIVES	    3			/* drives in system */
#define	NO_OF_EXTENTS	    64
#define	SECTORS_PER_TRACK   20
#define	SKEW		    8
#define	VERTICAL_LINE	    '\170'		/* graphics vertical	
						   line             */

#define	SETTRK		    10
#define	SETSEC		    11
#define	SETDMA		    12
#define	READ		    13

main(argc,argv)
int argc;
char *argv[];
{
	struct fcb {
	   char entry_type;
	   char name[11];
	   char extent;
	   char unused[2];
	   char records;
	   char diskmap[16];
	} directory[NO_OF_EXTENTS*NO_OF_DRIVES];

	char drive,entries,extents,index,logged_drive,loop_index;
	char number_of_records,sector,used,wildcard;
	int comp();

/*	process argument		 	*/

	logged_drive=bdos(INTER_DISK,0);
	if((wildcard=**++argv)==ALL) loop_index=0;
	else {
	   (argc==1) ? drive=logged_drive : drive=wildcard-'A';
	   bdos(SELECT_DISK,drive); loop_index=NO_OF_DRIVES-1;
	   wildcard=RESET;
	   }

/*	read directory		 	 	*/

	entries=RESET;
	do {
	   if(wildcard) bdos(SELECT_DISK,loop_index);
	   sector=1; bios(SETTRK,DIRECTORY_TRACK);
	   index=RESET;
	   do {
	      bios(SETSEC,sector); bios(SETDMA,&directory[entries++*4]);
	      bios(READ); sector+=SKEW;
	      if (sector>SECTORS_PER_TRACK) sector-=SECTORS_PER_TRACK;
	      switch(sector) {
	         case 7:  sector=4; break;
	         case 2:  sector=3; break;
	         case 1:  sector=2;
	         }
	      } while (++index<LENGTH_OF_DIRECTORY);
	   } while (++loop_index!=NO_OF_DRIVES);

/*	setup and sort directory		*/

	if(wildcard) {
	   index=RESET; entries=NO_OF_EXTENTS*NO_OF_DRIVES;
	   do {
	      if(directory[index].entry_type!=ERASED) {	
	         if(index<NO_OF_EXTENTS) directory[index].unused[1]='A';
	         else if(index<NO_OF_EXTENTS*2)
		    directory[index].unused[1]='B';
	         else directory[index].unused[1]='C';
	         }
	      } while (++index<entries);
	   }
	else entries=NO_OF_EXTENTS;
	qsort(directory,entries,FCB_LENGTH,comp);

/*	print listing header	 	 	*/

	index=RESET; printf(HEADER_IN_STRING);
	do {
	   printf("FILENAME.EXT    NR  K");
	   if(index==2) {
	      if(wildcard) printf("  :");
	      break;
	      }
	   wildcard ? printf("  :   ") : printf("   ");
	} while (++index<3);
	printf(HEADER_OUT_STRING);

/*	print out sorted directory	 	*/

	index=used=RESET;
	do {
	   switch(index%3) {
	      case 0: putchar('\n'); break;
	      case 1:
	      case 2: printf("%c ",VERTICAL_LINE);
	      }
  	   drive=directory[index].unused[1];
	   extents=directory[index].extent;
	   number_of_records=directory[index].records;
	   directory[index].unused[0]=EOS; loop_index=10;
	   do {
	      directory[index].name[loop_index+1]
		 =directory[index].name[loop_index];
	      } while (--loop_index>7);
	   directory[index].name[8]='.'; used+=(number_of_records+7)/8;
	   printf("%s",directory[index].name);
	   extents ? printf("+%d",extents) : printf("  ");
	   printf(" %3d %2d ",number_of_records,(number_of_records+7)/8);
	   if(wildcard) printf(" %c ",drive);
	   } while (!directory[++index].entry_type);

/*	print tail,select original drive	*/

	printf(EXIT_STRING);
	if (!wildcard) printf("\nfree: %dK\n",CAPACITY-used);
	bdos(SELECT_DISK,logged_drive);
}

/*	comparison function for 'qsort'		*/

comp(x,y)
char *x,*y;
{
	char index;
	
	for(index=1;(index||*x)&&*x!=ERASED&&*x==*y;
	   x++,y++,index=RESET);
	if(*x>*y) return  1; if(*x<*y) return -1; return 0;
}
