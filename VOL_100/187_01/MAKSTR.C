/*@*****************************************************/
/*@                                                    */
/*@ makstr - make a string of <length> characters.     */
/*@        Copies from start to end and zero           */
/*@        terminates.  Max of length.                 */
/*@                                                    */
/*@   Usage:     makstr(buf, st, end, len);            */
/*@       where buf is the output buffer.              */
/*@             st is a pointer to the start.          */
/*@             end is a pointer to the end.           */
/*@             len is the max length of buf.          */
/*@                                                    */
/*@    Returns a pointer to buf.                       */
/*@                                                    */
/*@*****************************************************/

#define EOS '\0'

/*******************************************************/

char *makstr(buffer, start, end, length)
char *buffer, *start, *end;
int length;
{
	char *sp;
	int i, j, len;

	sp = start;
	if ((len = end - start) > length - 1)
		len = length - 1;

	for (j=i=0; i <= length; j=i++)
		if (j < len)
			buffer[j] = *sp++;
		else {
			buffer[j] = EOS;
			break;
		}
	return buffer;
}
