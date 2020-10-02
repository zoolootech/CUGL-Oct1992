/*@*****************************************************/
/*@                                                    */
/*@ getctl - transforms a string containing the        */
/*@        name of a control- character to it's        */
/*@        internal binary value.  E.G. NUL to 0x00.   */
/*@                                                    */
/*@   Usage:     getctl(string);                       */
/*@       where string contains a standard name for a  */
/*@          a control character.                      */
/*@    Returns the binary value or -1 if no match.     */
/*@                                                    */
/*@*****************************************************/

#define ERR  -1

char getctl(str)
char *str;
{
	static char *cntlchr[] = {"NUL","SOH","STX","ETX","EOT","ENQ","ACK","BEL",
		"BS ","HT ","LF ","VT ","FF ","CR ","SO ","SI ","DLE",
		"DC1","DC2","DC3","DC4","NAK","SYN","ETB","CAN","EM ",
		"SUB","ESC","FS ","GS ","RS ","US "};
	int i;

	for (i=0; i<(sizeof(cntlchr)/sizeof(cntlchr[1])); i++)
		if (strcmp(cntlchr[i],str) == 0)
			return i;
	return ERR;
}
