#include "bdscio.h"

int rawgetc(iobuf)
struct _buf *iobuf;
{
	int nsecs;

	/* CONSOLE */
	if (iobuf == 0) return getchar();

	/* READER */
	if (iobuf == 3) return bdos(3);

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
	/* DECREMENT _nleft AND
	RETURN NEXT CHARACTER EVEN IF IT IS CPMEOF */
	iobuf->_nleft--;
	return(*iobuf->_nextp++);
}
les differ at %u:\n\t%s = %d = ",cc,argv[1],c1);
			showchar(c1);
			printf("\n\t%s = %d = ",argv[2],c2);
			sh