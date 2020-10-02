;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                                                                           *
;*  FILE:  CONIO.ASM                                                         *
;*                                                                           *
;*  This file contains library routines for putch(), getch(), getche(),      *
;*  ungetch(), and kbhit().  Although these routines are included in the     *
;*  C libraries that come with the Microsoft (R) C compiler, they are        *
;*  redirectable, which renders them useless.  (kbhit() is not redirectable, *
;*  but was added to increase functionality; specifically, to indicate what  *
;*  key was pressed, while still implementing Ctrl-C and Ctrl-S checking.    *
;*                                                                           *
;*  Since these five routines are used by cputs(), cgets(), cprintf(), and   *
;*  cscanf(), these higher-level functions are also useless, unless          *
;*  correctly working versions of putch(), getch(), getche(), ungetch(),     *
;*  and kbhit() are installed in the C libraries.                            *
;*                                                                           *
;*  (If you WANT your I/O to be redirectable, you can use putchar(),         *
;*   getchar(), ungetchar(), puts(), gets(), printf(), and scanf() ).        *
;*                                                                           *
;*  When assembling, use the -dRNEAR option if assembling for small or       *
;*  compact models, and the -dRFAR option if assembling for medium, large,   *
;*  or huge models.  If you are not using masm ver 4.0 or later, you will    *
;*  have to define either a RNEAR or RFAR label at the top of this program.  *
;*                                                                           *
;*  To assemble, use a command like this:                                    *
;*                                                                           *
;*     masm -mx -dRFAR conio;                                                *
;*                                                                           *
;*  To install these routines into the library, use a command like this:     *
;*                                                                           *
;*     lib llibc -putch -getch -getche -ungetch -kbhit +conio;               *
;*                                                                           *
;*                                                                           *
;*  Routines written by Jeff D. Pipkins at Quality Micro Systems, Inc.       *
;*  This file is hereby declared to be public domain.  Since it is public    *
;*  domain, QMS (R) is not liable for ANY damages caused by the use or       *
;*  abuse of this material.  Original release: 03/02/1987                    *
;*                                                                           *
;*  NOTE: These routines were written to work with the Microsoft (R)         *
;*        C compiler (ver 3.0 or later), but they can easily be adapted      *
;*        to work with other C compilers on any IBM (R) PC compatible.       *
;*                                                                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                                                                           *
;*  Revision 04/16/1987 (JDP):                                               *
;*     Added code for ungetch() with a one-character buffer.                 *
;*     See ungetch() and getch() routines for details.                       *
;*                                                                           *
;*  Revision 06/09/1987 (JDP):                                               *
;*     Modified getch() to translate lf to cr (it already translated         *
;*     cr to lf) so that a discriminating routine can tell the difference    *
;*     if necessary.                                                         *
;*     Added kbhit() in response to a quest by The C User's Group for        *
;*     a standard routine.                                                   *
;*     Added more documentation in the external .DOC file so that            *
;*     C programmers who have no assembly-language experience can read       *
;*     the specs without looking at the assembly code.                       *
;*                                                                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

IF1
 IFNDEF RNEAR
  IFNDEF RFAR
   %OUT FATAL ERROR: You MUST specify either -dRNEAR or -dRFAR on the command line!
   %OUT              (Note: Remember to use the -MX option as well!)
   .ERR
   ERROR EQU TRUE
  ENDIF
 ENDIF
 IFDEF RNEAR
  IFDEF RFAR
   %OUT FATAL ERROR: You CANNOT specify both -dRNEAR and -dRFAR on the command line!
   %OUT              (Note: Remember to use the -MX option as well!)
   .ERR
   ERROR EQU TRUE
  ENDIF
 ENDIF
 IFNDEF ERROR
  IFDEF RNEAR
   %OUT Generating object file for use with small or compact memory models...
  ELSE
   %OUT Generating object file for use with medium, large, or huge memory models...
  ENDIF
 ENDIF
ENDIF

_TEXT        Segment      Byte Public 'CODE'
             Assume       CS:_TEXT

Parm         Equ          [BP]
ungot_char   DW           0         ; Storage for one keystroke of lookahead
EOF          Equ          -1

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                                                                           *
;*  void putch(c)                                                            *
;*     int c;                                                                *
;*                                                                           *
;*  Displays the character passed on the screen, regardless of redirection.  *
;*  NOTE: Any line feeds displayed ('\n') will be preceeded by a cr ('\r').  *
;*                                                                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

             Public       _putch
IFDEF RFAR
_putch       Proc         Far
ELSE
_putch       Proc         Near
ENDIF

;*  Parameters

IFDEF RFAR
putch        Struc
             DW           ?         ; Single-word save area for BP
             DD           ?         ; Double-word return address
char         DW           ?         ; Character to put on the console
putch        EndS
ELSE
putch        Struc
             DW           ?         ; Single-word save area for BP
             DW           ?         ; Single-word return address
char         DW           ?         ; Character to put on the console
putch        EndS
ENDIF

;*  Output character to console using video BIOS

             Push         BP
             Mov          BP, SP
             Mov          CX, SI    ; Some BIOS clones destroy SI & DI
             Mov          DX, DI    ;   so save them first.
             Mov          AH, 0EH   ; TTY output function
             Mov          BL, 7     ; Attribute
             Mov          AL, Byte Ptr Parm[char]
             Cmp          AL, 0AH   ; Line feed?
             Je           CRLF      ; If so, translate to CRLF sequence
             Int          10H       ; Video BIOS
             Mov          SI, CX    ; Restore SI
             Mov          DI, DX    ;   and DI
             Pop          BP        ; BIOS destroys BP if scrolling occurs.
             Ret
CRLF:
             Mov          AH, 0BH   ; Check keyboard status
             Int          21H       ; Allow Ctrl-C or Ctrl-S
             Mov          AX, 0E0DH
             Int          10H
             Mov          AX, 0E0AH
             Int          10H
             Mov          SI, CX
             Mov          DI, DX
             Pop          BP
             Ret

_putch       EndP

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                                                                           *
;*  int getch();                                                             *
;*                                                                           *
;*  Get a keystroke from the keyboard regardless of redirection.  If the     *
;*  key has an ASCII value, it is returned as an integer.  If the key does   *
;*  not have an ASCII value (it is a function key, arrow key, etc.), then    *
;*  the value returned has the low byte zero and the high byte will hold     *
;*  the scan code for the key pressed.                                       *
;*                                                                           *
;*  Example for detecting special keys:                                      *
;*                                                                           *
;*  if ((i=getch()) <= 255) {                                                *
;*     spec_key = FALSE;                                                     *
;*     mychar = i;                                                           *
;*  }else{                                                                   *
;*     spec_key = TRUE;                                                      *
;*     scan_code = (unsigned) i >> 8;                                        *
;*  }                                                                        *
;*                                                                           *
;*  NOTE: Carriage returns are translated to line feeds, and vice versa.     *
;*        This is done so that a program can check for the ENTER key by      *
;*        comparing the character to '\n', as usual.                         *
;*                                                                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

             Public       _getch
IFDEF RFAR
_getch       Proc         Far
ELSE
_getch       Proc         Near
ENDIF
             Mov          AX, CS:[ungot_char] ; Is there a char in buffer?
             Or           AX, AX
             Jz           getch_kbd ; Get from keyboard if buffer empty
             Mov          CS:[ungot_char], Word Ptr 0  ; clear buffer
             Ret          ; ungot character is returned in AX
getch_kbd:
             ; Assume AX is 0 from above code
             Int          16H       ; Keyboard BIOS, read w/wait, no echo
             Or           AL, AL    ; Special key (no ASCII code) ?
             Jz           getch_end ; If so, return full AX
             Xor          AH, AH    ; Else, get rid of scan code

; Translate cr to lf so that a compare with '\n' will be valid.
; Translate lf to cr so that a discriminating routine can tell the
; difference if necessary.
; Note: the technique used below works because:
;       A xor (A xor D) = (A xor A) xor D = 0 xor D = D
;       D xor (A xor D) = D xor (D xor A) = (D xor D) xor A = 0 xor A = A

             Cmp          AL, 0DH   ; Carriage return?
             Je           getch_xlat
             Cmp          AL, 0AH   ; Line feed?
             Jne          getch_end
getch_xlat:
             Xor          AL, (0AH xor 0DH)   ; cr <=swap=> lf
getch_end:
             Mov          BX, AX    ; Save AX
             Mov          AH, 0BH   ; Check keyboard status
             Int          21H       ; Allow Ctrl-C or Ctrl-S
             Mov          AX, BX    ; Restore AX
             Ret

_getch       EndP

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                                                                           *
;*  int getche()                                                             *
;*                                                                           *
;*  {                                                                        *
;*     register unsigned i;                                                  *
;*                                                                           *
;*     if ((i = getch()) <= 255) {                                           *
;*        putch(i);                                                          *
;*     }                                                                     *
;*     return(i);                                                            *
;*  }                                                                        *
;*                                                                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

             Public       _getche
IFDEF RFAR
_getche      Proc         Far
ELSE
_getche      Proc         Near
ENDIF
             Call         _getch    ; Get character from keyboard
             Or           AL, AL    ; Special key (No ASCII code) ?
             Jz           getche_end ; If not, don't display it.
             Push         AX        ; Pass character as parameter
             Call         _putch    ; Call putch() to echo it.
             Pop          AX        ; Return character in AX
getche_end:
             Ret
_getche      EndP

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                                                                           *
;*  int ungetch(char)                                                        *
;*     int char;                                                             *
;*  { ... }                                                                  *
;*                                                                           *
;*  Pushes the character (char) back to the console, causing (char) to be    *
;*  the next character to be read by getch() or getche().  Returns (char)    *
;*  if successful, EOF (-1) if error.                                        *
;*                                                                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

             Public       _ungetch
IFDEF RFAR
_ungetch     Proc         Far
ELSE
_ungetch     Proc         Near
ENDIF
             Push         BP
             Mov          BP, SP

             Mov          AX, CS:[ungot_char] ; Make sure buffer is empty
             Or           AX, AX              ; Must be zero
             Jnz          ungetch_err         ; Go if buffer full

             Mov          AX, Parm[char]      ; Get character to push
             Mov          CS:[ungot_char], AX ; Push it into the buffer
             Jmp Short    ungetch_ok          ; Return value is in AX

ungetch_err:
             Mov          AX, EOF   ; Indicate EOF if too many ungetch()s.
ungetch_ok:
             Mov          SP, BP
             Pop          BP
             Ret
_ungetch     EndP

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                                                                           *
;*  int kbhit();                                                             *
;*                                                                           *
;*  This function is identical to getch() with the exception that if there   *
;*  is no keypress to be processed, kbhit() returns 0 instead of waiting     *
;*  for one.  (In contrast, getch() will wait for a keypress, and will       *
;*  never return 0.)  Please see getch() documentation for more info.        *
;*                                                                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

             Public       _kbhit
IFDEF RFAR
_kbhit       Proc         Far
ELSE
_kbhit       Proc         Near
ENDIF
             Mov          AX, CS:[ungot_char] ; Is there a char in buffer?
             Or           AX, AX
             Jz           kbhit_kbd ; Get from keyboard if buffer empty
             Mov          CS:[ungot_char], Word Ptr 0  ; clear buffer
             Ret          ; ungot character is returned in AX
kbhit_kbd:
             Mov          AH, 1     ; Tell BIOS not to wait on character
             Int          16H       ; Keyboard BIOS read, no wait, no echo
             Jnz          kbhit_hit ; Don't go if no keypress is available
             Xor          AX, AX    ; Return a 0
             Jmp Short    kbhit_end ; Check for break even if no keypress
kbhit_hit:
             Or           AL, AL    ; Special key (no ASCII code) ?
             Jz           kbhit_end ; If so, return full AX
             Xor          AH, AH    ; Else, get rid of scan code

; Translate cr to lf so that a compare with '\n' will be valid.
; Translate lf to cr so that a discriminating routine can tell the
; difference if necessary.
; Note: the technique used below works because:
;       A xor (A xor D) = (A xor A) xor D = 0 xor D = D
;       D xor (A xor D) = D xor (D xor A) = (D xor D) xor A = 0 xor A = A

             Cmp          AL, 0DH   ; Carriage return?
             Je           kbhit_xlat
             Cmp          AL, 0AH   ; Line feed?
             Jne          kbhit_end
kbhit_xlat:
             Xor          AL, (0AH xor 0DH)   ; cr <=swap=> lf
kbhit_end:
             Mov          BX, AX    ; Save AX
             Mov          AH, 0BH   ; Check keyboard status
             Int          21H       ; Allow Ctrl-C or Ctrl-S
             Mov          AX, BX    ; Restore AX
             Ret

_kbhit       EndP


_TEXT        EndS
             End

