/*
HEADER:		;
TITLE:		ISIS directory lister;
VERSION:	1.0;

DESCRIPTION:	"Displays the directories of single density ISIS2 diskettes.";

KEYWORDS:	Disk, diskette, directory, utility, ISIS;
SYSTEM:		CP/M-80;
FILENAME:	IDIR.C;
WARNINGS:	"Contains a syntax error which prevents compiling.";
SEE-ALSO:	IDIR.DOC;
AUTHORS:	Jan Larsson;
COMPILERS:	BDS C;
*/
/*****************************************************************

Version 1.0:
	Jan Larsson, Kosterv. 12, S-181 35 Lidingo, SWEDEN

*****************************************************************/
     
#define SETTRK 10
#define SETSEC 11
#define SETDMA 12
#define READ 13


#include <bdscio.h>

char row ;					

struct ientry {
	char ok ;
	char iname[6] ;
	char iext[3] ;
	char attr ;
	char oflw ;
	char blocks ;
	char dummy[3] ; 
	};

struct ientry isektor[8] ;

dsektor()
{
	char index, astr[7], sattr ;
	char xoflw ;
	char gg, gc ;

	for(index = 0 ; index < 8 ; index++){
	    sattr = isektor[index].attr ;
	    isektor[index].attr = 0 ;
	    xoflw = isektor[index].blocks / 64 ;
	    if(isektor[index].ok == 0){
	        if(row == 0){ row = 1 ; printf("        ");}
		else { row = 0 ; putchar('\n');}
		for(gg = 0 ; gg < 6 ; gg++){
		    gc = isektor[index].iname[gg] ;
		    if(gc == 0)putchar(' ');
		    else putchar( gc );
		    }
		if(isektor[index].iname[gg] == 0)putchar(' ');
		else putchar('.');
		for(gg = 0 ; gg < 3 ; gg++){
		    gc = isektor[index].iext[gg] ;
		    if(gc == 0)putchar(' ');
		    else putchar( gc );
		    }
		printf("     %3d    %5d   ",
		       isektor[index].blocks + 1 + xoflw,
		       (isektor[index].blocks - 1) * 128 + isektor[index].oflw );
		if(sattr & 0x04)strcpy( astr, "W" );
		else strcpy( astr, " " );
		if(sattr & 0x02)strcat( astr, "S" );
		else strcat( astr, " " );
		if(sattr & 0x01)strcat( astr, "I" );
		else strcat( astr, " " );
		if(sattr & 0x80)strcat( astr, "F" );
		else strcat( astr, " " );
	
		printf("%s", astr );
	        }
	    }
}


main( argc, argv )
char **argv ;
int argc ;
{
	char extended, sec ;
	char ndriv, odriv ;

	extended = FALSE ;
	row = 1 ;
	ndriv = 255 ;
	printf("\n + + +  Maffians Isis II Tittare  + + +\n");
	printf(  "            by Janne Larsson\n");
		if(argc > 1 && *(argv[1]) == '-'){
			ndriv = *(argv[1]+1) ;
			odriv = bdos( 25 );
			ndriv -= 'A' ;
			bdos( 14, ndriv );
			}
		else {
			printf("\n\nPutta in Isis II disken nu, Rune :");
			bios(3);putchar('\n');
		     }
	putchar('\n');
	printf("Directory of Isis II Diskette:\n");
	printf("Name  .Ext    Blks   Length   Attr");
	printf("        ");
	printf("Name  .Ext    Blks   Length   Attr");
	bios( SETDMA, isektor );
	bios( SETTRK, 1 );
	for(sec = 2 ; sec < 27 ; sec++){
	    bios( SETSEC, sec );
	    bios( READ );
	    dsektor();
	    }
	printf("\n\nEnd of Isis Directory\n");
	if(ndriv == 255){
	    printf("Skyffla in CP/M disketten igen nu, Rune :");
	    bios(3);
	    putchar('\n');
	    }
	else bdos( 14, odriv );
        }
