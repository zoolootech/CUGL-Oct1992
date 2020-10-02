#include <210ctdl.h>	/* header file	*/

main()
{
    int  index;
    int i, entry, lstfl, pw;
    char obuf[BUFSIZ];
    
    readsystab();
    if ((logfl = open("ctdllog.sys",0)) == ERROR) {
	printf("Can't open the Citadel log!\n");
	exit();
    }
    pw = lstfl = 0;
    entry = 1;
    for ( i = 0; i < MAXLOGTAB; i++ ) {
	printf("log #%d",i);
	if (logTab[i].ltpwhash != 0 &&
	    logTab[i].ltnmhash != 0) {
	    getlog(&logBuf,logTab[i].ltlogSlot);
	    showlog(&logBuf,entry++,lstfl,obuf,pw); 
	}
	else {
	    putchar('\r');
	}
    }
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
/SECSPERLOG);
	exit(printf("\n%s",