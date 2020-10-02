




/*
    This is the part of the program which con-
    tains routines for reading Intel standard
    HEX files (used by the CP/M assembler and
    many others). These routines will also re-
    cognize HEX record types which is not used
    by the CP/M assembler but will ignore them
    and print a message on the console device.
    
            (c) 1981 Jan Larsson.
*/


#include "bdscio.h"
#include "makedef.h"


rhex( name, p, max, cmp, real )
char *name, *p ;
int max ;
unsigned cmp ;
int real ;
{
	int c, total ;
	unsigned oadress ;
	char *p2, n, type, bytes, buf[BUFSIZ], first ;
	char *pp ;
	char chk ;
	unsigned adress, fd ;

	fd = fopen( name, buf );
	if(fd == ERROR)error(24);
	total = 0 ; first = TRUE ;
	skiplf( buf );
	while((c = getc( buf )) != CPMEOF && c != ERROR){
		bytes = getbyte( buf );
		chk = bytes ;
		if(first == TRUE);else oadress = adress ;
		adress = getword( buf );
		if(first == TRUE);else p = p2 + (adress - oadress);
		chk += (adress & 0x00ff);
		chk += ((adress >> 8) & 0x00ff);
		if(first == TRUE){
			if(cmp != adress && cmp != 0)
				cont( cmp, adress );
			first = FALSE ;
			}
		if(sbrk( bytes ) == ERROR)error(20);
		type = getbyte( buf );
		chk += type ;
		switch (type) {
		  case 0 :
			p2 = p ;
			for(n = 0 ; n < bytes ; n++ ){
				*p = getbyte( buf );
				chk += *p++ ;
				}
			chk += getbyte(buf);
			if(chk == 0)total += bytes ;
			else {
				printf("Checksum error at adress %4x",adress );
				printf("\nrecord ignored." );
				printf("\nChecksum = %x.\n",chk);
				p = p2 ;
			     }
			skiplf( buf );
			break ;
		  case 1 :
			fabort( fd );
			return( total );
			break ;
		  case 2 : 
			printf("INTERNAL SYMBOL RECORD found at adress ");
			printf("%4x, ignored. \n", adress );
			skiplf( buf );
			break ;
		  case 3 : 
			printf("No processing of EXTERNAL SYMBOL RECORDS ");
			printf(", found one at adress %4x \n", adress );
			skiplf( buf );
			break ;
		  case 4 :
			printf("RELOCATION INFORMATION RECORD found at ");
			printf("adress %4x, ignored.\n",adress );
			skiplf( buf );
			break ;
		  case 5 :
			printf("Ignoring a MODULE DEFINITION RECORD at ");
			printf("adress %4x.\n",adress );
			skiplf( buf );
			break ;
		  default :
			printf("I'm ignoring an unknown HEX record type ");
			printf("at adress %4x.\n", adress );
			skiplf( buf );
			break ;
		  }
		}
	if(bytes > 0){
	printf("%s was a strange HEX file, I couldn't find the ", name );
	printf("EOF record.\n");
	printf("I have read %d number of bytes successfully (I hope) and\n",total);
	printf("the latest adress was %4x.\n",adress );
		}
	fabort( fd );
	return(total);
}





/*
    The following function returns a word from
    four HEX characters in a file.
*/

unsigned getword( buf )
char *buf ;
{
	return((getbyte( buf ) << 8) | getbyte( buf ));
}



/*
    Here's the function which returns a byte 
    from two HEX characters.
*/

getbyte( buf )
char *buf ;
{
	return((gethex( buf ) << 4) | gethex( buf ));
}




/*
    Finally the real workhorse which reads a
    HEX character from the file and returns
    it's binary value. If we find an EOF in
    this routine we might as well quit as there
    must be some serious error.
*/

gethex( buf )
char *buf ;
{
	int c ;

	c = tolower( getc( buf ) );
	if(c == CPMEOF || c == ERROR)error(21);
	if(c >= '0' && c <= '9')
			return(c - '0');
	else if(c >= 'a' && c <= 'f')
			return((c - 'a') + 10);
	else error(22);
}


/*
    A small function whose only purpose is to
    skip to EOL.
*/

skiplf( buf )
char *buf ;
{
	int c ;

	c = getc( buf );
	while(c == 0x0d || c == '\n' || c == ' ')c = getc( buf );
	ungetc( c, buf );
}




/*
    To this fonction we come when we are trying
    to read in a bios with a start adress which 
    doesn't match the CP/M we are patching it
    together with.
*/

cont( cmp, adress )
unsigned cmp, adress ;
{
	printf("What are you trying to do ??? Your CP/M wants the BIOS at \n");
	printf("adress %x and you want me to patch in a BIOS which starts\n",cmp);
	printf("at %x, want to continue (y/n) ? ", adress );;
	if(tolower(getchar()) == 'y')return ;
	else exit();
}





rcpm( cpmfile, base )
char *cpmfile, *base ;
{
	char n ;
	unsigned fdcpm ;

	if((fdcpm=open(cpmfile,0)) == ERROR){
		printf("Sorry, cannot open %s as cpmfile.\n",cpmfile);
		exit();
		}
	for(n = 0 ; n < 16 ; n++ )
	if(read( fdcpm, base, 1 ) == ERROR){
		printf("Uuuuhhh !!!  Error when reading %s, I give up.\n",cpmfile);
		exit();
		}
	for(n = 0 ; n < 51 ; n++ )
	if(read( fdcpm, base, 1) == ERROR){
		printf("Error occured while reading %s so I'm quitting.\n",cpmfile);
		exit();
		}
	else base += 128 ;
	fabort( fdcpm );
}




wcpm( cpmfile, base, size )
char *cpmfile, *base ;
int size ;
{
	unsigned fdcpm ;
	char n ;

	if((fdcpm=open(cpmfile,1)) == ERROR){
		printf("Can't open %s for output, unfortunately.\n",cpmfile);
		exit();
		}
	for( n = 0 ; n < 16 ; n++ )
	if(write( fdcpm, base, 1 ) == ERROR){
		printf("Disk write error when writing %s.\n",cpmfile);
		exit();
		}
	for( n = 0 ; n < size ; n++ )
	if(write( fdcpm, base, 1 ) == ERROR){
		printf("Write error on %s, maybe lack of disk space ?\n",cpmfile);
		exit();
		}
	else base += 128 ;
	if(close( fdcpm ) == ERROR){
		printf("Oh nooo...  can't close %s. \n",cpmfile);
		exit();
		}
}




sysgen( drive, base )
int drive ;
char *base ;
{
	int cdrive, sector, track ;

	cdrive = bdos( 25 );
	bdos( 14, drive );

	for( track = 0 ; track < 2 ; track++ ){
		for( sector = 1 ; sector <= 26 ; sector++ ){
			bios( 10, track );
			bios( 11, skew(sector) );
			bios(12,base + (((track*26) + (skew(sector) - 1)) * 128));
			bios(14);
			}
		}
	bdos( 14, cdrive );
}



skew( sector )
int sector ;
{
	char skewtab[27];

	initb(skewtab,"1,7,13,19,25,5,11,17,23,3,9,15,21,2,8,14,20,26,6,12,18,24,4,10,16,22");

	sector-- ;
	return( skewtab[ sector ] );
}





error( type )
char type ;
{
	printf("Error of type %u occured.\n",type);
}


