#include	BDSCIO.H

/**************************************************************/
/* TITLE: CEDIT Utility (Epson MX80 version)		      */
/*							      */
/* ID: CEDIT	    Version 1.1	    30th January 1983	      */
/*							      */
/* PROGRAMMER:  Paolo Prandini				      */
/*		Viale Europa 72/G			      */
/*		25100 Brescia -- Italy			      */
/*							      */
/* DESCRIPTION:	In any Epson MX80/III or Epson MX80 with Graf-*/
/*		trax we can easily notice the presence of 3   */
/*		2716-type EPROMS. The central one contains the*/
/*		character patterns of the main set and also of*/
/*		the national sets: the content of this EPROM  */
/*		is reproduced in the MASTER.CHR file (Please  */
/*		note that from now on we'll refer to every    */
/*		character set file as .CHR, that is the defau-*/
/*		lt forced specification for this type of file)*/
/*		With this program anyone can easily edit his  */
/*		own set (even APL!): a scholar used it to de- */
/*		fine an old Indian language for his Softcard  */
/*		Apple CP/M system!			      */
/*		The edited char set can be saved to disk and  */
/*		later programmed onto a new 2716 EPROM with   */
/*		any suitable hardware ( I used an homebrew    */
/*		programmer ).				      */
/*		COMMANDS AVAILABLE:			      */
/*	X	.E(X)it and reboot CP/M			      */
/*	E	.(E)dit current char pattern		      */
/*		The cursor positions on the upper left corner */
/*		of the character screen area and then the fol-*/
/*		lowing control sequences are accepted:	      */
/*		^S,^H	moves cursor left		      */
/*		^D	moves cursor right		      */
/*		^X	moves cursor down		      */
/*		^E	moves cursor up			      */
/*		Space	clears current dot and moves right    */
/*		0	sets current dot and moves right      */
/*		^Q	exits edit mode and saves edited char */
/*		ESC	exits edit mode without saving modifi-*/
/*			ed character			      */
/*	C	.(C)hange current char waiting for any char   */
/*	N	.Change current char waiting for a two digit  */
/*		hexadecimal (N)umeric value		      */
/*	F	.Change current default (F)ile name	      */
/*	L	.(L)oad char set file using default file name */
/*	S	.(S)ave char set file using default file name */
/*							      */
/* HISTORY:	Cursor addressing sequences for ADM 3A	      */
/*							      */
/**************************************************************/

#define	COLUMN	9	/* Character matrix width and length  */
#define	ROW	8	/* for EPSON MX80		      */

char	matrix[COLUMN][ROW],caract;
char	file_in[32],filename[32],work_file[32];
int	yes_file,yes_exit;

main(argc,argv)
int	argc;
char	**argv;
{
	int	i,j;
	char	c;
	yes_file=FALSE;
	yes_exit=FALSE;
	if (argc>2) {  /* If more than 2 parms then error */
		printf("Use:  CEDIT  charset.CHR\n");
		exit();
	}
	printf(CLEARS); /* Clear screen */
	caract=' ';
	status_line();
	if (argc==2) { /* Get work char set file */
		for (i=0;(c=argv[1][i]) && c!='.';i++)
			file_in[i]=c;
		file_in[i]='\0';
		strcat(file_in,".CHR"); /* Force .CHR */
		strcpy(filename,file_in);
		if ((open_in(file_in))==OK)
			status_line();
	}

	/* Initialize current char matrix */
	for (i=1;i<=COLUMN;++i) for (j=1;j<=ROW;++j) matrix[i][j]=0;

	caract=' ';
	while (yes_exit==FALSE) {
		if (yes_file==TRUE) {
			get_matrix(area());
			out_matrix();
		}
		get_input();
		status_line();
	}
}

edit_matrix()
{
	char	cm;
	int	x,y,txit;
	cm=0;
	x=35;
	y=8;
	setcursor(x,y);
	txit=FALSE;
	while (txit==FALSE) {
		cm=inkey();
		switch(cm) {
		case 24:	
			++y;
			if (y>7+ROW) y=7+ROW;
			break;
		case 05:	
			--y;
			if (y<8) y=8;
			break;
		case 8:
		case 19:	
			--x;
			if (x<35) x=35;
			break;
		case 04:	
			++x;
			if (x>34+COLUMN) x=34+COLUMN;
			break;
		case 32:	
			matrix[x-34][y-7]=1;
			printf("%c",45-2*((x-34)-((x-34)/2)*2));
			++x;
			if (x>34+COLUMN) x=34+COLUMN;
			break;
		case 48:	
			matrix[x-34][y-7]=0;
			printf("O");
			++x;
			if (x>34+COLUMN) x=34+COLUMN;
			break;
		case 27:	
			txit=TRUE;
			break;
		case 17:	
			txit=TRUE;
			put_matrix(area());
			break;
		default:	
			putchar(7);
		}
		setcursor(x,y);
	}
}

