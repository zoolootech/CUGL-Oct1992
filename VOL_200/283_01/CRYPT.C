
/* crypt.c
** December 30, 1986
*/

/*=======================================================================*/
/*  An Elementary Holocryptic Cipher:                                    */
/*  The following is a variation on what was described as Hogg's code    */
/*  from World War I in the Sept. 1984 FOGHORN (First Osborne Group).    */
/*  The point of the additional bells & whistles is to make the entire   */
/*  text of the Zaphod Adventure as nearly holocryptic as possible.      */
/*  It works by spinning the key modulus on successive lines; so, while  */
/*  line appearance is preserved, analysis of the characters of each     */
/*  line depends on recovering the initial LCG seed, as well as the key. */ 
/*  Bright adventurers are discouraged from trying to cheat by dumping   */
/*  the Zaphod files, hopefully.  -D. C. Oshel, 10/6/84                  */
/*=======================================================================*/

/*
        USAGE:   set_crypt(keystring, seedinteger);  < string ciphers >
                 encode( target1string );
                 encode( target2string );
                 ... etc.
        
                 set_crypt(keystring, seedinteger);
                 decode( target1string );
                 decode( target2string );
                 ... etc.

        OR:      set_crypt(key, seed);   < single-character ciphers >
                 c0 = cryptic(c0); 
                 c1 = cryptic(c1); 
                 ... ; 
                 cN = cryptic(cN); 

                 set_crypt(key, seed);
                 c0 = uncrypt(c0); 
                 c1 = uncrypt(c1); 
                 ... ; 
                 cN = uncrypt(cN); 
*/

#include <stdio.h>
#include <string.h>

static unsigned int kRNDlxo;
static int kloc, klen;
static char kQey[32];

static void Krandi() 
{
        kRNDlxo *= 2053;      /* a long-period LCG */
        kRNDlxo += 13849;
        kloc = abs(kRNDlxo);
        kloc %= klen;
}



void set_crypt( key, seed ) 
char *key; 
int seed; 
{
        strncpy( kQey, key, 31 );
        kRNDlxo = seed; 
        klen = strlen( kQey ); 
        Krandi();
}



int cryptic( c ) 
int c; 
{
        static int k;
        k = c;
        if ( k > 31 && k < 127 ) 
        {
            k = ( k - 32 ) + ( kQey[ kloc ] - 32 );
            k %= 95;
            ++kloc;
            kloc %= klen;
            k += 32;
        }
        return ( k );
}



int uncrypt( c ) 
int c; 
{
        static int k;
        k = c;
        if ( k > 31 && k < 127 ) { 
                k -= 32; 
                k -= ( kQey[ kloc ] - 32 ); 
                ++kloc;
                kloc %= klen;
                if ( k < 0 ) k += 95;
                k += 32;
        }
        return ( k ); 
}



char *encode(p) 
char *p; 
{
        static char *q;
        q = p;

        while (*q) { *q = cryptic(*q); q++; }
        Krandi();
        return (p);
}


char *decode(p) 
char *p; 
{
        static char *q;
        q = p;

        while (*q) { *q = uncrypt(*q); q++; }
        Krandi();
        return (p);
}



