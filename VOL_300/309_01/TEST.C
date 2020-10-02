#include startup.h

#include headers\hercs.h
#include headers\acia.h
#include headers\dioboard.h

#code
main()
{
        char ch, ch1;
        initvideo();
        initacia();
        initdio();
        ch1 = 0;
        while( 1 ) {
                writeattr("This is bold text",BOLD);
                writeln(" ");
                for( ch = 'a'; ch <= 'z'; ch++) {
                        writecharattr(ch, REVERSE);
                        writeleds(ch);
                        putchar(ch);
                        writeseg( ch1 ); ch1++;
                        if( ch1 > '9' ) ch1 = '0';
                }
        }
}
