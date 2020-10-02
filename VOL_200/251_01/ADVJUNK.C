#include <stdio.h>

long _seed = 1L;

int rand()
{
    _seed *= 397204094L;
    return (_seed & 0x7FFF);
}

srand(n)
  long n;
{
   _seed = n;
}

int getch()
{
    int ch;
    if ((ch = bdos(1) & 0xFF) == '\r') { bdos(6,'\n'); ch = '\n'; }
    return (ch);
}

waitch()
{
    bdos(7);
}

putch(ch,fp)
  int ch; FILE *fp;
{
    aputc(ch,fp);
}

int advsave(hdr,hlen,save,slen)
  char *hdr; int hlen; char *save; int slen;
{
    char fname[50];
    int fd;

    trm_str("File name? ");
    trm_get(fname);

    /* add the extension */
    strcat(fname,".sav");

    /* create the data file */
    if ((fd = creat(fname,0666)) == -1)
	return (0);

    /* write the header */
    if (write(fd,hdr,hlen) != hlen) {
	close(fd);
	return (0);
    }

    /* write the data */
    if (write(fd,save,slen) != slen) {
	close(fd);
	return (0);
    }

    /* close the file and return successfully */
    close(fd);
    return (1);
}

int advrestore(hdr,hlen,save,slen)
  char *hdr; int hlen; char *save; int slen;
{
    char fname[50],hbuf[50],*p;
    int fd;

    if (hlen > 50)
	error("save file header buffer too small");

    trm_str("File name? ");
    trm_get(fname);

    /* add the extension */
    strcat(fname,".sav");

    /* create the data file */
    if ((fd = open(fname,0)) == -1)
	return (0);

    /* read the header */
    if (read(fd,hbuf,hlen) != hlen) {
	close(fd);
	return (0);
    }

    /* compare the headers */
    for (p = hbuf; hlen--; )
	if (*hdr++ != *p++) {
	    trm_str("This save file does not match the adventure!\n");
	    return (0);
	}

    /* read the data */
    if (read(fd,save,slen) != slen) {
	close(fd);
	return (0);
    }

    /* close the file and return successfully */
    close(fd);
    return (1);
}

