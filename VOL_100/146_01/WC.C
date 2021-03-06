/*
HEADER:		CUG146.07;
TITLE:		Word Count;
DESCRIPTION:  	"This program counts total number of lines, words and
              	characters in the specified file."
FILENAME:     	wc.c;
*/

              
/*  This program counts total number of lines, words
       and characters in the specified file           */

#define CR 13
#define YES 1
#define NO 0

main(argc,argv)
       int argc, *argv[];
{      int c, lines, nw, nc, nc1, inword, *fp;
       if(argc != 2) return;
       fp=fopen(argv[1],"r");
       inword=NO;
       lines=nw=nc=nc1=0;
       nl();
       while((c=getc(fp)) >= 0)
       {       ++nc;
               if(nc > 9999)
               {       ++nc1; nc=0;
                       }
               if(c == CR)
                       ++lines;
               if(c <= ' ')
                       inword=NO;
               else if(inword == NO)
               {       inword=YES;
                       ++nw;
                       }
               }
       nl();puts("# of lines = ");putnum(lines);
       nl();puts("# of words = ");putnum(nw);
       nl();puts("# of chars = ");
       if(nc1)  {
               putnum(nc1);puts("0000 + ");
                }
       putnum(nc);
       nl();
       fclose(fp);
       }

nl()
{      putchar(CR);
       }

putnum(n)
       int n;
{      if(n < 0)
       {       putchar('-');
               n=(-n);
               }
       if(n > 9)
               putnum(n/10);
       putchar('0'+(n%10));
       }
      