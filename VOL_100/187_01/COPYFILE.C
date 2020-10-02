/*@*****************************************************/
/*@                                                    */
/*@ copyfile -  file named <ifn> to file named <ofn>.  */
/*@     if a file named <ofn> exists, it is deleted    */ 
/*@     prior to copy.                                 */
/*@                                                    */
/*@   Usage:     copyfile(infilename, outfilename);    */
/*@       where infilename is ASCIIZ input filename.   */
/*@            outfilename is ASCIIZ output filename.  */
/*@                                                    */
/*@       returns zero if successful, 1 or 2 otherwise.*/
/*@       1 => input open failed.                      */
/*@       2 => output open failed.                     */
/*@       NOTE: opens and closes files.                */
/*@                                                    */
/*@*****************************************************/


char cpy_buf[1024];

int copyfile(ifn, ofn)
char *ifn, *ofn;
{
	int ifp, ofp, ret, retv;

	retv = 0;

	if (0 == (ifp = fopen(ifn, "r")))
		retv |= 1;			/* cannot open input file */

	if (!retv)
		if (0 == (ofp = fopen(ofn, "w")))
			retv |= 2;			/* cannot open output file */


	if (!retv) {
		ret = 1;
		while (ret > 0) 
			if ((ret = read(ifp, cpy_buf, 1024)) > 0)
				ret = write(ofp, cpy_buf, ret);
	}

	if (!(retv & 1))		/* if ifp open */
		fclose(ifp);
	if (!(retv & 2))		/* if ofp open */
		fclose(ofp);

	return retv;

}
