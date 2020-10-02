#include <dos.h>
#include <conio.h>

#define  ONTONE  outportb( 0x61, inport( 0x61 ) | 3 )
#define  OFFTONE outportb( 0x61, inport( 0x61 ) & 0xFC )
static void setfreq(unsigned int);
static unsigned long int getticks();

void     beep(unsigned int pitch, unsigned int nticks )
/* Sound the speaker using indicated pitch for nticks long
   for error sound use beep(440,3) beep(220,3)
*/   	
{
unsigned long int ticks,ticksa;

    setfreq( pitch );
    ticksa = getticks();
    while ( ( ticks = getticks() ) == ticksa )
        ;
    ONTONE;
    ticks = ticks + (unsigned long int) nticks;
    while ( getticks() < ticks )
        ;
    OFFTONE;
}


static void setfreq(unsigned int count )
{

    outp( 0x43, 0xB6 );
    outp( 0x42, (int) (count & 0xFF) );
    outp( 0x42, (int) (count >> 8) );
}

static unsigned long int getticks()
{
union   REGS regs;

    regs.h.ah = 0x00;
    int86( 0x1A, &regs, &regs );
    return( ((unsigned long int) regs.x.cx << 16) + regs.x.dx );
}

