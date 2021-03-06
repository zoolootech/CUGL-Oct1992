/*
	Program to provide serial i/o from the system on an SSM
IO4 board.
*/
#include	bdscio.h
char	inbuf[BUFSIZ];
#define	DATAB 0x23
#define STATB 0x22
#define DATAV	0x80
#define RCVERR	0x70
#define TBE	0x01	/* Transmitter buffer empty */
#define DELAY	100	/* line delay */
/*
	program to dump an ascii file to a serial port
*/
main(argc, argv)
char	*argv[];
{
int fd;
char	*p;
char	c;
 
  if(argc >1) {
	fd=fopen(argv[1],inbuf);
	if(fd !=NULL) {
		serout(0x0d);
		serout(0x0A);
		serout('S');
		serout(0x0D);
		serout(0x0A);
		while((c=getc(inbuf)) !=CPMEOF) {
			serout(c);
		}
		serout(0x0D);
		serout(0x0A);
		serout('X');
		fclose(inbuf);
	}
	else printf("Cannot open file \n");
  }
}
/*
	serout outputs to the serial port
*/
serout(ch)
char ch;
{
int	t;
	while((TBE & inp(STATB))==0); /* wait on xmt buff empty */
	if(ch < 0x20) wait();
	outp(DATAB,ch);
}
/*
	serinp inputs a character from the port
*/
serinp()
{
char	ch;
while((DATAV && (ch = inp(STATB))) == 0);	/*wait on data available */
	if((ch & RCVERR) == 0) return(inp(DATAB));
	else {
		ch=inp(DATAB);
		return('?');
	     }
}
wait()
{
int	j;
	for(j=0;j<=DELAY;j++);
}
� � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � 