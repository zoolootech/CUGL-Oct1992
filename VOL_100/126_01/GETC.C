#include "bdscio.h"

int getc(iobuf)
FILE *iobuf;
{
	int nsecs;

	/* CONSOLE */
	if (iobuf == STD_IN) return getchar();

	/* READER */
	if (iobuf == DEV_RDR) return bdos(3);

	/*  IF THERE ARE NO CHARACTERS LEFT IN THE BUFFER */
	if (!(iobuf->_nleft)) {

		/* ATTEMPT TO READ ANOTHER BUFFER FULL (NSECTS SECTORS) */
		if ((nsecs = read(iobuf->_fd, iobuf->_buff, NSECTS)) <= 0) {

			/* IF THERE IS NO MORE TO READ, RETURN EOF */
			return(EOF);
		}
		/* IF ADDITIONAL SECTORS WERE READ,
		RESET _nleft AND _nextp FOR NUMBER OF SECTORS READ */
		else {
			iobuf->_nleft = (nsecs * SECSIZ);
			iobuf->_nextp = iobuf->_buff;
		}
	}
	/* DECREMENT _nleft */
	iobuf->_nleft--;

	/* RETURN NEXT CHARACTER, OR EOF IF IT IS ^Z=CPMEOF */
	if (*iobuf->_nextp != CPMEOF) return(*iobuf->_nextp++);
	else {
		iobuf->_nextp++; /* NOT STRICTLY NECESSARY, BUT RETAINED
						FOR CONSISTENCY WITH LEOR'S GETC (WHICH
						HOWEVER RETURNS CTRL_Z UNALTERED);
						SEEKEND() USES UNGETC AFTER EOF SO
						FAILS IF THIS IS OMITTED */
		return(EOF);
	}
}
{
	int nsecs;

	/* CONSOLE */
	if (iobuf == STD_IN) return getchar();