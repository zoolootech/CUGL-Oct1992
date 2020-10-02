/*@*****************************************************/
/*@                                                    */
/*@ get_uniq - returns a unique file name built from   */
/*@        the date and time.  The file name is mmdd   */
/*@        from the date and hhmm from the time.       */
/*@                                                    */
/*@   Usage:     get_uniq(buffer);                     */
/*@       where buffer is a 9-byte or more area.       */
/*@   NOTE: The extension must be added by the caller. */
/*@      (I.E. strncat(get_uniq(buf), ".LOG");         */
/*@                                                    */
/*@   Returns a pointer to the buffer.                 */
/*@                                                    */
/*@*****************************************************/


char *get_uniq(buffer)
char *buffer;
{
	char date[9], time[9];
	int i;

	dates(date);
	times(time);

	buffer[0] = date[0];		/* mm */
	buffer[1] = date[1];
	buffer[2] = date[3];		/* dd */
	buffer[3] = date[4];
	buffer[4] = time[0];		/* hh */
	buffer[5] = time[1];
	buffer[6] = time[3];		/* mm */
	buffer[7] = time[4];

	buffer[8] = '\0';

	for (i=0; buffer[i]; i++)		/* prevent imbedded spaces */
		if (buffer[i] == ' ')
			buffer[i] = '0';

	return buffer;

}
