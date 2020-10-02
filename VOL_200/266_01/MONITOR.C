/*
      Common code for all IBM-compatible graphics monitor drivers.
      Copyright 16 April 1990, Robert L. Patton, Jr.
*/
#define GETINT GetWord
#define  GETXY(A,B) A=DX(GETINT(Plot))+Xo;B=DY(GETINT(Plot))+Yo;

void Line (int X1,int Y1,int X2,int Y2)
/*   ====               Draws a straight line */
{
  int Dx, Dy, D1x, D1y, D2x, D2y, M, N, S, K;

  Dx = X2 - X1;
  Dy = Y2 - Y1;
  D1x = Sign(Dx);
  D1y = Sign(Dy);
  D2x = D1x;
  D2y = 0;
  M = abs(Dx);
  N = abs(Dy);
  if (M <= N) {
    D2x = 0;
    D2y = D1y;
    M = abs(Dy);
    N = abs(Dx);
  }
  S = M/2;
  for (K=0; K<=M; K++)
  {
    if (Dotter())  DOT (X1,Y1);
    S += N;
    if (S >= M) {
      S -= M;
      X1 += D1x;
      Y1 += D1y;
    }
    else {
      X1 += D2x;
      Y1 += D2y;
    }
  }
}
/* ----------------------------------------------------*/
main(int ArgC,char **ArgV)
{
  FILE *Plot;
  #include "bitfont.h"
  static int  Command=0,Xp=0,Yp=0,Xo=0,Yo=0;
  int  I,J,M,N,X,Y,X2,Y2,Shape,Pattern,Density,Hit,Gap,Color;
  unsigned Count=0;

  if((Plot=fopen("PLOTCOM.DAT","rb"))==NULL) {
      puts("Plot file, PLOTCOM.DAT not found. No picture possible.\n");
      puts("Do a PLOX command on a .PIC file before trying to plot.\n");
      exit(0);
  }
  TVON;
  if (ArgC>1) NEWHUE(atoi(ArgV[1]));
  else        NEWHUE(NORMAL);
  SetLine (SOLID);
  while (Command<99)  {
    Command=getc(Plot);
    if(Command==EOF) Command=99;
    switch (Command)
    {
      case   ON: GETXY(Xp,Yp)    /* Set pixel */
                 DOT(Xp,Yp);
                 break;

      case MARK: GETXY(X,Y)       /* Move and mark */
                 Line(Xp,Yp,X,Y);
                 Xp=X;Yp=Y;
                 break;

      case MOVE: GETXY(Xp,Yp)    /* Move */
                 break;

      case LTYP: SetLine (getc(Plot)); /* Set line type */
                 break;
 /**/
      case TXTH: ;                /* Font 1 horizontal */
      case TXTV: GETXY(X,Y)       /* Font 1 vertical */
                 while ((N=getc(Plot))!=0) {
                  if(N<0x20) N=0x20;
                  else if(N>0x5F) N-=0x20;
                  N=Font1[N-0x20];
                  for(J=0;J<5;J++)
                    for(I=0;I<3;I++) {
                      if(N&1) DOT(X+I,Y-J);
                        N=N>>1;
                    }
                  if (Command==TXTH) X+=5;
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
                      if (M&1) DOT(X+I,Y-J);
                      if (N&1) DOT(X+I,Y-J-3);
                      M=M>>1;
                      N=N>>1;
                    }
                    if (Command==TX2H) X+=7;
                    else               Y-=8;
                  }
                  Xp=X; Yp=Y;
                  break;

      case SYMB: Shape=getc(Plot); /* Draw symmetrical symbol */
                 for(J=1;J<3;J++)
                   for(I=2;I>=0;I--) {
                     if(Shape&1) {
                       DOT(Xp+I,Yp+J);
                       DOT(Xp-J,Yp+I);
                       DOT(Xp-I,Yp-J);
                       DOT(Xp+J,Yp-I);
                     }
                     Shape=Shape>>1;
                   }
                 if(Shape%2) DOT(Xp,Yp);
                 break;

      case ORIG: Xo=DX(GETINT(Plot)); /* New origin */
                 Yo=DY(GETINT(Plot));
                 break;

      case MRGN: GETINT(Plot);  /* No margin with crt graph */
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
                     if ((I-X+Hit)%Gap) NODOT(I,J)
                     else                 DOT(I,J);
                 }
                 break;

      case  HUE: Color = GETINT(Plot);
                 NEWHUE(Color);
                 break;

      case CLOS: fclose(Plot);
                 Xp=Yp=Xo=Yo=0;
                 break;
    }
  }
  getch();
  TVOFF;
}
