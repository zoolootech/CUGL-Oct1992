#define TITLE	"Display contents of the CITADEL log.	 v2.1"
/**
***	This program allows inspection of the Citadel log
***	displaying misc things about the user.
***	Modified by H.A. White, May 2, 1984.
**/

#include <210ctdl.h>	/* header file	*/

main(argc,argv)
char **argv;
{
    int  index;
    int i, entry, lstfl, pw;
    char obuf[BUFSIZ];
    
    printf("%s\n",TITLE);
    readsystab();
    if ((logfl = open("ctdllog.sys",0)) == ERROR) {
	printf("Can't open the Citadel log!\n");
	exit();
    }
    pw = lstfl = 0;
    index = 1;
    while (argc >= 2) {
	if (argv[index][0] == '-') {
	    if (argv[index][1] == 'D' || argv[index][2] == 'D') {
		printf("Listing Log entries in: CTDLLOG.LST\n");
		if ( (lstfl = fcreat("CTDLLOG.LST",obuf)) == ERROR) {
		    printf("Can't create CTDLLOG.LST file!\n");
		    exit();
		}
		fprintf(obuf,"%s\n",TITLE);
	    }
	    if (argv[index][1] == 'P' || argv[index][2] == 'P') {
		printf("Listing Passwords.\n");
		pw = TRUE;
	    }
	}
	argc--;
	index++;
    }
    entry = 1;
    for ( i = 0; i < MAXLOGTAB; i++ ) {
	getlog(&logBuf,i);
	if (lstfl)
	    fprintf(obuf,"log #%d",i);
	printf("log #%d",i);
	if (!nogood(i))
	    showlog(&logBuf,entry++,lstfl,obuf,pw); 
	else {
	    putchar('\n');
	    if (lstfl)
		fprintf(obuf,"\n");
	}
    }
    if (lstfl) {
	putc(CPMEOF,obuf);
	fflush(obuf);
	fclose(obuf);
    }
}

nogood(s)
{
    int i;

    for (i = 0; i < MAXLOGTAB; i++)
	if (logTab[i].ltlogSlot == s)
	    break;
    if (i == MAXLOGTAB) return TRUE;
    if (logTab[i].ltpwhash != 0 &&
	logTab[i].ltnmhash != 0)
	return FALSE;
    return TRUE;
}

showlog(log,i,lst,buf,pw)
struct logBuffer *log;
int i,lst,pw;
char *buf;
{
    int j;
    char work[80],work2[80];
    
    sprintf(work,"%4d: %-20s",i,log->lbname);
    if (pw) {
	sprintf(work2,"%-20s",log->lbpw);
	strcat(work,work2);
    }
    sprintf(work2," %sAide, %sExpert, %d col.\n",
	log->lbflags & AIDE   ? "    " : "Not ", 
	log->lbflags & EXPERT ? "    " : "Not ",
	log->lbwidth);
    strcat(work,work2);
    printf("%s",work);
    if (lst)
	fprintf(buf,"%s",work);
}

/*
 *	getLog() loads requested log record into RAM buffer
 */
getlog(lBuf, n)
struct logBuffer *lBuf;
int n;
{
    int sec;
    if (lBuf == &logBuf)
	thisLog = n;

    n *= SECSPERLOG;

    seek(logfl, n, 0);
    if ( (sec = read(logfl, lBuf, SECSPERLOG) ) != SECSPERLOG ) {
	printf("?getLog-rread fail, request was for %d", n/SECSPERLOG);
	exit(printf("\n%s", errmsg(errno())));
    }
    crypte(lBuf, (SECSPERLOG*SECTSIZE), n);	/* decode buffer	*/
}

dumpsec(p)
char *p;
{
    char c;
    int i,j;

    for ( i = 0; i < 8; i++) {
	for (j = 0; j < 16; j++)
	    printf(" %02x",p[i*16+j]);
	printf("  |");
	for (j = 0; j < 16; j++) {
	    c = p[i*16+j];
	    if (c < ' ' || c > 128)
		putchar('.');
	    else
		putchar(c);
	}
	printf("|\n");
    }
    putchar('\n');
}

crypte(buf, len, seed)
char	  *buf;
unsigned  len, seed;
{
    seed = (seed + 0x553) & 0xFF;
    for ( ; len; len--) {
	*buf++ ^= seed;
	seed = (seed + CRYPTADD)  &  0xFF;
    }
}

/*
**   readSysTab() restores state of system from SYSTEM.TAB
*/
readSysTab()
{
    char getc();
    char fBuf[BUFSIZ];
    char *c;

    if(fopen("ctdlTabl.sys", fBuf) == ERROR)
	exit(printf("?no ctdlTabl.sys!"));

    getw(fBuf);
    getw(fBuf);
    c = &firstExtern;
    while (c < &lastExtern)
	*c++ = getc(fBuf);
    return(TRUE);
}
fBuf);
    return(TRUE);
}
             