/* hercs.h */

#define CRTC            0x93b0
#define INDEX           4
#define DATA            5
#define MODE            8
#define VIDMEM          0x8000
#define BOLD            0x0f
#define NORMAL          0x07
#define UNDERLINE       0x01
#define REVERSE         0x70
#define BLINKING        0x87

#data
char cursx, cursy;

#const
char vidtab[16] = {97,80,82,15,25,6,25,25,2,13,12,0,0,0,0,0};

#code
initvideo()
{
  int regnumber;
  char *videomem;
  char *videoregs;

  videoregs = CRTC;
  videomem = VIDMEM;

  home();
  videoregs[MODE] = 0;
  clrscr();
  for( regnumber = 0; regnumber < 16; regnumber = regnumber + 1)
  {
      videoregs[INDEX] = regnumber;
      videoregs[DATA] = vidtab[regnumber];
  }
  videoregs[MODE] = 0x28;
}

clrscr()
{
  int videopos;
  char *videomem;
  videomem = VIDMEM;
  for( videopos = 0; videopos < 4000; videopos = videopos+2 )
  {
     videomem[videopos] = 0x20;
     videomem[videopos+1] = NORMAL;
  }
}

gotoxy( x, y )
char x, y;
{
  cursx = x;
  cursy = y;
  if( cursx > 79 )
  {
     cursx = 0;
     cursy = cursy + 1;
  }
  if( cursy > 24 )
  {
     cursy = 24;
     scrollup();
  }
}

home()
{
   cursx = 0;
   cursy = 0;
}

scrollup()
{
   int videoffset;
   char *videomem;
   videomem = VIDMEM;
   for( videoffset = 160; videoffset < 4000; videoffset = videoffset+1)
   {
      videomem[videoffset - 160] = videomem[videoffset];
   }
   for( videoffset = 3840; videoffset < 4000; videoffset = videoffset + 2 )
   {
      videomem[videoffset] = 0x20;
   }
}

writecharattr( ch, attrib )
char ch;
char attrib;
{
   int videoffset;
   char *videomem;
   videomem = VIDMEM;
   videoffset = ((cursy * 80) + cursx) * 2;
   if( ch == 0x0d ) cursx = 0;
   else if( ch == 0x0a ) cursy = cursy + 1;
   else
   {
      videomem[videoffset] = ch;
      videomem[videoffset+1] = attrib;
      cursx = cursx + 1;
   }
   gotoxy( cursx, cursy );
}

write( string )
char *string;
{
   while( *string )
   {
      writecharattr( *string, NORMAL );
      string++;
   }
}

writeattr( string, attr )
char *string, attr;
{
   while( *string )
   {
      writecharattr( *string, attr );
      string++;
   }
}

writeln( string )
char *string;
{
   while( *string )
   {
        writecharattr( *string, NORMAL );
        string++;
   }
   writecharattr( 0x0a, NORMAL );
   writecharattr( 0x0d, NORMAL );
}

writeint(value)
int value;
{
        int x;
        char sign, str[8];
        str[7] = '\0';
        for( x = 0; x < 7; x++ )  str[x] = ' ';
        if( value < 0 ) {
                sign = '-';
                value = (-value);
        }
        else sign = '+';
        x = 6;
        do {
                str[x--] = (value % 10) + '0';
                value /= 10;
        } while( value >  0 );
        str[x] = sign;
        write( str );
}

writehexbyte(value)
char value;
{
   char temp;
   temp = ((value & 0xf0) >> 4);
   if( temp > 9 ) temp = temp + 7;
   temp = temp + 0x30;
   writecharattr( temp, NORMAL);
   temp = value & 0x0f;
   if( temp > 9 ) temp = temp + 7;
   temp = temp + 0x30;
   writecharattr( temp, NORMAL);
}

