/*
** include doscall.c -- functions to invoke DOS
*/
#include <doscall.h>
 
 
/*
** variables for communicating with #asm code
*/
 
static char
  AH, AL;
static int
  CX, DX;
 
 
/*
** determine if a character is a legal DOS file name character
*/
 
_doschar(ch) char ch; {
  char *ptr;
  if((ch >= 'a') & (ch <= 'z')) return 1;
  if((ch >= 'A') & (ch <= 'Z')) return 1;
  if((ch >= '0') & (ch <= '9')) return 1;
  ptr = "$&#@!%'()-{}_^~`";
  while(*ptr) if(ch == *ptr++) return 1;
  return 0;
  }
 
 
/*
** perform a DOS function call
*/
 
_dosfcall(function, dreg) int function, dreg; {
  AH = function;
  DX = dreg;
#asm
       MOV AH,QAH      ; get the function code
       MOV DX,QDX
       INT 021H        ; call DOS
       MOV QAL,AL
#endasm
  return AL;
  }
 
 
/*
** perform a DOS function call using CX
*/
 
_dosfxcall(function, dreg, cin, cout) int function, dreg, cin, *cout; {
  AH = function;
  CX = cin;
  DX = dreg;
#asm
       MOV AH,QAH      ; get the function code
       MOV CX,QCX
       MOV DX,QDX
       INT 021H        ; call DOS
       MOV QAL,AL
       MOV QCX,CX
#endasm
  *cout = CX;
  return AL;
  }
 
 
/*
** get date from DOS
*/
 
_dosdate(year, month, day) int *year, *month, *day; {
#asm
       MOV AH,02AH
       INT 021H
       MOV QCX,CX
       MOV QDX,DX
#endasm
  *year = CX;
  *month = (DX >> 8) & 255;
  *day = DX & 255;
  }
 
 
/*
** get time from DOS
*/
 
_dostime(hours, minutes, seconds, hundreths)
    int *hours, *minutes, *seconds, *hundreths; {
#asm
       MOV AH,02CH
       INT 021H
       MOV QCX,CX
       MOV QDX,DX
#endasm
  *hours = (CX >> 8) & 255;
  *minutes = CX & 255;
  *seconds = (DX >> 8) & 255;
  *hundreths = DX & 255;
  }
 
