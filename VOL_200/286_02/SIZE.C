#include <stdio.h>
#include <fcntl.h>
#include <sys\types.h>
#include <io.h>
#include <sys\stat.h>
#include <stdlib.h>

#define ERROR (-1)
#define FONTID 51

struct fontspec {
    unsigned char width,inkwidth,height;
      signed char leftmargin;
    unsigned char topline,cellheight;
};

struct fontspec fontinfo[256], outinfo[256];

unsigned char DOTVALUE[] =
{ 
    0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001
};

unsigned char bufi[4096], bufo[4096];
int infn,outfn, mode=0, option=0;

main(argc,argv)
int argc;
char *argv[];
{
    int loop, count, ret;
    char ch;

    argv++;
    while (argc-- > 1 && *(*argv)++=='-') {
        if (**argv) {
            ch= *(*argv)++;
            ret=atoi(*argv);
        } else {
            ch=0;
        }
        switch(ch) {
        case 's':
            if (ret > 2) {
                fprintf(stderr,"only 0, 1 or 2 can be used with -s\n");
                exit(1);
            }
            option=ret;
            break;
        case 'o':
            if (ret > 4) {
                fprintf(stderr,"only 0 to 3 can be used with -o\n");
                exit(1);
            }
            mode=ret;
            break;
        default:
            fprintf(stderr,"Unknow option -%c ** IGNORED **\n",ch);
            break;
        }
        argv++;
    }

    if (argc!=2) {
        fprintf(stderr,"size: size [options] infile outfile\n\n");
        fprintf(stderr,"Available options include:\n");
        fprintf(stderr,"    -s0  reduce height to half\n");
        fprintf(stderr,"    -s1  double the width\n");
        fprintf(stderr,"    -s2  double the height\n\n");
        fprintf(stderr,"-o option is used with reduction only\n");
        fprintf(stderr,"    -o0  ink dominate\n");
        fprintf(stderr,"    -o1  perfer ink\n");
        fprintf(stderr,"    -o2  perfer space\n");
        fprintf(stderr,"    -o3  space dominate\n");
        exit(1);
    }
    (*argv)--;
    if ((infn=open(argv[0],O_RDONLY | O_BINARY)) == ERROR) {
        fprintf(stderr,"\nsize: Open file error -- %s\n",argv[0]);
        exit(1);
        }
    if ((outfn=open(argv[1],O_CREAT | O_BINARY | O_TRUNC |O_WRONLY,
                          S_IREAD | S_IWRITE))
           == ERROR) {
        fprintf(stderr,"\nsize: Create file error -- %s\n",argv[1]);
        exit(1);
        }
    if ((count=read(infn,bufi,256)) <= 0) {
errorexit:
        fprintf(stderr,"Bad font file\n");
    }
    if (bufi[0]!=FONTID) goto errorexit;
    for (loop=1; loop<=count; loop++) {
        if (bufi[loop]==0) goto cont;
    }
    goto errorexit;
cont:
    loop++;
    lseek(infn,(long) loop, SEEK_SET);
    if (read(infn,bufi,16) != 16) {
        fprintf(stderr,"\n\nPremature EOF!\n");
        goto cleanup;
    }
    switch(bufi[0]) {
    case 0:
        process0();
        break;
    case 1:
        process1();
        break;
    default:
        fprintf(stderr,"Invalid Font Type\n!");
    }
cleanup:
    close(infn);
    close(outfn);
}

process0()
{
    unsigned char temp1,temp2;
    int loop1,loop2,nochar,charsize,outsize;
    struct fontspec inspec, outspec;

    inspec.width=bufi[5];
    inspec.inkwidth=bufi[6];
    inspec.height=bufi[7];
    inspec.leftmargin=bufi[8];
    inspec.topline=bufi[9];
    inspec.cellheight=bufi[10];
    chgspec(&inspec, &outspec);
    bufo[0]=FONTID;
    for (loop1=1;loop1<15;) bufo[loop1++]=' ';
    for (loop1=15;loop1<32;) bufo[loop1++]=0;
    for (loop1=16;loop1<27;loop1++) bufo[loop1]=bufi[loop1-16];
    bufo[21]=outspec.width;
    bufo[22]=outspec.inkwidth;
    bufo[23]=outspec.height;
    bufo[24]=outspec.leftmargin;
    bufo[25]=outspec.topline;
    bufo[26]=outspec.cellheight;
    write(outfn,bufo,32);
    nochar=bufi[1]+(bufi[2]<<8);
    charsize=((inspec.inkwidth+7)>>3)*inspec.height;
    outsize=((outspec.inkwidth+7)>>3)*outspec.height;
    while (nochar--) {
        if (read(infn,bufi,charsize)!=charsize) {
            fprintf(stderr,"\n\nPremature EOF!\n");
            goto cleanup;
        }    
        size(&inspec,bufi,bufo);
        write(outfn,bufo,outsize);
    }
cleanup: ;
}

