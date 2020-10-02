/* MSC.H header file for porting Turbo C v2 to Microsoft C v5
 *
 *	This header file 'irons out' some of the minor differences.
 *
 */


/* memory model indicator -- at least one must be defined,
 * default is medium (for Quick C)
 */
#ifndef __SMALL__
	#ifndef __LARGE__
		#ifndef __MEDIUM__
			#ifndef __COMPACT__
				#ifndef __HUGE__
					#ifndef __TINY__
						#define __MEDIUM__ 1
					#endif
				#endif
			#endif
		#endif
	#endif
#endif		/* end memory model indicator */


/* In-Line interrupts - TurboC can, microsoft C can't
 *
 *
 *  To use this technique, ALWAYS load the registers in inverse
 *  alphabetical order (DX first, then CX, BX and AX last)
 *  and don't try to do any complex computations in the lines
 *  that access the registers.
 *	 WRONG, for 3 reasons:
 *		_AH = 1;
 *		_DX = value1 + 3*value2;	===> TurboC uses AX
 *		geninterrupt ( variable_int_number );  ditto
 *	RIGHT, but useless:
 *		value3 = value1 + 3*value2;
 *		if ( int_number = 0x01 )
 *			{
 *			_DX = value3;
 *			_AX = 1;
 *			geninterrupt ( 0x01 );
 *			}
 *		else	{
 *			_DX = value3;
 *			_AX = 1;
 *			geninterrupt ( 0x02 );
 *			}
 *
 */
		#define PSEUDOREGS union REGS regs; struct SREGS sregs;
		#define _AX	regs.x.ax
		#define _AL	regs.h.al
		#define _AH	regs.h.ah
		#define _BX	regs.x.bx
		#define _BL	regs.h.bl
		#define _BH	regs.h.bh
		#define _CX	regs.x.cx
		#define _CL	regs.h.cl
		#define _CH	regs.h.ch
		#define _DX	regs.x.dx
		#define _DL	regs.h.dl
		#define _DH	regs.h.dh
 
 		#define _ES	sregs.es
		#define _CS	sregs.cs
		#define _SS	sregs.ss
		#define _DS	sregs.ds
		
		/* Microsoft C can't access the flags.
		 */
		#undef _FLAGS

		#define INTERRUPT(intno)	int86((intno), &regs, &regs)

		#define INTERRUPTX(intno) int86x ( (intno), &regs, &regs, &sregs )
		
/* interrupt handlers
 */
#define enable() 		_enable()
#define disable() 		_disable()
#define setvect(x,y)   	_dos_setvect((x),(y))
#define getvect(x)   	_dos_getvect((x))

/* In Line outport()
 */
#define outport(x,y)  outp((x),(y))
#define inport(x)     inp ((x))



/* Memory allocation.
 * uses different names from TurboC
 */
	#define farmalloc(nb)    halloc(nb, 1)
	#define farfree(x)		 hfree(x)



/* make a far ptr given a seg & offset
 */
#define MK_FP(s,o) (void far *)(((unsigned long)(s)<<16)|((unsigned)(o)))




/* simple graphics calls
 */
#define line(a,b,c,d)	\
			_moveto((a),(b)), _lineto((c),(d)) 
#define lineto(a,b)  _lineto ((a), (b))
#define setviewport(a,b,c,d,x)  \
	_setviewport((a),(b),(c),(d)), \
	if (x) {_setcliprgn ((a),(b),(c),(d)) }
#define putpixel(a,b,c)  _setcolor((c)), _setpixel ((a),(b))
#define getpixel(a,b)    _getpixel ((a),(b))
#define setcolor(x)      _setcolor((x))


