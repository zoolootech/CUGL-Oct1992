/*
 * xmodem.c - xmodem functions for Xenix or Unix
 * copyright 1986, 1988 Ronald Florence
 * 
 * modified 3/88 to do crc by packet instead of by byte
 */

#include <signal.h>
#include <stdio.h>

#ifdef CU

extern int	rlfd;				/* the open line in cu */
#define	WFD	rlfd
#define RFD	rlfd
#define	errf	stderr

#else

extern	FILE	*errf;				/* error file for remote */
#define	WFD	1				/* stdout */
#define RFD	0				/* stdin */

#endif

#define	BSIZE		128
#define	DEBUG		01
#define LF		02
#define CRC		04
#define NOREAD(x, c)	(rchar(x, &c) == -1)
#define TX(c)		write(WFD, &c, 1)	
#define ever		(;;)

static	char	soh = 0x01,
		eot = 0x04,
		ack = 0x06,
		nak = 0x15,
		can = 0x18,
		crcinit = 'C';

static	int	debug,
		crc;

unsigned short	do_crc();

int	kleenex(),
	onalarm(); 

xget(fp, opts)
FILE	*fp;
int	opts;
{
	char 	buf[BSIZE]; 
	unsigned char	inch, b= 1, crchi;
	int	iput = BSIZE, i; 

	debug = (opts & DEBUG);
	crc = (opts & CRC);
	signal(SIGALRM, onalarm);
#ifdef CU
	signal(SIGINT, kleenex);
#else
	sleep(10);
#endif
	(crc) ? TX(crcinit) : TX(nak);
	for ever {
		if NOREAD(10, inch) {
			err("Timeout during SOH");
			cksend(crc ? crcinit : nak);
			continue;
		}
		if (inch == eot) 
			break;
		if (inch == can) {
			err("CAN block %u", b);
			kleenex(-1);
		}
		if (inch != soh) {
			err("Bad SOH block %u: %#x", b, inch);
			cksend(nak);
			continue;
		}
		if NOREAD(2, inch) {
			err("Timeout block %u during blocknum", b);
			cksend(nak);
			continue;
		}
		if (inch != b) {
			err("Expected blocknum %u, got %u", b, inch);
			cksend(nak);
			continue;
		}
		if NOREAD(2, inch) {
			err("Timeout block %u during ~blocknum", b);
			cksend(nak);
			continue;
		}
		if (inch != ~b) {
			err("Expected ~blocknum %u, got %u", ~b, inch);
			cksend(nak);
			continue;
		}
			/* Read in the block of 128 bytes without
			 * taking time for checksums or crc.
			 */
		for (i = 0; i < BSIZE; i++)
			if NOREAD(2, buf[i]) 
				break;
		if (i < BSIZE) {
			err("Timeout data recv, char #%d", i);
			cksend(nak);
			continue;
		}
                if (crc && NOREAD(2, crchi)) {
			err("Timeout crc hibyte");
			cksend(nak);
			continue;
		}
                if NOREAD(2, inch) {
			err("Timeout %s", (crc) ? "crc lobyte" : "checksum");
			cksend(nak);
			continue;
		}
			/* Now, when we have the whole packet,
			 * do the checksum or crc.
			 */
		if (crc) {
			unsigned short	crcsum;

			crcsum = do_crc(buf);
			if (inch + (crchi << 8) != crcsum) {
				err("Expected crc %u, got %u", 
					crcsum, inch + (crchi << 8));
				cksend(nak);
				continue;
			}
		}
		else {
			unsigned char	cksum;

			for (cksum = 0, i = 0; i < BSIZE; i++) 
				cksum += buf[i];
			cksum %= 256;
			if (cksum != inch) {
				err("Expected checksum %u, got %u", cksum,inch);
				cksend(nak);
				continue;
			}
		}
		TX(ack);
#ifdef CU
		putc('.', stderr);
#endif
		if (opts & LF) 
			for (i=0, iput=0; i < BSIZE; i++) {
				if (buf[i] == 0x1a)	/* old ms-dos eof */
					break;
				if (buf[i] != '\r')
					buf[iput++] = buf[i];
			}
		fwrite(buf, iput, 1, fp);
		b++;
		b %= 256;
	}
	TX(ack);
	kleenex(0);
}


