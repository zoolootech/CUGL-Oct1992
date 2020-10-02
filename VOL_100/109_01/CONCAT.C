
/* Concat will concatenate two files (file1.ext and file2.ext)
into a third file (file3.ext). It's used by typing the 
following command line:
concat file1.ext file2.ext file3.ext
*/


#define NB 80       /*  80*128 = 10K BUFSIZ     */
#define CPMEOF -1
#define EOF 26
int ifd1,ifd2,ofd,c;
char ibuf[NB*128+8], obuf[NB*128+8];
main (argc,argv)
char **argv;
{
	ifd1 = FOpenBig(argv[1], ibuf, NB);
	ofd = FCreatBig(argv[3], obuf, NB);
	transfer ();
	close(ifd1);
	ifd2 = FOpenBig(argv[2], ibuf);
	transfer ();
	close(ifd2);
	PutcBig(EOF, obuf);
	FflushBig (obuf);
	close(ofd);
}
transfer ()
{
	while ((c = GetcBig(ibuf)) != EOF && c != CPMEOF)
		PutcBig(c, obuf);
}
