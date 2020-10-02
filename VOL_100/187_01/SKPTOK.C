/*@*****************************************************/
/*@                                                    */
/*@ skptok - skip over a token (to next delimiter).    */
/*@        A blank, ';', or NULL will terminate it.    */
/*@                                                    */
/*@   Usage:     skptok(cbuf);                         */
/*@       where cbuf is a pointer to characters.       */
/*@                                                    */
/*@   Returns a pointer to the char which terminated   */
/*@      the scan (i.e. the blank, ';' or NULL).       */
/*@                                                    */
/*@*****************************************************/

#define NULL 0x00

char *skptok(cbuf)
char *cbuf;
{
		/* skip token in buffer */
	for(;(*cbuf != ' ') && (*cbuf != ';') && (*cbuf != NULL);cbuf++)
		;
	return (cbuf);
}
