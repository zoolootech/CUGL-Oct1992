/*@*****************************************************/
/*@                                                    */
/*@ cant - tell user that we cannot open given file.   */
/*@        Returns to caller.                          */
/*@                                                    */
/*@   Usage:     cant(filename);                       */
/*@       where filename is an ASCIIZ string.          */
/*@                                                    */
/*@*****************************************************/

cant(s)
char *s;
{
	puts(s);
	puts(": cannot open\n");
}


