/*-
 *  ----------------------------------------------------------------------
 *  File        :   CHIPFN02.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00            February 1991
 *  Language    :   Microsoft Quick C   Version 2.0
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   80X86 IN Instruction Function
 *  ----------------------------------------------------------------------
 */

void chp_portrd (int d_port, unsigned char *d_byte);

/*- CHIP : Byte Get --------------------------**
 *  Read a byte from one of the 80X86 ports.
 *  Duplicates the library function inp()
 */
void chp_portrd (int d_port, unsigned char *d_byte)
	{
	unsigned char   t_byte = 0; /* temporary byte   */

	/*  set port address
	 *  get data into AL
	 *  store data into temporary variable
	 *  Note: Use temporary variable because ASM has no idea
	 *  what a pointer to an unsigned char is!
	 */
	_asm    {
		PUSH    AX
		PUSH    DX

		MOV     DX, d_port
		IN      AL, DX          ; get byte into AL
		MOV     t_byte, AL      ; transfer data to C temporary

		POP     DX
		POP     AX
		}

	*d_byte = t_byte;   /* transfer data back to C  */
	}

/*-
 *  ----------------------------------------------------------------------
 *  END CHIPFN02.C Source File
 *  ----------------------------------------------------------------------
 */
