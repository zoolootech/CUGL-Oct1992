/*
HEADER:         CUGXXX;
TITLE:          Decimal string to packed decimal;
DATE:           12-28-83;
DESCRIPTION:    Converts integer (string) decimal number to packed decimal;
KEYWORDS:       Packed decimals;
FILENAME:       PACDEC.C;
WARNINGS:       Decimal number must already be a string;
AUTHORS:        Odis C. Wooten;
COMPILER:       DeSmet C;
REFERENCES:     US-DISK 1308;
ENDREF
*/

/*     driver for pacdec    */

main()
{
    char toptr[11]  , fromptr[10] ;
    int  ndigits , i,j;
for (j=0;  j < 2000;  j +=10 )
    {
	for ( i = 0;   i < 11;	 i++ )
	 toptr[i] = '\0' ;

	ndigits = itoa(j   , fromptr) ;
	printf ( " j = %d  ndigits = %d   %s :", j,ndigits,fromptr);
	toptr[9] = 0x0f ;
	pacdec(fromptr, toptr, ndigits, 10, 1 ) ;
	for ( i=0;  i<11;  i++ )
	      printf ( "%02x ",toptr[i] );
	printf("\n\n\n");
    }
}


#define  RIGHT 0
#define  LEFT  1
pacdec(fromptr, toptr, ndigits, topos, nibble)
char *fromptr, *toptr ;
int   ndigits, topos, nibble ;
{
fromptr += (ndigits-1);
toptr	+= (topos-1)  ;

while ( ndigits>0 )
  {
  printf("\nndigid=%d ",ndigits);
     if ( nibble == RIGHT )
	{      /*  pack right nibble */
	  *toptr = (*toptr & 0xf0)  |  (*fromptr-- & 0x0f)  ;
	  nibble = LEFT ;
	}
     else
	{	/*  pack left nibble */
	  *toptr = (*toptr & 0x0f)  |  ( (*fromptr-- & 0x0f) << 4 ) ;
	  nibble = RIGHT ;
	  toptr--;
	}
     ndigits--;
  }
}
