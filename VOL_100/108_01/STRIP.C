#include bdscio.h
#define INFILE 1
#define OUTFILE 2
/*
Usage: STRIP <infile> <outfile>

This program strips the high order bit from each byte in <infile> and
creates <outfile> from the stripped data.
*/

main(argc, argv)

int argc;
char *argv[];

{
int i;
int input;
char wrkbuf[BUFSIZ];
char wrkbuf2[BUFSIZ];

i=0;


if (argc!=3) {
	prtuse();
	return;
	}

if (fopen(argv[INFILE],wrkbuf)==ERROR){
	printf("\nCan't open %s",argv[INFILE]);
	prtuse();
	return;
	}
if (fcreat(argv[OUTFILE],wrkbuf2)==ERROR){
	printf("\nCan't open %s",argv[OUTFILE]);
	prtuse();
	return;
	}
printf("\n             characters processed\r");
while(((input=getc(wrkbuf))>0)&&(putc((input & 0x7f), wrkbuf2)!=ERROR)){
	printf("%d\r",++i);
	}
if (putc(CPMEOF,wrkbuf2)<0){ 
	printf("\nCan't write end of file marker in %s",argv[INFILE]);
	}
if((fclose(wrkbuf)<0)) {
	printf("\nCan't close %s",argv[INFILE]);
	}
if(((fflush(wrkbuf2)<0)||(fclose(wrkbuf2)<0))){
	printf("\nCan't close %s",argv[OUTFILE]);
	}
}

prtuse()
{
printf("\nUSAGE: strip <infile> <outfile>");
}
