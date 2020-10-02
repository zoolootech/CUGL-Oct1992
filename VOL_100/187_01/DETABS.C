/*@*****************************************************/
/*@                                                    */
/*@ detabs - remove tabs from input string.            */
/*@        Assumes a fixed tab interval.               */
/*@                                                    */
/*@   Usage:   detabs(out, in, interval, omax);        */
/*@       where out is the output string area.         */
/*@             in  is the input string area.          */
/*@             interval is the fixed tab interval.    */
/*@             omax is the max output area size.      */
/*@                                                    */
/*@*****************************************************/


detabs(bufo,bufi,tabsize,omax)	/* remove tabs from input string */
char bufi[], bufo[];
int tabsize, omax;
{
	char c;
	int ii, io;

	ii = io = 0;
	while ((c = bufi[ii++]) && io < omax)		/* till end of string */
		if (c == '\t') {
			bufo[io++] = ' ';
			while ((io % tabsize) && io < omax)	/* space to next tab */
				bufo[io++] = ' ';
		}
		else
			bufo[io++] = c;

	bufo[io] = '\0';


}

