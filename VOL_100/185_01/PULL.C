/* Program to extract 1 file from a Novosielski .lbr file
 * Tom Burnett      10/08/85
 */

#define	VERSION		1
#define REVISION	0
#define MOD_DATE	"10/08/85"

/* This program will extract a member of a LBR file...

   USE: PULL <library> <member>
   COMPILE/LINK: cc pull.c
		 as pull.asm
		 ln pull.o c.lib
  Based on Ltype by S. Kluger 01-13-83
and
   Vers 3.0   3-1-84
   Upgrade to lattice c ms dos P. H. Mack.
   Change to Aztec C 10/08/85 Tom Burnett
*/

#include "stdio.h"

char curdsk, fcb[36];
char fnam[12], libnam[16], dirbuf[128], *dirp;
int  fd, dirsiz, filsiz;
FILE *fp1,*fopen();

#define OK	0
#define ERROR -1

/************************************************
 main
*************************************************/

main(argc,argv)
int argc;
char **argv;
{
	fprintf(stderr,"\nPULL vers:%d.%02d  %s\n\r\n",
	VERSION,REVISION,MOD_DATE);

	if (argc != 3) {
		fprintf(stderr,"usage PULL LIBRARY.LBR FILENAME\n\n");
		exit(1);
		}
	opnlib(argv[1]);

	if (fndmem(argv[2]) == ERROR) erxit("\nMember not in LBR file!\n\r");

	fprintf(stderr,"\nFile '%s' present - %d sectors.\n",argv[2],filsiz);
	fprintf(stderr,"extracting file to default drive....\n");

	if ((fp1 = fopen(argv[2],"w")) == NULL) {
		fprintf(stderr,"cannot create -> %s\n",argv[2]);
		exit(1);
		}

	extract();
}

/************************************************
 Extract function 
*************************************************/

extract()

{
	int	j;
	int	c;

	dirsiz = filsiz;
	do
	{
		reload();

		for (j=0; j<128; j++){

			fputc(*dirp,fp1);
			dirp++;
		}
	}while(dirsiz != 0);
}

/************************************************
 open library file
*************************************************/


opnlib(file)
char *file;
{
	char l, *npnt;

	strcpy(libnam,file);
	l = matchr(libnam,'.');
	if (l == 0) strcat(libnam, ".LBR");
	fcbinit(libnam,fcb);	/* build name in fcb */
	movmem(fcb,fnam,12);	/* get from dfcb2 for log */

	if(strcmp(fcb+9, "LBR")){
		fprintf(stderr,"got %s\n",libnam);
		erxit("Invalid file spec, MUST be type .LBR\n");
	}

	fd = open(libnam,0x8000);
	if(fd == -1) erxit("Library file not found.\n");
}

/************************************************
 find library member
*************************************************/

fndmem(file)
char *file;
{
	char dnam[16], fname[36];
	long int	floc;

	fcbinit(file,fname);
	read(fd,dirbuf,128);
	dirp = dirbuf;
	dirsiz = *(dirp+14);
	dirp += 32;

	do{
		if (*dirp == 255) return(ERROR);
		if (*dirp == 0){
			strcpy(dnam, dirp+1);
			dnam[11]=0;
			if(strcmp(dnam, fname+1) == 0){
				filsiz = (*(dirp+14)) + ((*(dirp+15)) * 256);
				floc=(*(dirp+12)) + ((*(dirp+13)) * 256);
				lseek(fd,floc *128,0);
				return(OK);
			}
		}
		dirp += 32;
		if(dirp > dirbuf+128) reload();
	}
	while(dirsiz);
	return(ERROR);
}

/************************************************
 reload
*************************************************/

reload()
{
	read(fd,dirbuf,128);
	dirp = dirbuf;
	dirsiz--;
}

/************************************************
 match char
*************************************************/

matchr(st,ch)
char *st,ch;
{
	int i;
	for(i=0; st[i]; i++){
		if(st[i] == ch) return(i);
	}
	return(0);
}

/************************************************
 error exit
*************************************************/

erxit(strg)
char *strg;
{
	fprintf(stderr,strg);
	exit();
}
