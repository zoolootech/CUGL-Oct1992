
#include <ctype.h>

soundex(char *out_pntr, char *in_pntr)
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Purpose: Calculate the soundx code of a string.                     ³
³                                                                    ³
³ Inputs: char *out_pntr = pointer to a 5 char array to put the      ³
³                          soundx code into.                         ³
³         char *in_pntr  = pointer to string to calc. soundx code of.³
³                                                                    ³
³Outputs: Soundx code stored into area pointed to by *out_pntr.      ³
³                                                                    ³
³ Return: None                                                       ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
extern char get_scode();
char ch,last_ch;
int count = 0;

        strcpy(out_pntr,"0000");        /* Pre-fill output string for    */
                                        /* error and trailing zeros.     */
        *out_pntr = toupper(*in_pntr);         /* Copy first letter             */
        last_ch = get_scode(*in_pntr);        /* code of the first letter      */
                                        /* for the first 'double-letter  */
                                        /* check.                         */
                                        /* Loop on input letters until   */
                                        /* end of input (null) or output */
                                        /* letter code count = 3         */

        while( (ch = get_scode(*(++in_pntr)) ) && (count < 3) )
        {
        if( (ch != '0') && (ch != last_ch) ) /* if not skipped or double */
                *(out_pntr+(++count)) = ch; /* letter, copy to output */
                last_ch = ch;        /* save code of last input letter for */
                                /* next double-letter check */
        }
        return(0);
}

char get_scode(ch)
char ch;
{
                            /* ABCDEFGHIJKLMNOPQRSTUVWXYZ */
                            /* :::::::::::::::::::::::::: */
static char soundex_map[] =   "01230120022455012623010202";

        /* If alpha, map input letter to soundex code. If not, return 0 */

        if( !isalpha(ch) )        /*error if not alpha */
                return(0);
        else
                return(soundex_map[(toupper(ch) - 'A')] );
}
