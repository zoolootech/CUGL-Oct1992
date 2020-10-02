/*
** stdio.c -- standard I/O library
**
** Rewritten by D. R. Hicks -- October 1983
**
** These functions have been modeled after the UNIX interfaces
** as described in "The UNIX System" by S. R. Bourne.  Unfortunately
** my lack of hands-on experience with UNIX will no doubt cause me to
** misinterpret some details.
*/
#define NOCCARGC /* don't pass argument count to functions */
                 /* called by these functions */
 
#define FILE           int     /* mimic standard C file pointers */
 
#define NUMFILES       20      /* maximum number of base I/O files */
#define _NFILE         20      /* maximum number of stream I/O files */
#define _BUFSIZE       512     /* size of stream I/O buffers */
#define PMODE          0       /* file creation options */
 
#include "errno.h"
 
 
/*
** externs
*/
extern int
  fputs(),
  getparm(),
  _doschar(),
  _dosfcall(),
  _dosfxcall();
 
 
/*
** control variables for stack, heaps, etc
*/
int
  _dataseg, /* DS segment reg value */
  _stattop, /* top of statics (in paragraphs) */
  *_heapbase, /* bottom of heap (and top of static storage -- in bytes) */
  *_heapfree, /* first possible free space entry -- address rel to DS */
  *_heaptop, /* current top of heap -- address rel to DS */
  _stkbase,  /* highest (almost) stack offset -- set SP here prior to _EXIT */
  _buffbot, /* bottom of buffer heap (in paragraphs) */
  _buff,    /* buffer heap anchor */
  _bufftop; /* top of buffer heap area (in paragraphs) */
 
 
/*
** define stream pointers for stdio
*/
FILE
  *stdin, /* standard input */
  *stdout, /* standard output */
  *stderr; /* standard error */
 
 
/*
** UNIX-like error code (see errno.h)
*/
#asm /* temp */
Q_ERRNO  LABEL   WORD
         PUBLIC  Q_ERRNO
#endasm
int
  errno;
 
/*
** arguments, return code, etc for main()
*/
static int
  _argc,       /* argument count for main */
  _argv[65],   /* array of character pointers (maximum possible) */
  _mainrc;     /* return code from main */
static char
  _argl[128],  /* argument string copied from PSP (null terminated) */
  *inname,     /* standard in name */
  *outname;     /* standard out name */
 
 
/*
** recursion flag for exit()
*/
static int
  _recur;
 
 
/*
** user's main entry point
*/
extern int
  main();
 
/*
** link module entry point (from DOS)
*/
#define MAIN_ENT _LOGIN /* define label to go on the END statement */
_login() {
#asm
       ASSUME  DS:NOTHING,ES:NOTHING,SS:NOTHING
       MOV     CX,DATASEG
       MOV     ES,CX           ; set up addressability to the data segment
       ASSUME  ES:DATASEG
       MOV     Q_DATASEG,CX    ; save data segment address for others
       MOV     DX,DS           ; save the PSP address
       MOV     Q_BUFFBOT,0     ; above-stack buffer heap does not exist
       MOV     Q_BUFF,0        ; clear buffer heap anchor
       MOV     AX,DS:2         ; get the storage size (in paragraphs)
       MOV     Q_BUFFTOP,AX    ; end of above-stack buffer
       SUB     AX,CX           ; get stack segment size (may be > 64k)
       TEST    AX,0F000H       ; see if > 64k
       JZ      SMALLSTG        ; branch if less than 64k
       MOV     AX,CX           ; stack segment
       ADD     AX,01000H       ; start of buffer heap (skip 16 bytes for luck)
       MOV     Q_BUFFBOT,AX    ; set start of buffer heap
       MOV     AX,0FFFH        ; stack is maximum size
SMALLSTG:
       CLI                     ; be paranoid
       MOV     DS,CX           ; set up data seg reg
       MOV     SS,CX           ; set up stack seg reg
       MOV     CX,4
       SAL     AX,CL           ; calculate stack size in bytes
       MOV     SP,AX           ; set stack pointer
       PUSH    DX              ; save PSP segment address
       MOV     DX,0
       PUSH    DX              ; zero offset to INT 20 in PSP
       MOV     BP,SP           ; set BP to point to stack frame
       MOV     Q_STKBASE,SP    ; save current frame address for _EXIT
       STI                     ; cease paranoia
       ASSUME  SS:DATASEG,ES:DATASEG,DS:DATASEG
       MOV     Q_STATTOP,SEG STACKSEG ; needed to find stack base
#endasm
  _heapbase = (_stattop - _dataseg)<<4; /* first heap byte */
  _heapfree = _heaptop = _heapbase; /* other heap pointers */
  _heapbase[0] = 0; /* size of first entry */
  _heapbase[1] = (-32768); /* mark entry 1st (prior = 0) & free */
  _recur = 0; /* no recursion yet */
  _parms(); /* process command line parameters */
  _iosetup(inname, outname); /* set up standard I/O */
  _mainrc=main(_argc, _argv); /* call the user's main program */
  _ioclnup(); /* close out I/O */
  _exit(_mainrc); /* return with code */
  }
 
 
