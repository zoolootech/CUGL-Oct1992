/* MCRDRV.H

   This is the drivers for decoding and CARD (MCR) reading techniques
   The program reads three lines from an input port.

      The lines are : CARD SENSE         (b0)
                      CLOCK              (b1)
                      DATA               (b2)

   These procedures are provided for use by the calling program:

      initmcr();
      sensecard();
      readcard();
      decodecard();

   The following global declarations are required for this to work:

*/
#define MaxBitSize      401
#define IOBaseAddress   0x9000
#define MCRPortInit     0x227
#define CardSenseBit    1
#define MCR             0x226
#define ClockBit        2
#define DataBit         4
#define CardSTX         11
#define CardETX         15
#define forwards        1
#define backwards       -1

#data
int     MaxIndex;
char    buf[MaxBitSize];

#code

initmcr() /* This procedure initializes the PPI on Hugh's digital board */
{
        char *port;
        port = IOBaseAddress;
        port[ MCRPortInit ] = 0x83; /* Initialize 8255 PPI */
}

sensecard() /* This procedure senses if a card is in the MCR, 1 = card */
{
        char    *portMCR;
        portMCR = IOBaseAddress+MCR;
        return( ((*portMCR) & CardSenseBit ) == 0 );
}

readcard()
{
 /* This procedure reads a card connected to the 8255 PPI, placing each
   data pulse bit in the byte array 'buf'.

   The procedure sets the global variable 'MAXINDEX' to the end of the
   data in the array 'buf'.
 */
        char *portMCR;
        MaxIndex = 0;
        portMCR = IOBaseAddress+MCR;

        do
        {
                while( (*portMCR & ClockBit) == 0 )
                        ;
                while( ((*portMCR & ClockBit) != 0) && ((*portMCR & CardSenseBit) ==0) )
                        ; /* Wait for edge */
                buf[ MaxIndex ] = *portMCR & DataBit; /* Get Card */
                if( MaxIndex < MaxBitSize )  MaxIndex++;
        } while( (*portMCR & CardSenseBit) == 0);
}

Pack( index, dirn, result )
int index, dirn;
char *result;
{
/*
   This function packs the 5 bit code (4 bits+parity) in buf[ index ]
   into the result byte.

   The function returns TRUE if the parity is correct.

   The pack may be done in either direction (forwards or backwards), with
   dirn having the value 1 or -1
*/
        char mask, parity;
        mask = 1;
        parity = 0;
        *result = 0;
        do {
                if( buf[index] == 0) {
                        *result = *result | mask;
                        if( parity == 0 ) parity = 1; else parity = 0;
                }
                index = index + dirn;
                mask  = mask + mask;
        } while ( mask != 0x20 );
        *result = *result & 15;
        return parity;
}

CheckPattern( index,dirn,result,rslt )
int index, dirn;
char result[], *rslt;
{
        char lst, Cont, loc;
  /*
   This function checks the pattern in the array buf, starting at element
   'index'.

   If the pattern starts with a card 'STX' and ends with a card 'ETX', the
   function returns TRUE. The decoded ASCII is placed in the string 'result'.

   The returned BYTE 'rslt' contains the last code found.
  */
        loc = 0;
        if( Pack( index,dirn,rslt ) )
        {
                if( *rslt == CardSTX )
                {
                        index = index + ( dirn*5 );
                        lst   = 0;
                        Cont  = 1;
                        while( Pack( index,dirn,rslt ) && Cont )
                        {
                                if( *rslt == CardETX )
                                {
                                        *rslt = 0;
                                        result[ lst ] = 0;
                                        loc = 1;
                                        Cont = 0;
                                }
                                else if( ( index<=0 ) || ( index>=MaxBitSize ) )
                                {
                                        rslt = 0;
                                        result[lst] = 0;
                                        loc = 1;
                                        Cont = 0;
                                }
                                else
                                {
                                        result[lst] = *rslt + 0x30;
                                        lst++;
                                        index = index+( dirn*5 );
                                }
                        }
                }
                else if( *rslt == CardETX ) loc = 0;
                else
                {
                        *rslt = 0;
                        loc = 0;
                }
        }
        return loc;
}

Decode( dirn, result )
int dirn;
char result[];
{
        char lastchar, loc, Cont;
        int index;
/*
   This function returns TRUE if the buf array can be coded correctly. The
   result is returned in the string 'result'.

   This function only makes a single pass through the buf bit array in one
   direction.
*/
        loc = 0;
        if( dirn == forwards ) index = 0;
        else index = MaxIndex;
        Cont = 1;
        while( Cont && ( index >= 0 ) && ( index <= MaxIndex ) )
        {
                if( CheckPattern( index,dirn,result,&lastchar ) )
                {
                        Cont = 0;
                        loc = 1;
                }
                else if( lastchar == CardETX )
                {
                        Cont = 0;
                        loc  = 0;
                }
                else index = index+dirn;
        }
        return loc;
}


decodecard( result )
char result[];
{
/*  This function attempts to decode the buf bit array in both the forward and
   backward directions.
*/
        if( Decode( forwards,result ) ) return 1;
        else if( Decode( backwards,result ) ) return 1;
        else return 0;
}

