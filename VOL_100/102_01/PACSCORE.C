/* Scoring for CPM pacman				*/
#include "pacdefs.h"

/* get score file:					*/

gscore()
 {	int fid;
	char *cc;
	fid = open(SCFILE, 0);
	if (fid >= 0)
	 { read(fid, &Scores, 4);	/* 4 128-byte blocks */
	   close(fid);
	   UpdSc = 0; }
	else
	 { cc = &Scores;
	   for (fid=512; fid--; *cc++ = 0);
	   UpdSc = 1; }
 }

/* Write out score file, iff UpdSc==1				*/

wscore()
 {	int fid;
	if (UpdSc == 0) return;
	if ((fid = creat(SCFILE)) < 0) return;
	write(fid, &Scores, 4);
	close(fid);
	UpdSc = 0;
 }

/* Enter current score, iff its sufficiently high:			*/

escore(s)
 unsigned s;
 {
	char i, *cc, *dd;
	for (i=0; i<8; i++)
	 { if (s > Scores[i].Score)
		{ dd = &(Scores[7]);
		  cc = &(Scores[i]);
		  printf("\r\nCongratulations,");
		  printf(" you made the number %d score!\r\n", i+1);
		  if (i < 7) while (dd-- != cc) dd[64] = *dd;

		  Scores[i].Score = s;
		  dd = &(Scores[i].Name);
		  printf("Your name, for the record: ");
		  cc = dd;
		  while (1) switch (i = 0177&getchar())
			{ case '\n':	case '\r':
				*cc++ = 0; UpdSc = 1; goto disp;
			  case 0177:
			  case '\b':    if (cc == dd) continue;
					--cc;
					putch('\b'); putch(' '); putch('\b');
					continue;
			  default:	if (i < ' ') continue;
					putch(i);
					*cc++ = i; }	
		}

	 }
disp:	printf("\r\n\n");
	printf("           TOP EIGHT GAMES:                       \r\n");
	for (i=0; i<8; i++)
	if (Scores[i].Score)
	 printf("%3d.  %5d  %s\r\n", i+1, Scores[i].Score, Scores[i].Name);
	sleep(2);
 }
