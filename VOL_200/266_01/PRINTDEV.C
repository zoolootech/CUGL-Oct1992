/*                                                   File: PRINTDEV.C
       Common code for dot-matrix printer drivers
       Copyright 02 May 1990, Robert L. Patton, Jr.
   Designed to be included and customized by a main driver that
   has all the standard #include files and that defines HSPACE,
   PRINTYPE and DX.
*/
#define GETINT GetWord
#define GETXY(A,B) A=DX(GETINT(Plot))+Xo;B=GETINT(Plot)+Yo;
#define HBUMP1 HSPACE+3
#define HBUMP2 HSPACE+5

main(ArgC,ArgV)
int  ArgC;
char *ArgV[];
{
  FILE *Plot;
  static int  Command=0,Xp=0,Yp=0,Xo=0,Yo=0,Bold=0;
  int  I,J,M,N,X,Y,X2,Y2,Shape,Pattern,Density,Hit,Gap;
  unsigned Count=0;
  #include "bitfont.h"

  LP_SetType (PRINTYPE);
  if (ArgC > 1) Bold = 1;
  Plot=fopen("PLOTCOM.DAT","rb");
  NewImage();
  while (Command<99)  {
    Command=getc(Plot);
    if(Command==EOF) Command=99;
    if(++Count%10==0) putchar('+');
    else              putchar('.');
    if(Count%50==0)   putchar('\n');
    switch (Command)
    {
      case   ON: GETXY(Xp,Yp)    /* Set pixel */
                 Dot(Xp,Yp);
                 break;

      case MARK: GETXY(X,Y)       /* Move and mark */
                 Line(Xp,Yp,X,Y);
                 Xp=X;Yp=Y;
                 break;

      case MOVE: GETXY(Xp,Yp)    /* Move */
                 break;

      case LTYP: SetLine (getc(Plot)); /* Set line type */
                 break;

      case TXTH: ;                /* Font 1 horizontal */
      case TXTV: GETXY(X,Y)       /* Font 1 vertical */
                 while ((N=getc(Plot))!=0) {
                  if(N<0x20) N=0x20;
                  else if(N>0x5F) N-=0x20;
                  N=Font1[N-0x20];
                  for(J=0;J<5;J++)
                    for(I=0;I<3;I++) {
                      if(N&1) Dot(X+I,Y+J);
                        N=N>>1;
                    }
                  if (Command==TXTH) X+=HBUMP1;
                  else               Y-=7;
                 }
                 Xp=X;Yp=Y;
                 break;

      case TX2H:                  /* Font 2 horizontal */
      case TX2V: GETXY(X,Y)       /* Font 2 vertical */
                 while ((N=getc(Plot))!=0) {
                  if(N<0x20) N=0x20;
                  N-=0x20;
                  M=Font2[N][0];
                  N=Font2[N][1];
                  for (I=0;I<5;I++)
                    for (J=0;J<3;J++) {
                      if (M&1) Dot(X+I,Y+J);
                      if (N&1) Dot(X+I,Y+J+3);
                      M=M>>1;
                      N=N>>1;
                    }
                    if (Command==TX2H) X+=HBUMP2;
                    else               Y-=8;
                  }
                  Xp=X; Yp=Y;
                  break;

      case SYMB: Shape=getc(Plot); /* Draw symmetrical symbol */
                 for(J=1;J<3;J++)
                   for(I=2;I>=0;I--) {
                     if(Shape&1) {
                       Dot(Xp+I,Yp+J);
                       Dot(Xp-J,Yp+I);
                       Dot(Xp-I,Yp-J);
                       Dot(Xp+J,Yp-I);
                     }
                     Shape=Shape>>1;
                   }
                 if(Shape%2) Dot(Xp,Yp);
                 break;

      case ORIG: Xo=GETINT(Plot); /* New origin */
                 Yo=GETINT(Plot);
                 break;

      case MRGN: LP_Margin(N=GETINT(Plot)); /* Set Margin */
                 break;

      case HTYP: Pattern=GETINT(Plot);  /* Set hatch type */
                 Density=GETINT(Plot);
                 SetHatch (Pattern,Density);
                 break;

      case FILL: GETXY(X,Y)    /* Hatch area */
                 GETXY(X2,Y2)
                 for (J=Y+1;J<Y2;J++) {
                   HatchHow (J-Y,&Hit,&Gap);
                   for (I=X+1;I<X2;I++)
                     if (!((I-X+Hit)%Gap)) Dot(I,J);
                     else                NoDot(I,J);
                 }
                 break;

      case CLOS: M=GETINT(Plot);   /* Print picture */
                 N=GETINT(Plot);
                 Draw(M,N,Bold);
                 Xp=Yp=Xo=Yo=0;
                 break;

      case  HUE: M=GETINT(Plot);   /* Absorb the color code */
                 break;
    }
  }
}
