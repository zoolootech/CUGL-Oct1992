/*@*****************************************************/
/*@                                                    */
/*@ fdate - get the file date/time stamp and return    */
/*@     it in string form as "YY-MM-DD HH:MM:SS ".     */ 
/*@                                                    */
/*@   Usage:   fdate(fp, buf, fn);                     */
/*@       where fp is a file "handle".                 */
/*@            buf is a buffer to receive the string.  */
/*@            fn is the filename (used in error msg). */
/*@                                                    */
/*@*****************************************************/


extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern char _carryf, _zerof;

char *fdate(fp, buf, fn)
int fp;				/* file pointer */
char *buf;			/* buffer for output */
char *fn;			/* filename (for error message) */
/*
 * Return MM-DD-YY HH:MM:SS for last update for file "fp".
 */
{
	int _doint(), mon, day, year, hour, min, sec;
	char *z_fill(), *itoa();

	buf[0] = 0x00;	/* clear output area */
	_rax = 0x5700;	/* get date/time DOS function */
	_rbx = fp;		/* file handle */
	_rcx = 0;		/* returns time here */
	_rdx = 0;		/* returns date here */
	_doint(0x21);	/* call DOS */

	if (_carryf) {		/* error */
		puts("Unable to get date/time stamp for ", fn);
		return buf;		/* buf is empty at this time */
	}

	mon = (_rdx >> 5) & 0x0f;
	day =  _rdx & 0x1f;
	year = ((_rdx >> 9) & 0x7f) + 80;

	hour = (_rcx >> 11);
	min  = (_rcx >> 5) & 0x3f;
	sec =  (_rcx & 0x1f);

	z_fill(itoa(year, buf), 2);
	buf[2] = '-';
	z_fill(itoa(mon,&buf[3]), 2);
	buf[5] = '-';
	z_fill(itoa(day,&buf[6]), 2);
	buf[8] = ' ';
	z_fill(itoa(hour,&buf[9]), 2);
	buf[11] = ':';
	z_fill(itoa(min,&buf[12]), 2);
	buf[14] = ':';
	z_fill(itoa(sec,&buf[15]), 2);
	buf[17] = ' ';
	buf[18] = 0x00;			/* end of string */

	return buf;		/* allow stacking of functions */
}

