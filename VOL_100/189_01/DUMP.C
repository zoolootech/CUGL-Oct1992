/* hexdump.c:   dump a file to standard output in hex format */

#include <stdio.h>
#include <ctype.h>

main(argc,argv)
int argc;
char *argv[];
{
    FILE *f;
    int i;
    
    for (i = 1; i < argc; ++i)
        if ((f = fopen(argv[i],"rb")) == NULL)
            fprintf(stderr,"can't open %s\n");
        else
        {
            dump(f,argv[i]);
            fclose(f);
            putchar('\f');
        }
}

dump(f,s)
FILE *f;
char *s;        
{
    unsigned char buf[16];
    int count, i;
    long size;
    
    printf("Dump of %s\n\n",s);
    size = 0;
    
    while ((count = fread(buf,1,16,f)) > 0)
    {
        printf("  %06X ",size+=count);

        /* ..Print Hex Bytes.. */
        for (i = 0; i < 16; ++i)
        {
            if (i < count)
            {
                printf(" %02x",buf[i]);
                if (iscntrl(buf[i]))
                    buf[i] = '.';
            }
            else
            {
                fputs("   ",stdout);
                buf[i] = ' ';
            }
            if (i == 7)
                putchar(' ');
        }
       
        /* ..Print Text Bytes.. */
        printf(" |%16.16s|\n",buf);
    }
}C             �t�0�  FTRIM   C             ڐ} V  