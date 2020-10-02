/* HEADER: CUG120.05;
   TITLE: CP;
   DESCRIPTION: "File copy utility";
   KEYWORDS: file copy;
   SYSTEM: CP/M;
   FILENAME: CP.C;
   CRC: 9EA7;
   SEE-ALSO: FPUT;
   AUTHORS: Leor Zolman;
   COMPILERS: BDS C;
*/
/*
	Makes a copy of a file in the current directory (easier
	than doing a pip). Written by LZ. Uses the "usercode" library.

	Usage:
		A>cp [user#/][d:]<filename> [user#/][d:][<filename>] <cr>

	Note that the second name is optional; if omitted, attempts to copy
	to the given disk and/or user number. For example, to copy foo.c
	into user area 7:
		cp  foo.c  7/ 

	Link with:
		A>clink cp -f usercode
*/

#define debug 0
#include "bdscio.h"

main(argc,argv)
char **argv;
{
	int i,j,c;
	int fd1,fd2;
	int bufsects;
	unsigned bufsize, topofmem();
	unsigned corebuf;
	int orig_user;			/* original user area */
	int source_user;		/* source user area */
	int dest_user;			/* destination user area */
	char destname[30];


	if (argc != 3) {
		printf("Usages: cp [u/]filename [u/]newname <cr>\n");
		printf("	cp [u/]filename u/ <cr>\n");
		exit();
	}

					/* get current user number */
	source_user = dest_user = orig_user  = bdos(32,0xff);

	if ((fd1 = open(argv[1],0)) == ERROR) {
		printf("Can't open %s\n",argv[1]);
		exit();
	}

	strcpy(destname,argv[2]);
	if (hasuno(destname))
		dest_user = atoi(destname);
	if (hasuno(argv[1]))
		source_user = atoi(argv[1]);

	if ( (c = destname[strlen(destname) - 1])=='/' || c == ':')
	{
		for (i = strlen(argv[1]) - 1; i >= 0; i--)
			if (argv[1][i] == '/' || argv[1][i] == ':')
				break;				
		strcat(destname,&argv[1][i+1]);
	}

	if ((fd2 = creat(destname)) == ERROR) {
  	printf("Can't create %s\n",destname);
		exit();
	}

	corebuf = endext();
	bufsize = topofmem() - 2500 - corebuf;
	bufsects = bufsize / SECSIZ;

#if debug
	printf("topofmem() = %x\n",topofmem());
	printf("corebuf = %x\n",corebuf);
	printf("topofmem() - 2500 - corebuf = %x, ",
		topofmem() - 2500 - corebuf);
	printf("bufsize = %x (%d decimal)\n",bufsize,bufsize);

	printf("bufsects = %d\n",bufsects);
	printf("end of buffer = %x\n",corebuf+bufsize);
	printf("&i = %x\n",&i);
#endif

	printf("	copying...");

	while (1)
	{
		bdos(32,source_user);
		if (kbhit()) getchar();
		if (!(i = read(fd1,corebuf,bufsects))) break;
		bdos(32,dest_user);
		if (kbhit()) getchar();
		if (write(fd2,corebuf,i) != i) {
			printf("Write error. Disk full?\n");
			exit();
		}
	}

	bdos(32,dest_user);
	if (close(fd2) == ERROR) {
		printf("Can't close the output file.\7\n");;
	}
	bdos(32,orig_user);
	fabort(fd1);
	printf("done.");
}

/*
	Return true if the string arg is a filename prefixed by "nn/",
	where "nn" is a user number:
*/

int hasuno(str)
char *str;
{
	char c;
	int sum;

	sum = 0;

	if (!isdigit(*str)) return FALSE;

	while (isdigit(c = *str++))
		sum = sum * 10 + c - '0';
	return (c == '/') ? (sum >= 0 && sum < 32) : FALSE;
}

/
	char destname[30];


	if (argc != 3) {
		printf("Usages: cp [u/]fi