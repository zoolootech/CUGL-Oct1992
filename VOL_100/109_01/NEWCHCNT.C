/*
This program reads an input ascii file and compiles statistics
on all possible char. values (0-255).
To run, type: chcnt filename.ext <cr>.
The original program was from an earlier BDS Distribution
disk.  I have cleaned up the existing bugs and added the
BigFio buffered file input routines.
The main bug was chcnt read past the EOF of a file to the
physical end of the sector.  This added some extra counts!
This thing is usefull in making sure that the # of left (,{,
and [ equals the # of right ),} and ].
*/
#define NB 32     /*  32*28 = 4K Input buffer  */
#define CPMEOF -1
#define EOF 26
unsigned table[255];
char ascy[4];
main (argc,argv)
int argc;
char **argv;
{
	char ibuf[NB*128+8];
	int ifd,c,i;
	for (i=0; i<255; i++)
		table[i] = 0;
	ifd = FOpenBig(argv[1], ibuf, NB);
	if (ifd == -1)    {
		printf ("Can't open File!!");
		exit ();
	}
	while ((c = GetcBig(ibuf)) != EOF && c != CPMEOF)
		table[c]++;
	table[c]++;		/*  count EOF also */

	close (ifd);
	display ();
}
display ()
{
	int i,j,k;
	char *cnvt();
	unsigned count;
	count = 0;
	printf("\n\n  ");
	for (i=1; i<=8; i++) printf ("ch  cnt   ");
	printf("\n  ");
	for (i=1; i<=8; i++) printf("--  ---   ");
	printf("\n");

	for (i=0; i<=15; i++)  {
		for (j=i,k=0; k<8; k++,j+=16) {
			count += table[j];
			printf ("%3s: %4u ",cnvt(j), table[j]);
		}
		printf ("\n");
	}
	printf ("\nTotal # of chars read = %u\n",count);
}
char *cnvt(byte)
int byte;
{
	int c;
	c=byte;
	switch(c)
	{
		case 0:  return ("NU");
		case 13: return ("CR");
		case 10: return ("LF");
		case 27: return ("ES");
		case 28: return ("FS");
		case 29: return ("GS");
		case 30: return ("RS");
		case 31: return ("US");
		case 32: return ("SP");
		case 8:  return ("BS");
		case 127:return ("DL");
		default:
			if (c>=1 && c<=26) {
				ascy[0]='^';
				ascy[1]=c+64;
				ascy[2]='\0';
				return(ascy);
			}
			ascy[0]=' ';
			ascy[1]=c;
			ascy[2]= '\0';
			return(ascy);
	}
}
