/*@*******************************************************/
/*@                                                      */
/*@ tseconds - return the current time-of-day in         */
/*@        tenths of seconds since midnight.             */
/*@                                                      */
/*@   Usage:     tseconds();                             */
/*@                                                      */
/*@   Returns a long value representing the number       */
/*@      of seconds since midnight (0 <= x <= 864,000)   */
/*@                                                      */
/*@*******************************************************/

unsigned hour, min, sec, hund;

long tseconds()
{
	
#asm
	PUSH	AX
	PUSH	CX
	PUSH	DX
	MOV		AX,2C00H
	INT		21H
	MOV		AH,00
	MOV		AL,CH
	MOV		WORD hour_,AX
	MOV		AL,CL
	MOV		WORD min_,AX
	MOV		AL,DH
	MOV		WORD sec_,AX
	MOV		AL,DL
	MOV		WORD hund_,AX
	POP		DX
	POP		CX
	POP		AX
#endasm

	return ((long)hour * 36000 + (long)min * 600
			 + (long)sec * 10 + (long)hund / 10);

}
