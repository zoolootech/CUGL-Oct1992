#include <stdio.h>
#include <fcntl.h>
#include <sys\types.h>
#include <io.h>
#include <sys\stat.h>
#include <stdlib.h>

/*==============================================================*
 *                                                              *
 *    This is a program to rotate the characters in a font      *
 *    file by 90 degree in anti-clockwise direction.            *
 *                                                              *
 *    Usage : rotate infilename outfilename                     *
 *                                                              *
 *    Input file and output file must have different name.      *
 *                                                              *
 *==============================================================*/

#define ERROR (-1)
#define FONTID 51

struct fontspec {
    unsigned char width,inkwidth,height;
      signed char leftmargin;
    unsigned char topline,cellheight;
};

/* maximum number of characters in the font file is 256 */
struct fontspec fontinfo[256], outinfo[256];

unsigned char DOTVALUE[] =
{ 
    0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001
};

/* maximum size of a character is 4096 bytes. approx 180 by 180 */
unsigned char bufi[4096], bufo[4096];
int infn,outfn;

main(argc,argv)
int argc;
char *argv[];
{
    int loop,count;

    if (argc != 3) {
        fprintf(stderr,"usage: rotate infilename outfilename\n");
        exit(1);
    }
    if ((infn=open(argv[1],O_RDONLY | O_BINARY)) == ERROR) {
        fprintf(stderr,"\nrotate: Cannot open input file\n");
        exit(1);
        }
    if ((outfn=open(argv[2],O_CREAT | O_BINARY | O_TRUNC |O_WRONLY,
                          S_IREAD | S_IWRITE))
           == ERROR) {
        fprintf(stderr,"\nrotate: Cannot create output\n");
        exit(1);
        }
    if ((count=read(infn,bufi,256)) <= 0) {
errorexit:
        fprintf(stderr,"Bad Font File\n");
    }
    /* first byte has to be equal to FONTID */
    if (bufi[0]!=FONTID) goto errorexit;

    /* Look for the end of comment string */
    for (loop=1; loop<=count; loop++) {
        if (bufi[loop]==0) goto cont;
    }
    goto errorexit;
cont:
    loop++;
    /* seek to begining of font information */
    lseek(infn,(long) loop, SEEK_SET);
    loop+=16;
    if (read(infn,bufi,16) != 16) {
        fprintf(stderr,"\n\nPremature EOF!\n");
        goto cleanup;
    }
    switch(bufi[0]) {
    case 0:     /* type 0 font file */
        process0();
        break;
    case 1:     /* type 1 font file */
        process1();
        break;
    default:
        fprintf(stderr,"Invalid Font Type\n!");
    }
cleanup:
    close(infn);
    close(outfn);
}

/* process type 0 font file */
process0()
{
    unsigned char temp1,temp2;
    int loop1,loop2,nochar,charsize,outsize;
    struct fontspec inspec;

    /* see the appendix in user manual about font file format */
    inspec.width=bufi[5];
    inspec.inkwidth=bufi[6];
    inspec.height=bufi[7];
    inspec.leftmargin=bufi[8];
    inspec.topline=bufi[9];
    inspec.cellheight=bufi[10];
    bufo[0]=FONTID;

    /* Allocate 14 bytes for comment */
    for (loop1=1;loop1<15;) bufo[loop1++]=' ';
    for (loop1=15; loop1<32;) bufo[loop1++]=0;
    for (loop1=16;loop1<21;loop1++) bufo[loop1]=bufi[loop1-16];
    bufo[21]=inspec.cellheight;
    bufo[22]=inspec.height;
    bufo[23]=inspec.inkwidth;
    bufo[24]=inspec.cellheight-inspec.topline-1;
    bufo[25]=inspec.inkwidth+inspec.leftmargin-1;
    bufo[26]=inspec.width;

    /* write out the header */
    write(outfn,bufo,32);
    nochar=bufi[1]+(bufi[2]<<8);
    /* calculate input character size in byte */
    charsize=((inspec.inkwidth+7)>>3)*inspec.height;
    /* calculate output character size in byte */
    outsize=((inspec.height+7)>>3)*inspec.inkwidth;

    /* loop until all characters are processed */
    while (nochar--) {
        if (read(infn,bufi,charsize)!=charsize) {
            fprintf(stderr,"\n\nPremature EOF!\n");
            goto cleanup;
        }    
        rotate(&inspec,bufi,bufo);
        write(outfn,bufo,outsize);
    }
cleanup: ;
}

