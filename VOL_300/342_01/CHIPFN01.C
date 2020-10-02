/*-
 *  ----------------------------------------------------------------------
 *  File        :   CHIPFN01.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00            February 1991
 *  Language    :   Microsoft Quick C   Version 2.0
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   80X86 OUT Instruction Function
 *  ----------------------------------------------------------------------
 */

void chp_portwt (int d_port, unsigned char d_byte);

/*- CHIP : Byte Put --------------------------**
 *  Write a byte to one of the 80X86 ports.
 *  Duplicates the library function outp()
 */
void chp_portwt (int d_port, unsigned char d_byte)
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
 *  END CHIPFN01.C Source File
 *  ----------------------------------------------------------------------
 */