xput(fp, opts)
FILE	*fp;
int	opts;
{
	char	buf[BSIZE];
	unsigned char	b = 1, cb, inch;
	int 	cread, i;


#ifdef CU
	signal(SIGINT, kleenex);
#endif
	signal(SIGALRM, onalarm);
	debug = (opts & DEBUG);
	rchar(60, &cb);
	if (cb == crcinit)
		crc = 1;
	else if (cb == nak)
		crc = 0;
	else  {
		err("No startup %s", (crc) ? "'C'" : "NAK");
		kleenex(-1);
	}
	cread = fillbuf(fp, buf, (opts & LF));
	while (cread) {
		for (i = cread; i < BSIZE; i++) 
			buf[i] = 0;
		TX(soh);
		TX(b);
		cb = ~b;
		TX(cb);
		write(WFD, buf, BSIZE);
		if (crc) {
			unsigned short	crcsum;
			unsigned char	crclo, crchi;

			crcsum = do_crc(buf);
			crclo = (crcsum & 0xff);
			crchi = (crcsum >> 8);
			TX(crchi);
			TX(crclo);
		}
		else {
			unsigned char	cksum;

			for (cksum = 0, i = 0; i < BSIZE; i++) 
				cksum += buf[i];
			cksum %= 256;
			TX(cksum);
		}
		if NOREAD(15, inch) {
			err("Timeout after block %u", b);
			continue;
		}
		if (inch == can) {
			err("CAN after block %u", b);
			kleenex(-1);
		}
		if (inch != ack) {
			err("Non-ACK after block %u: %#x", b, inch);
			continue;
		}
#ifdef CU
		putc('.', stderr);
#else
		if (debug) 
			fprintf(errf, "Validated block %u\n", b);
#endif
		cread = fillbuf(fp, buf, (opts & LF));
		b++;
		b %= 256;
	}
	for ever {
		TX(eot);
		if NOREAD(15, inch) {
			err("Timeout during EOT");
			continue;
		}
		if (inch == can) {
			err("CAN during EOT");
			kleenex(-1);
		}
		if (inch != ack) {
			err("Non-ACK during EOT: %#x", inch);
			continue;
		}
		break;
	}
	kleenex(0);
}


fillbuf(fp, buf, lf)
FILE	*fp;
char	*buf;
int	lf;
{
	int	i = 0, c; 
	static	int	cr_held; 

	if (cr_held) {
		buf[i] = '\n';
		i++;
		cr_held--;
	}
	for (; i < BSIZE; i++) {
		if ((c = getc(fp)) == EOF)
			break;
		if (c == '\n' && lf) {
			buf[i] = '\r';
			if (i == 127) {
				cr_held++;
				return BSIZE;
			}
			buf[i+1] = '\n';
			i++;
		}
		else
			buf[i] = c;
	}
	return i;
}


unsigned short do_crc(b)
char	*b;
{
	unsigned short	shift, flag, crcsum;
	char	c;
	int	i;

	for (i = 0, crcsum = 0; i < (BSIZE + 2); i++) {
		c = (i < BSIZE) ? b[i] : 0;
		for (shift = 0x80; shift; shift >>= 1)  {
			flag = (crcsum & 0x8000);
			crcsum <<= 1;
			crcsum |= ((shift & c) ? 1 : 0);
			if (flag)
				crcsum ^= 0x1021;
		}
	}
	return (crcsum);
}


/* The timeout in rchar() works by deliberately 
 * interrupting the read() system call.  We know 
 * errno=EINTR, so there is no reason for a 
 * perror() autopsy. 
 */
rchar(timeout, cp)
unsigned	timeout;
char	*cp;
{
	int	c; 

	alarm(timeout);
	if ((c = read(RFD, cp, 1)) == -1)
		return -1;
	alarm(0);
	return c;
}


onalarm()
{
	signal(SIGALRM, onalarm);
}


kleenex(sig)
int sig;
{
#ifdef CU
	if (sig > 0)
		cksend(can);
	else 
		fprintf(stderr, "\r\nFile transfer %s.",
			(sig) ? "cancelled" : "complete");
	fprintf(stderr, "\r\n");
#else
	printf("File transfer %s.\r\n", (sig) ? "cancelled" : "complete");
	resetline();
#endif
	exit(sig);
}


cksend(ch)
char	ch;
{
	int	j;
	char	cp;

	do {
		j = rchar(2, &cp);
	} while (j != -1);
	TX(ch);
}


/* VARARGS1 */
err(s, i, j)
char	*s;
int	i, j;
{
	if (debug) {
		fprintf(errf, s, i, j);
#ifndef CU
		fprintf(errf, "\n");
	}
#else
		fprintf(errf, "\r\n");
	}
	else
		putc('%', stderr);
#endif
}
