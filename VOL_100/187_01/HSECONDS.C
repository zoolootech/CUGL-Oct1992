/*@*******************************************************/
/*@                                                      */
/*@ hseconds - return the current time-of-day in         */
/*@        hundredths of seconds since midnight.         */
/*@                                                      */
/*@   Usage:     hseconds();                             */
/*@                                                      */
/*@   Returns a long value representing the number       */
/*@      of seconds since midnight (0 <= x <= 8,640,000) */
/*@                                                      */
/*@*******************************************************/


int hour, min, sec, hund;

long hseconds()
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

	return ((long)hour * 360000 + (long)min * 6000
			 + (long)sec * 100 + (long)hund);

}