/* process type 1 font file */
process1()
{
    unsigned char temp1,temp2;
    int loop1,loop2,nochar,charsize,outsize;
    long size;

    bufo[0]=FONTID;
    /* allocate 14 bytes for comment */
    for (loop1=1;loop1<15;) bufo[loop1++]=' ';
    for (loop1=15;loop1<32;) bufo[loop1++]=0;
    for (loop1=16;loop1<24;loop1++) bufo[loop1]=bufi[loop1-16];
    nochar=bufi[1]+(bufi[2]<<8);

    /* read characters information */
    if (read(infn,fontinfo,6*nochar)!=6*nochar) {
        fprintf(stderr,"\n\nPremature EOF!\n");
        goto cleanup;
    }
    size=0L;
    for (loop1=0; loop1<nochar; loop1++) {
        size+=(long) ((fontinfo[loop1].height+7)>>3)*fontinfo[loop1].inkwidth;
        /* transform the character information */
        rotspec(&fontinfo[loop1],&outinfo[loop1]);
    }
    bufo[21]=(unsigned char) (size & 0xff);
    bufo[22]=(unsigned char) ((size>>8) & 0xff);
    bufo[23]=(unsigned char) ((size>>16) & 0xff);
    write(outfn,bufo,32);
    if (write(outfn,outinfo,6*nochar)!=6*nochar) {
        fprintf(stderr,"\nError writing the output file!\n");
        goto cleanup;
    }
    for (loop1=0; loop1<nochar; loop1++) {
        charsize=((fontinfo[loop1].inkwidth+7)>>3)*fontinfo[loop1].height;
        outsize=((fontinfo[loop1].height+7)>>3)*fontinfo[loop1].inkwidth;
        if (read(infn,bufi,charsize)!=charsize) {
            fprintf(stderr,"\n\nPremature EOF!\n");
            goto cleanup;
        }    
        rotate(&fontinfo[loop1],bufi,bufo);
        write(outfn,bufo,outsize);
    }
cleanup: ;
}

/* rotate the pattern given in bufi and stored output in bufo */
rotate(spec,bufi,bufo)
struct fontspec *spec;
char bufi[], bufo[];
{
    int wb, tw, loop1, loop2, loop3, outptr,count;
    unsigned char out;

    outptr=0;

    /* Hard to describe. You will understand it after tracing once */
    wb=(spec->inkwidth+7)>>3;
    tw=(spec->inkwidth-1) & 0x07;
    for (loop1=wb-1; loop1>=0 ; loop1--) {
        for (loop2=tw; loop2>=0 ; loop2--) {
            out=count=0;
            for (loop3=loop1; loop3<(wb*spec->height); loop3+=wb) {
                if (count++ == 8) {
                    bufo[outptr++]=out;
                    out=0;
                    count=1;
                }
                out <<= 1;
                if (bufi[loop3] & DOTVALUE[loop2])
                    out |= 1;
            }
        if (count<8) out <<= 8-count;
        bufo[outptr++]=out;
        }
    tw=7;
    }
}

/* transform the character information */
rotspec(in,out)
struct fontspec *in, *out;
{
    out->width=in->cellheight;
    out->inkwidth=in->height;
    out->height=in->inkwidth;
    out->topline=in->inkwidth+in->leftmargin-1;
    out->cellheight=in->width;
    out->leftmargin=in->cellheight-in->topline-1;
}

