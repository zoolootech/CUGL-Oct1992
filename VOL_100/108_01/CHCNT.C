
/*
    This program counts the frequency of each ASCII character
        (0-127) in the list of files given as arguments, 
	and prints the total number of characters in the files.
	If the total number of characters exceeds 65535 (the
	maximum value for "unsigned" integers in C), then the
	wrong total will be printed. Oh well.
	CHCNT.C has been included mainly as an example of
	buffered file I/O. See TELNET.C for another example.

        written by L. Zolman   12/21/78 (Season's Greetings!!)  */

unsigned table[255];
char ibuf[128];

char *ascy;

char *badfile;  /* message on error from "open" */
char *readerr; /* message on read error */

main (argc,argv)
char **argv;
int argc;
{
        int n;
        int fi;
        int i;
	ascy = "     ";
        badfile = "chcnt: cannot  open "; 
        readerr = "chcnt: read error from  ";
	for (i=0; i<256; i++) table[i]=0;

        if (argc-- == 1) {
                printf("I need some filenames for this! \n");
                exit (0);
         }
        while (argc--) {
                if ((fi=open(*++argv,0)) < 0) {
                        error (*argv,1);
                        continue;
                 }
                while ((n=read(fi,ibuf,1)) > 0)
                        for (i=0; i<128; i++) table[ibuf[i]]++;

		if (n == -1) error(*argv,2);
		close(fi);
         }
        display();
}


error(name,code)
char *name;
int code;
{
        if (code==1) printf("%s  ",badfile);
        if (code==2) printf("%s  ",readerr);
        printf("%s\n\n",name);
}

display()
{
        int i;
        int j;
        int k;
        char *cnvt();
        unsigned count;
	count = 0;

        printf("\n\n");
        for (i=1; i<=8; i++) printf ("ch cnt    ");
        printf("\n");
        for (i=1; i<=8; i++) printf ("-- ---    ");
        printf("\n\n");

        for (i=0; i<=15; i++) {
	for (j=i,k=0; k<8; k++,j+=16) {
                        count += table[j];
                        printf ("%3s: %3u ",cnvt(j), table[j]);
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
                case 0: return("NU");
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


 together with C.CCC (for common system
	subroutines) and any subordinate functions which
	"main" may require (from perhaps ma