/*
** exit with traceback & file clean-up
*/
abort(rcode) int rcode; {
  if(_recur) _exit(rcode);
  _recur = 1;
  if(rcode && stderr) {
    fputs("\nabort(", stderr);
    outdec(rcode, stderr);
    fputs(")\n", stderr);
    traceback(stderr);
    }
  _ioclnup();
  _exit(rcode);
  }
 
 
/*
** exit with file clean-up
*/
exit(rcode) int rcode; {
  if(_recur) _exit(rcode);
  _recur = 1;
  _ioclnup();
  _exit(rcode);
  }
 
 
/*
** exit with no clean-up
*/
_exit(rcode) int rcode; {
#asm
       MOV     SP,Q_STKBASE    ; restore stack frame address
; we might want to add code here to set the DOS return code from [BP]+4
DUMMYP PROC    FAR             ; trick assembler into generating far return
       RET                     ; RET FAR - return to DOS (via INT 20 in PSP)
DUMMYP ENDP
#endasm
  }
 
 
/*
** display the call sequence
*/
traceback(stream) int *stream; {
  int *stack, iar, *bp;
  fputs("\ndata segment address: ", stream);
  outhex(_dataseg, stream);
  fputs("\n", stream);
  stack = &stream;
  stack -= 2;
  while(ult(stack,_stkbase)) {
    bp = *stack++;
    iar = *stack;
    fputs("return address: ", stream);
    outhex(iar, stream);
    fputs("   stack frame address: ", stream);
    outhex(bp, stream);
    fputs("\n", stream);
    if(ult(bp,stack)) break;
    stack = bp;
    }
  }
 
 
/*
** unsigned less than
*/
static ult(a, b) int a, b; {
  if(a>=0)
    if(b>=0)
      return (a<b);
    else
      return 1;
  else
    if(b>=0)
      return 0;
    else
      return (a<b);
  }
 
 
/*
** output a decimal value
*/
static outdec(number, stream) int number, *stream; {
  int k, zs;
  char c;
  zs = 0;
  k = 10000;
  if(number<0) {
    number = (-number);
    fputc('-', stream);
    }
  while(k>=1) {
    c = number/k + '0';
    if((c!='0')|(k==1)|(zs)) {
      zs = 1;
      fputc(c, stream);
      }
    number = number%k;
    k = k/10;
    }
  }
 
 
/*
** output a hex value
*/
static outhex(number, stream) int number, *stream; {
  int k;
  char c;
  k = 4;
  while(k--) {
    c = (number>>12)&15;
    if(c<10) c = c + '0';
    else c = (c-10) + 'A';
    fputc(c, stream);
    number = number<<4;
    }
  }
 
 
/*
** process command line parms
*/
static _parms() {
  int count;
  char *ptr;
  inname = "kbd"; /* set default */
  outname = "scrn"; /* set default */
  count = getparm(_argl); /* get the argument list from the PSP */
  _argc = 1;
  ptr = _argl;
  _argv[0] = "main"; /* default for the first argument
                        (since DOS hides true file name) */
  while(1) {
    while(*ptr == ' ') ptr++;
    if(!*ptr) break;
    if(*ptr == '<') /* if input redirection */
      inname = ++ptr;
    else if(*ptr == '>') /* if output redirection */
      outname = ++ptr;
    else
      _argv[_argc++] = ptr;
    while(*ptr && *ptr != ' ') ++ptr;
    if(!*ptr) break;
    *ptr++ = 0;
    }
  _argv[_argc] = 0;
  }
 
/*
** set up I/O system
*/
_iosetup(inname, outname) char *inname, *outname; {
  _baseinit();
  _strminit(inname, outname, "con");
  }
 
 
/*
** close files and clean up I/O
*/
_ioclnup() {
  _strmclnup();
  _baseclnup();
  }
 
#include <streamio.h>
#include "streamio.c"
#include "sgtty.h"
#include "doscall.h"
#include "baseio1.c"
#include "baseio2.c"
#include "heap.c"
 
/*
** stack segment to satisfy linker, provide interrupt space while setting up,
** and to provide a way to identify the top of DATASEG
*/
#asm
STACKSEG SEGMENT STACK
       DB 256 DUP(?)
STACKSEG ENDS
#endasm
 

