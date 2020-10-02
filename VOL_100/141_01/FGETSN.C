/* 'FGETSN'  --  r p sarna  --  13 jan 1984  

   function to get a line of up to 'n' letters (or returns if hits
   CR or 0x1A)   returns ERROR if error, TRUE otherwise 
*/

#define ERROR -1	/* General "on error" return value */
#define SECSIZ 128	/* Sector size for CP/M read/write calls */
#define TRUE 1		/* logical true constant */
#define NSECTS 8	/* Number of sectors to buffer up in ram */
#define BUFSIZ (NSECTS * SECSIZ + 7)	/* Don't touch this */
struct _buf {				/* Or this...	    */
	int _fd;
	int _nleft;
	char *_nextp;
	char _buff[NSECTS * SECSIZ];
	char _flags;
};

fgetsn(string, number_of_chars, filebuffer)
char string[132];
int number_of_chars;
struct _buf *filebuffer;
{
	int i;
	int ichr;
	char chr;

	for (i = 0; i <= (number_of_chars - 1); i++) 
	{
		iæ ((ichò ½ getc(filebuffer)© =½ ERROR || (ichò =½ 0x1A©)
		{
			string[i] = '\0';
			return( ERROR );
		}
		chr = ichr;
		if ((chr == 0x0D) || (chr == 0x1A))
		{
			string[i] = '\0';
			return( TRUE );
		}
	if ((chr != 0x0D) && (chr != 0x0A) && (chr != 0x1A)) string[i] = chr;
	else i--;
	}	
	string[i + 1] = '\0';
	return( TRUE );
}