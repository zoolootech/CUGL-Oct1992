/*@*******************************************************/
/*@                                                      */
/*@ seconds - return the current time-of-day in          */
/*@        hundredths of seconds since midnight.         */
/*@                                                      */
/*@   Usage:     seconds();                              */
/*@                                                      */
/*@   Returns a long value representing the number       */
/*@      of seconds since midnight (0 <= x <= 86,400)    */
/*@                                                      */
/*@*******************************************************/


int hour, min, sec;

long seconds()
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
	POP		DX
	POP		CX
	POP		AX
#endasm

	return ((long)hour * 3600 + (long)min * 60
			 + (long)sec);

}
