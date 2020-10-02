#include "libc.h"

#define TRUE -1
#define FALSE 0
#define MAXFILES 7
/*
Aztec C sets up eleven streams.  stdin, stdout, stderr are three.  One more is
allowed for here to permit stdout to be redirected to a file.  This leaves
seven available for zip.
*/

main(argc,argv)
int argc; char *argv[];
{
	int c, i, o, was;
	FILE *f[MAXFILES];	/* An array of pointers to FILE */

	if (argc < 3){
		fprintf(stderr,"Usage: zip file1 file2 [file3 ... file7]\n");
		fprintf(stderr,"Output is to stdout.\n");
		(argc > 1 && !strcmp(argv[1],"?")) ? exit(0) : exit(1);
			/* exit(1) halts any active submit */
	}
	if (argc > MAXFILES + 1){
		fprintf(stderr,"Too many files to zip.  Eliminating %d file(s) from end of list.\n", argc - (MAXFILES + 1));
		argc = MAXFILES + 1;
	}

	o = argc - 1;
	for (i = 0; i < argc - 1; i++){		/* open files for zipping */
		if ((f[i] = fopen(argv[i+1],"r")) == NULL){
			fprintf(stderr,"Bad filename: %s\n", argv[i+1]);
			exit(1);
		}
	}

	for (;;){		/* Loop closed and broken out of later */
		was = FALSE;
		for (i = 0; i < argc - 1; i++){
			if (f[i]){
				if ((c = agetc(f[i])) != EOF){
/* If null lines are special, move to here 'if (was) putchar(' ');' */
					if (c != '\n'){
						if (was)
							putchar(' ');
						do {
							putchar(c);
							if ((c = agetc(f[i])) == EOF){
								fclose(f[i]);
								f[i] = NULL;
								o--;
								break;
							}
						} while (c != '\n');
					}
/* If null lines are special, place here 'else putchar('#');' or somesuch */
					was = TRUE;
				} else {
					fclose(f[i]);
					f[i] = NULL;
					o--;
				}
			}
		}

		if (was)
			putchar('\n');

		if (!o)		/* Break out if no more open files */
			break;
	}
}
o--;
				}
			}
		}

		if (w