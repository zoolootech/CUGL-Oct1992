/*     Line Printer Control Routines - Epson printers            FILE
                  Machine Dependent Code                        LPTR.C
                    CP/M and MS-DOS
                      01 May 1990
*/
#define LIST    5  /* system call to send char to printer */
#define ESC    27
#define TYPLEN 14
#define EQUAL(A,B) !strcmp((A),(B))

char Type[TYPLEN+1] = "GENERIC";

/*   ============        Sets printer line spacing in 72nds of an inch  */
void LP_LineSpace (int N72nds) {
/*   ============ */
  bdos(LIST,ESC,0);
  bdos(LIST,'A',0);
  bdos(LIST,N72nds,0);
}
/*   ============           Sets normal density bit image for graphics  */
void LP_GraphMode (int LineBits) {
/*   ============ */
  bdos(LIST,ESC,0);
  if (EQUAL(Type,"FX80")) {
      bdos(LIST,'*',0);   /* 72 dots/inch horizontal */
      bdos(LIST,5,0);
  }
  else {
      bdos(LIST,'K',0);   /* 60 dots/inch horizontal */
  }
  bdos(LIST,LineBits%256,0);
  bdos(LIST,LineBits/256,0);
}
/*   =========            Sets the left margin  */
void LP_Margin (Column) {
/*   ========= */
  bdos(LIST,ESC,0);
  bdos(LIST,'l',0);
  bdos(LIST,Column,0);
  bdos(LIST,13,0);
}
/*   =======           Carriage-return(s), line-feed(s) to printer  */
void LP_CrLf (int N) {
/*   ======= */
  int i;
  for (i=0; i<N; i++)
  { bdos(LIST,10,0);
    bdos(LIST,13,0);
  }
}
/*   =======                Sends the given byte to the printer  */
void LP_Send (int Byte) {
/*   ======= */
  bdos(LIST,Byte,0);
}
/*   ==========                    Set new printer type */
void LP_SetType (char *NewType) {
/*   ========== */
  strncpy (Type,NewType,TYPLEN);
}
/*   ========                Reset the printer */
void LP_Reset () {
/*   ======== */
  bdos(LIST,ESC,0);
  bdos(LIST,'@',0);
}
