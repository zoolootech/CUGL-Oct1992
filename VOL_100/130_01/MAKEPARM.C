


#include "bdscio.h"
#include "makedef.h"




argument( linebuf, boot, cpmfil, bios, parm )
char *linebuf, *boot, *cpmfil, *bios ;
unsigned parm[] ;
{
char file[20], *line, *gettok() ;
char token[20], n ;



line = linebuf ;

for(n=0;n<MAXVECTOR;n++)parm[n] = 0 ;
parm[ISBOOT] = FALSE ;
line = gettok( line, boot );
if(*boot == '-' || line == EOL)xusage();
newext( boot, ".HEX" );
if(lookup( boot ) == FALSE){
	strcpy( cpmfil, boot );
	newext( cpmfil, ".COM" );
	}
else {
	line = gettok( line, cpmfil );
	if(*cpmfil == '-' || line == EOL)xusage();
	newext( cpmfil, ".COM" );
	parm[ISBOOT] = TRUE ;
     }
line = gettok( line, bios );
if(*bios == '-' || line == EOL)xusage();
newext( bios, ".HEX" );
while((line = gettok( line, token )) != 0){
	while(token[0] != '-'){
		printf("Can't figure %s out, ignored.\n", token );
		if((line = gettok( line, token )) == 0)return ;
		}
	switch (token[1]) {
	  case 'A' : parm[AUTOPATCH] = TRUE ; break ;
	  case 'S' : parm[NOCHECK] = TRUE ; break ;
	  case 'T' : parm[TEST] = TRUE ; break ;
	  case 'C' : parm[DIRECTBOOT] = TRUE ; break ;
	  case 'I' :
		parm[IMMEDIATE] = TRUE ;
		gettok( line, token );
		if(token[0] <= 'P' && token[0] >= 'A'){
			parm[DRIVE] = token[0] - 'A' ;
			if((line = gettok( line, token )) == 0)return;
			}
		else {
			parm[IMMEDIATE] = FALSE ;
			printf("Ignoring immediate option due to illegal syntax\n");
			printf("Output sent to disk file\n");
		     }
		break ;
	  case 'P' :
		if((line = gettok( line, token )) == 0)return ;
		strcat( token, "H" );
		parm[PATCHADRESS] = htoi( token );
		n = 0 ;
		while(gettok(line,token) != 0){
			if(token[0] == '-')break ;
			if(n > MAXPATCH)break ;
			strcat( token, "H" );
			parm[PATCHBYTE + n] = htoi( token );
			line = gettok( line, token );
			n++ ;
			}
		parm[PATCH] = TRUE ;
		parm[PATCHBYTE + n] = -1 ;
		break ;
	  default   :
		printf("%c is an illegal option, ignored.\n",token[1] );
	  }
	}
return;
}






char *gettok( line, token )
char *line, *token ;
{
	char *p, c, n ;

	while(*line == ' ')line++ ;
	p = line ;
	if(*line == EOL)return(EOL);
	if(*line == '-'){
		p += 2 ;
		c = *p ;
		*p = 0 ;
		strcpy( token, line );
		*p = c ;
		}
	else {
		while(*p != ' ' && *p != EOL)p++ ;
		if(*p == EOL)*(p + 1) = EOL ;
		*p = EOL ;
		strcpy( token, line );
		*p = ' ' ;
	     }
	return( p );
}






look( filename, ext )
char *filename, *ext ;
{
	char v[20] ;

	strcpy( v, filename );
	newext( v, ext );
	return( lookup( v ) );
}


isext( name )
char *name ;
{
	char n ;
	
	for(n = 0 ; n < 12 && name[n] != 0 ; n++ )
		if(name[n] == '.')return(TRUE);
	return(FALSE);
}




ishex( name )
char *name ;
{
	char v[20];

	if(isext( name ) == FALSE)return(FALSE);
	strcpy( v, name );
	newext( v, ".COM" );
	if(strcmp( name, v ) == 0)return(TRUE);
	else return(FALSE);
}



iscom( name )
char *name ;
{
	char v[20];

	if(isext( name ) == FALSE)return(FALSE);
	strcpy( v, name );
	newext( v, ".COM" );
	if(strcmp( name, v ) == 0)return(TRUE);
	else return(FALSE);
}



usage(){
	printf("\n");
	printf("Usage: MAKESYS [bootfile] cpmfile biosfile [option]...\n");
	printf("       Filenames may be given without extensions, boot and bios\n");
	printf("       files taken as .HEX and cpmfile as .COM\n");
	printf("       Options:\n");
	printf("         -s           Suppress adress check on bios\n");
	printf("         -p adress byte...   Patch up to 16 bytes into the\n");
	printf("                      relative adress given (same as DDT adress)\n");
	printf("         -i drive     Do a direct sysgen on destination drive\n");
	printf("         -a           Auto patch to make user 0 public access\n");
	printf("         -c           Do an automatic cold boot after sysgen\n");
	printf("\n");
}



/*
   Lookup returns TRUE if the filename exists
   on the disk, FALSE otherwise.
*/

lookup( name )
char *name ;
{
	char fcb[45];

	if(setfcb(fcb,name) == ERROR)return(ERROR);
	bdos(26,0x9000);
	if(bdos(17,fcb) > 3)return(FALSE);
	else return(TRUE);
}




/*
    Newext puts a new extension (ext[]) onto
    filename in name[].
*/

newext( name, ext )
char *name, *ext ;
{
	char k, *p ;

	p = name ;
	for( k = 0 ; k < 8 ; k++ )
		if(*p == '.')break ;
		else p++ ;
	*p = 0 ;
	strcat( name, ext );
}




htoi( string )
char *string ;
{
	return( cbin( string ) );
}





xusage(){ usage(); exit(); }



