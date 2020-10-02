/*@*****************************************************/
/*@                                                    */
/*@ kbecho - collects characters from a keyboard input */
/*@        routine, expands control chars to reverse   */
/*@        video names, and writes them when a line    */
/*@        is full.                                    */
/*@                                                    */
/*@   Usage:     kbecho(char);                         */
/*@       where char is the char to be echoed.         */
/*@                                                    */
/*@*****************************************************/


/***********************************************************************/
/*								       */
/*	The following are specific for the IBM PC.			   */
/*								       */
/***********************************************************************/

#define 	NORM	0x07	/* normal video attribute */

/***********************************************************************/

#define 	EOS	0x00	/* end of string */

char strbuf[20];
int  charcnt = 0;

kbecho(KbChar)
char KbChar;
{
      int showch();

      charcnt += showch(KbChar);
      if (charcnt >= 75) {
	  charcnt = 0;
	  strbuf[0] = '\n';
	  strbuf[1] = EOS;
	  conout(strbuf,NORM);
       }
}