inkey() /* Get single char without echo ! */
{
	return(bios(3,0));
}


status_line()
{	
	int	cm;
	init_screen();
	if ((caract<126) & (caract>31)) cm=caract;
	else cm='.';
	setcursor(0,0);
	printf("CEDIT v1.1/Epson MX80   Work File:");
	if (yes_file==TRUE) printf("%-26s",filename);
	else printf("None                      ");
	setcursor(50,0);
	printf("Char %s%c%s  Value %s%-02x%s",
	INTOREV,cm,OUTAREV,INTOREV,caract,OUTAREV);
}

get_input()
{	
	char	opt,filenout[32],s;
	int	value,j;
	setcursor(0,22);
	printf("-- X=Exit  E=Edit  C=Chan  F=File  L=Load  S=Save");
	printf("  N=Numb  ---->            \b\b\b\b\b\b\b\b");
	printf("\b\b\b\b");
	opt=toupper(inkey());
	switch(opt) {
	case 'X':	
		exit();
		break;
	case 'C':	
		setcursor(56,0);
		caract=inkey();
		if (caract<32) caract=' ';
		break;
	case 'N':	
		setcursor(67,0);
		scanf("%x",&value);
		if (value<32) caract=32;
		else caract=value;
		break;
	case 'E':	
		edit_matrix();
		break;
	case 'F':	
		printf("_\b");
		scanf("%s",filenout);
		for (j=0;((s=filenout[j]) && s!='.') ||
						j>strlen(filenout);j++)
			    work_file[j]=toupper(s);
		work_file[j]='\0';
		strcat(work_file,".CHR");
		break;
	case 'L':	
		yes_file=FALSE;
		strcpy(file_in,work_file);
		open_in(file_in);
		if (yes_file==TRUE)
			strcpy(filename,work_file);
		break;
	case 'S':	
		yes_file=FALSE;
		strcpy(file_in,work_file);
		open_out(file_in);
		if (yes_file==TRUE)
			strcpy(filename,work_file);
		break;
	default:	
		putchar(7);
	}
}

area() /* Compute address of working char */
{
	int	s;
	s=(endext()+100+(caract-32)*9);
	setcursor(5,5);
	s=s+4*((s-(endext()+100))>>8);
	return(s);
}

setcursor(x,y) /* Set cursor function , modify for your terminal */
int	x,y;
{
	putchar(ESC);
	putchar('=');
	putchar(y+32);
	putchar(x+32);
}

open_in(file_in) /* Read workfile in memory at endext()+100 */
char	file_in[32];
{
	int	n;
	if ((n=open(file_in,0))==ERROR) {
		setcursor(24,0);
		printf("Can't open:%s",file_in);
		return(ERROR);
	}
	if ((read(n,endext()+100,16))<16) {
		setcursor(24,0);	
		printf("Character set %s invalid",file_in);
		return(ERROR);
	}
	close(n);
	yes_file=TRUE;
	return(OK);
}

open_out(file_in) /* Write workfile present in memory from endext()+100 */
char	file_in[32];
{
	int	n;
	if ((n=creat(file_in))!=ERROR) {
		write(n,endext()+100,16);
		yes_file=TRUE;
	} 
	else {
		setcursor(24,0);
		printf("Can't create:%s",file_in);
	}
	close(n);
}

int exp(n) /* Compute 2 raised to nth power */
int	n;
{
	return(n ? (2*exp(--n)) : 1);
}

get_matrix(pa) /* Load specified memory area into character matrix */
char	*pa;
{
	char	cm;
	char	scratch;
	int	i,j;

	scratch=caract;
	for (i=1;i<=COLUMN;++i) {
		cm=*pa;
		for (j=1;j<=ROW;++j)
			matrix[i][j]=(cm & exp(8-j));
		++pa;
	}
	caract=scratch;
}

put_matrix(pa) /* Write back character matrix to memory */
char	*pa;
{
	char	cm,scratch;
	int	i,j;

	scratch=caract;
	for (i=1;i<=COLUMN;++i) {
		cm=0;
		for (j=1;j<=ROW;++j) {
			if (matrix[i][j]!=0)
				cm=(cm | exp(8-j));
		}
		*pa=cm;
		++pa;
	}
	caract=scratch;
}

init_screen()
{
	int	y;
	for (y=7;y<17;y++) {
		setcursor(33,y);
		printf(INTOREV);
		setcursor(45,y);
		printf(OUTAREV);
	}
}

out_matrix() /* Print out character matrix */
{
	int	i,j,y;
	y=8;
	for (i=1;i<=ROW;++i) {
		setcursor(35,y);
		++y;
		for (j=1;j<=COLUMN;++j) {
			if (matrix[j][i]==0) putchar('O');
			else putchar(45-2*(j-(j/2)*2));
		}
	}
}

