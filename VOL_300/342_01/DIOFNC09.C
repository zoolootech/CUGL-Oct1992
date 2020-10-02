/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOFNC09.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00            February 1991
 *  Language    :   Microsoft Quick C   Version 2.0
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *                  80X86 OUT Instruction Function
 *  ----------------------------------------------------------------------
 *  WARNING: Inline Assembly Language Here!
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Change int to short.
 *  070490 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define     DIOFNC09_C_DEFINED
#undef      DIOFNC09_C_DEFINED

void dio_bput ( short, unsigned char);

/*- DIO : Byte Put ---------------------------**
 *  Write a byte to one of the 80X86 ports.
 *  Duplicates the library function outp()
 */
void dio_bput (short d_port, unsigned char d_byte)
	{
	_asm    {
		PUSH    AX
		PUSH    DX

		MOV     DX, d_port
		MOV     AL, d_byte
		OUT     DX, AL

		POP     DX
		POP     AX
		}
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOFNC09.C Source File
 *  ----------------------------------------------------------------------
 */
