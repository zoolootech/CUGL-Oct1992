/*@*****************************************************/
/*@                                                    */
/*@ skpblk - skip to the next non-blank value.         */
/*@        A non-blank or NULL will terminate it.      */
/*@                                                    */
/*@   Usage:     skpblk(cbuf);                         */
/*@       where cbuf is a pointer to characters.       */
/*@                                                    */
/*@   Returns a pointer to the char which terminated   */
/*@      the scan (i.e. the non-blank or NULL).        */
/*@                                                    */
/*@*****************************************************/

#define NULL 0x00


char *skpblk(cbuf)
char *cbuf;
{
		/* skip to next non-blank in buffer */
	while((*cbuf == ' ') && (*cbuf != NULL))
		cbuf++;
	return (cbuf);
}
