/*@*****************************************************/
/*@                                                    */
/*@ show_err - output two strings and ring the bell.   */
/*@        Any CR/LFs must be in the strings.          */
/*@                                                    */
/*@   Usage:     show_err(s1, s2);                     */
/*@       where s1 and s2 are strings to be displayed  */
/*@         on the screen.                             */
/*@                                                    */
/*@*****************************************************/


#define NULL 0x00
#define NORM 0x07

int show_err(s1, s2)
char *s1, *s2;
{
	int i;
	char strbuf[5];

	conout(s1, NORM);
	conout(s2, NORM);
	strbuf[0] = 0x07;		/* ring the bell */
	strbuf[1] = NULL;
	for (i=0; i < 20; i++)
		conout(strbuf,NORM);

}

