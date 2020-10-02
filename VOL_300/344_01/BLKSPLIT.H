
/*
 *      HEADER:         ;
 *      TITLE:          blksplit() PROTOTYPE;
 *      DATE:           10/21/1989;
 *      VERSION:        1.0;
 *      FILENAME:       BLKSPLIT.H;
 *      SEE-ALSO:       BLKSPLIT.C, PB.DOC;
 *      AUTHORS:        Michael Kelly;
 */



/*
 *  int blksplit(
 *         char *blk, char **strptr, const char *strsep, int maxstrings
 *         );
 *
 *  blk-> NULL terminated text data block
 *  strptr-> array of char *
 *  strsep-> NULL terminated array of string seperator characters
 *  maxstrings = maximum number of char * that will fit in strptr
 *
 *  splits raw text data block into strings : returns # of strings
 *
 */
int blksplit(char *blk, char **strptr, const char *strsep, int maxstrings);