process1()
{
    unsigned char temp1,temp2;
    int loop1,loop2,nochar,charsize,outsize;
    unsigned char *curptr;
    long filesize;

    bufo[0]=FONTID;
    for (loop1=1;loop1<15;) bufo[loop1++]=' ';
    for (loop1=15;loop1<32;) bufo[loop1++]=0;
    for (loop1=16;loop1<24;loop1++) bufo[loop1]=bufi[loop1-16];
    nochar=bufi[1]+(bufi[2]<<8);
    if (read(infn,fontinfo,6*nochar)!=6*nochar) {
        fprintf(stderr,"\n\nPremature EOF!\n");
        goto cleanup;
    }
    filesize=0L;
    for (loop1=0; loop1<nochar; loop1++) {
        chgspec(&fontinfo[loop1],&outinfo[loop1]);
        filesize+=(long)((outinfo[loop1].inkwidth+7)>>3)*outinfo[loop1].height;
    }
    bufo[21]=(unsigned char) (filesize & 0xff);
    bufo[22]=(unsigned char) ((filesize>>8) & 0xff);
    bufo[23]=(unsigned char) ((filesize>>16) & 0xff);
    write(outfn,bufo,32);
    if (write(outfn,outinfo,6*nochar)!=6*nochar) {
        fprintf(stderr,"\nError writing File!\n");
        goto cleanup;
    }
    for (loop1=0; loop1<nochar; loop1++) {
        charsize=((fontinfo[loop1].inkwidth+7)>>3)*fontinfo[loop1].height;
        outsize=((outinfo[loop1].inkwidth+7)>>3)*outinfo[loop1].height;
        if (read(infn,bufi,charsize)!=charsize) {
            fprintf(stderr,"\n\nPremature EOF!\n");
            goto cleanup;
        }    
        size(&fontinfo[loop1],bufi,bufo);
        write(outfn,bufo,outsize);
    }
cleanup: ;
}

chgspec(in,out)
struct fontspec *in, *out;
{
    switch(option) {
    case 0:
        out->width=in->width;
        out->inkwidth=in->inkwidth;
        out->height=(in->height+1)>>1;
        out->topline=(in->topline+1)>>1;
        out->cellheight=(in->cellheight+1)>>1;
        out->leftmargin=in->leftmargin;
        break;
    case 1:
        out->width=in->width<<1;
        out->inkwidth=in->inkwidth<<1;
        out->height=in->height;
        out->topline=in->topline;
        out->cellheight=in->cellheight;
        out->leftmargin=in->leftmargin*2;
        break;
    case 2:
        out->width=in->width;
        out->inkwidth=in->inkwidth;
        out->height=in->height<<1;
        out->topline=in->topline<<1;
        out->cellheight=in->cellheight<<1;
        out->leftmargin=in->leftmargin;
        break;
    }
}

size(spec,bufi,bufo)
struct fontspec *spec;
unsigned char *bufi, *bufo;
{
    switch(option) {
    case 0:
        RH2to1(spec,bufi,bufo);
        break;
    case 1:
        EW1to2(spec,bufi,bufo);
        break;
    case 2:
        EH1to2(spec,bufi,bufo);
    }
}

/* Reduce height in the ratio 2 to 1 */
RH2to1(spec,bufi,bufo)
struct fontspec *spec;
unsigned char *bufi, *bufo;
{
    int loop1, loop2, count, wb, height;
    unsigned int temp1, temp2;

    wb=(spec->inkwidth+7)>>3;   /* calculate width in byte */
    height=spec->height;
    if (height==0 || wb==0) return;
    for (; height!=3 && height!=1 && height>0 ; height-=2, bufi+=wb) {
      for (loop1=0; loop1 < wb ; loop1++, bufi++) {
        switch(mode) {
        case 0:
        case 1:
            *bufo++= *bufi | *(bufi+wb);
            break;
        case 2:
        case 3:
            *bufo++= *bufi & *(bufi+wb);
            break;
        }
      }
    }
    if (height==1) {
        for (loop1=0; loop1<wb; loop1++) *bufo++ = *bufi++;
    } else {
        for (loop1=0; loop1<wb; loop1++,*bufi++) {
            switch(mode) {
            case 0:
            case 1:
                *(bufo+wb)= *(bufi+wb) | *(bufi+wb+wb);
                *bufo++ = *bufi | *(bufi+wb);
                break;
            case 2:
            case 3:
                *(bufo+wb)= *(bufi+wb) & *(bufi+wb+wb);
                *bufo++ = *bufi & *(bufi+wb);
                break;
            }
        }
    }
}

/* Enlarge width in the ratio 1 to 2 */
EW1to2(spec,bufi,bufo)
struct fontspec *spec;
unsigned char *bufi, *bufo;
{
    int loop1, loop2, count, wb, height;
    unsigned int temp1, temp2;

    wb=(spec->inkwidth+7)>>3;   /* calculate width in byte */
    count=(((spec->inkwidth<<1)+7)>>3) & 0x01;
    height=spec->height;
    if (height==0 || wb==0) return;
    for (;height>0; height--) {
        for (loop1=0; loop1 < wb; loop1++, bufi++) {
            temp1=0;
            for (loop2=0; loop2<8; loop2++) {
                temp1 <<= 2;
                temp1 |= (*bufi & 0x80) ? 3 : 0;
                *bufi <<= 1;
            }
            *bufo++ = temp1 >> 8;
            *bufo++ = temp1 & 0xff;
        }
        if (count) *bufo--;
    }
}

/* Enlarge height in the ratio 1 to 2 */
EH1to2(spec,bufi,bufo)
struct fontspec *spec;
unsigned char *bufi, *bufo;
{
    int loop1, loop2, wb, height;

    wb=(spec->inkwidth+7)>>3;   /* calculate width in byte */
    height=spec->height;
    if (height==0 || wb==0) return;
    for (;height>0; height--, bufo+=wb) {
        for (loop1=0; loop1 < wb; loop1++, bufi++, bufo++) {
            *(bufo+wb)= *bufo= *bufi;
        }
    }
}


