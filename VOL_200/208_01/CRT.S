*
* HEADER:	CUG208;
* TITLE:		crt
* VERSION:	1.00
* DESCRIPTION:	"C program starter";
*
* KEYWORDS:	linkage;
* SYSTEM:		CP/M68K, V1.2;
* FILENAME:	e/crt.s
* WARNINGS:	"as regards side-effects, study the source code
*		 non-owner of CP/M68K is not expected to run this file.
* AUTHORS:	sub-contractors of DRI.
* CODER:		Yoshimasa Tsuji( this file should be withdrawn when
*		DRI resumed consumer support).
* COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*	----	---	---	---	---	---	---
*
*	C runtime starter -- postprocesses command line and sets up
*				standard IO's.
*		s.s, main.c, xmain.c hacked
*				by Y. Tsuji 8/9/1986
*	You are advised to eliminate main.o,xmain.o,xmainnw.o from "libc.a".
*	You cannot undefine _nowildcard now.
*BUG:	command line handling of xmain.c is still buggy regarding
*	\" ,\', " ' "
*
*FIX:	fixing CCP is more productive.
*
*	Will you please send me your code?
*		Dr Yoshimasa Tsuji
*		Kasuga 2-20-12-114,
*		Bunkyo-ku,
*		Tokyo 112,
*		JAPAN
	.text
	.globl  __start,_brk,___BDOS,__sovf,__exit
* blkfill() and index() are found in libc.a too.
	.globl _blkfill,_strchr,_index
* _start is the start of executable file
__start:
	moveq	#0,d6			* don't know why
	movea.l  4(a7),a0
	move.l  a0,__base		* base page address
	movea.l  $18(a0),a1		* address of bss
  	adda.l  $1C(a0),a1		* bss length
* _break is the end address of the current executable file
* the linker sets these data always even
	move.l  a1,__break
  	lea  $80(a0),a2
	clr.w	d0
  	move.b  (a2)+,d0
  	move.w  d0,-(a7)
  	move.l  a2,-(a7)
  	suba.l  a6,a6			* don't know why
  	bsr  __main			* _main(cmdline,count)
  	bra  __exit
_brk:
* brk(addr)
* sets __break=address, and returns 0 or -1
* BUG: the original did not check odd address
* This is very dangerous because the caller can destroy itself.
* This function is used because accessing between _break and stack pointer
* is illegal.
*
  	movea.l  4(a7),a0
  	move.l  a0,d0
  	lea  $100(a0),a0
  	cmpa.l  a0,a7
  	bhi  brkok
  	moveq.l  #$FF,d0
  	bra	brkdone
brkok:
	addq.l	#1,d0
	andi.w	#$fffe,d0
  	move.l  d0,__break
  	moveq	#0,d0
brkdone:
  	rts
___BDOS:
* __BDOS(arg,fn)
* long arg;
* int fn; /* function number */
  	move.w  4(a7),d0
  	move.l  6(a7),d1
  	trap  #2
  	move.w	d0,___cpmrv
* now you can know what value bdos returned by __cpmrv
  	cmpa.l  __break,a7
  	bhi  noovf
__sovf:
  	move.w  #9,d0
  	move.l  #ovf,d1
  	trap  #2
__exit:
*			the value returned is 4(USP)
  	move.w  #0,d0
  	trap  #2
noovf:
  	rts
_blkfill:
* fixed so that it accepts unsigned count
* you can use malloc(unsigned) now.
* blkfill(buff,c,count)
* char *buff,c;
* unsigned int count;
* fills buff with c count times
  	movea.l  4(a7),a0
  	move.w  8(a7),d1
	move.w  10(a7),d0
	beq	filldone
  	subq.w  #1,d0
fillit:
  	move.b  d1,(a0)+
  	dbf  d0,fillit
filldone:
  	moveq	#0,d0
  	rts
_index:
_strchr:
  	movea.l  4(a7),a0
  	move.w  8(a7),d0
xindex:
  	cmp.b	(a0),d0
	beq	done
	tst.b  (a0)+
  	bne	xindex
  	suba.l	a0,a0
done:	move.l  a0,d0
	rts
*
	.data
	.globl ___pname
	.globl ___tname
	.globl ___lname
	.globl ___aname
	.globl ___xeof
ovf:
	dc.b	'Stack Overflow$'
___pname:
	dc.b 'C runtime',0
___tname:
	dc.b 'CON:',0
___lname:
	dc.b 'LST:',0
* aux was not supported by DRI
___aname:
	dc.b 'AUX:',0
* changing end of file char does not affect any subroutine that
* checks EOF of ASCII files.
___xeof:
	dc.b $1a
	.bss
	.globl __base
	.globl __break
	.globl ___cpmrv
__base:   ds.l	1
__break:  ds.l	1
___cpmrv: ds.w	1
*
**********************
* main.c
* _main(com,len)
* char *com;
* int len;
* DESCRIPTION
*{ _chinit(),open(___tname,0);open(___tname,1);open(___tname,1);
*  *(com+len)=0;tolower(com);
*  __main(com,len);
*}
*
		.text
		.globl _open,__chinit
__main:
* initialise file structure
		jsr	__chinit
* open stdin as tty input
		clr.w	-(a7)
		move.l	#___tname,-(a7)
		jsr	_open
	        addq.w  #4,a7		don't forget a7+2
* open stdout as tty output
		move.w	#1,(a7)
		move.l	#___tname,-(a7)
		jsr	_open
		addq.w	#4,a7
* open stderr as tty input/output.
* DRI did not allow you to read stderr.
		move.w	#2,(a7)		r/w stderr
		move.l	#___tname,-(a7)
		jsr	_open
	        addq.w  #6,a7		a7 is now properly restored
        	movea.l  4(a7),a0		com is long
	        move.w   8(a7),d0		len is word
        	clr.b  0(a0,d0.w)		clr
* tolower command line
* but convert \n to N
* you can pass upper case letters from the command line now.
		movea.l	a0,a1
L1:
		move.b	(a0),d1
		beq	L2
		cmpi.b	#'\',d1
		bne	L3
		move.b	1(a0),d0
		bsr.b	alpha
		tst	d0
		beq.b	L4
		subq.w	#1,8(a7)	len--
		addq.w	#1,a0
		move.b	(a0),d1
		bra.b	L4
L3:
		move.b	d1,d0
		bsr.b	alpha
		tst	d0
		beq	L4
		ori.b	#%00100000,d1
L4:
		move.b	d1,(a1)+
		addq.w	#1,a0
		bra	L1
alpha:
		cmpi.b	#'A',d0
		blt	L5
		cmpi.b	#'Z',d0
		bls	L6
L5:		clr	d0
L6:		rts
*
L2:
		clr.b	(a1)
*		jmp	___main
******
* xmain.c
*
	.text
*	.globl  _main
	.globl _strcpy,_creat,___open,__salloc
	.globl _lseek,_close,_strlen
* ___main:
* com = 8, len = 12, i = d7, s= a5, p = a4, c = d6, pfd = fffc, tmpbuf = ffde
        link  a6,#$FFDE
        movem.l  d5-d7/a3-a5,-(a7)
	lea	$ffde(a6),a3
 	move.l	__break,d0
	move.l  d0,argv
        move.l  d0,argn
        clr.w  argc
        move.l  #___pname,(a7)
        bsr	_addargv
        movea.l  8(a6),a5
        bra  L7b
L10b:
        addq.w  #1,a5
L11b:
        move.b (a5),d0
        beq  L5b
    	cmpi.b	#' ',d0
	beq	L10b
	cmpi.b	#9,d0
	beq	L10b
        cmpi.b  #$22,(a5)		* double quote
        beq  L10001b
        cmpi.b  #$27,(a5)		* single quote
        bne  L12b
L10001b:
        move.b  (a5),d6
        ext.w  d6
        move.w  d6,(a7)
        move.l  a5,-(a7)
        addq.l  #1,(a7)
        jsr  _strchr
        addq.w  #4,a7
        movea.l  d0,a4
        tst.l  d0
        bne  L13b
        move.l  #L14b,(a7)
        move.l  a5,-(a7)
        bra  __err		* unmatched quote
L13b:
	clr.b	(a4)
        sub.l  a5,d0
        move.w  d0,d7
        addq.w  #1,d7
        move.l  a5,(a7)
        addq.l  #1,(a7)
        bsr  _addargv
        bra  L6b
L12b:
        clr.w  d7
        bra  L18b
L17b:
        addq.w  #1,d7
L18b:
	move.b	0(a5,d7.w),d0
        beq  L20b
        cmpi.b	#' ',d0
	beq	L16b
	cmpi.b	#9,d0
	bne	L17b
L16b:
        clr.b	0(a5,d7.w)
        addq.w  #1,d7
L20b:
        move.b  (a5),d0
        ext.w  d0
        cmp.b  #'>',d0
        beq  L26b
        cmp.b  #'<',d0
        bne  L33b
* standard input
L23b:
        clr.w  (a7)
        jsr  _close
        clr.w  (a7)
        move.l  a5,-(a7)
        addq.l  #1,(a7)
        jsr  _open
        addq.w  #4,a7
        tst.w  d0
        beq  L6b
        move.l  a5,(a7)
        addq.l  #1,(a7)
        move.l  #L25b,-(a7)
        bra  __err
* standard output
L26b:
        move.w  #1,(a7)
        jsr  _close
        cmpi.b  #'>',1(a5)
        bne  L27b
* appending output
        move.w  #1,(a7)
        move.l  a5,-(a7)
        addq.l  #2,(a7)
        jsr  _open
        addq.w  #4,a7
        cmpi.w  #1,d0			* fd for std out
        bne  L10003b		 	* must create then***
        move.w  #2,(a7)
        clr.l  -(a7)
        move.w  #1,-(a7)
        jsr  _lseek			* append at end
        addq.w  #6,a7
        addq.l #1,d0
        bne  L6b
        move.l  a5,(a7)
        addq.l  #1,(a7)
        move.l  #L29b,-(a7)
        bra  __err
L10003b:
	subq.w	#1,d7
	addq.w	#1,a5
* delete old and create new outputfile
L27b:
        clr.w  (a7)
        move.l  a5,-(a7)
        addq.l  #1,(a7)
        jsr  _creat
        addq.w  #4,a7
        cmp.w  #1,d0
        beq  L6b
        move.l  a5,(a7)
        addq.l  #1,(a7)
        move.l  #L32b,-(a7)
        bra  __err
L33b:
        move.w  #$3F,(a7)		* '?'
        move.l  a5,-(a7)
        jsr  _strchr
        addq.w  #4,a7
        tst.l  d0
        bne  L10004b
        move.w  #$2A,(a7)		* '*'
        move.l  a5,-(a7)
        jsr  _strchr
        addq.w  #4,a7
        tst.l  d0
        beq  L34b
* contained ? or *
L10004b:
        movea.l  #__fds+540,a0		* 180*3 (21C)
        move.l  a0,$FFFC(a6)
	lea	$34(a0),a0
        move.l  a0,d1
        move.w  #$1a,d0
        trap	#2			* set dma
        move.w  #17,(a7)		search for first
        move.l  a5,-(a7)		filename
        move.w  #3,-(a7)		file descriptor
        jsr  ___open			this does not really open
        addq.w  #6,a7
        move.b  d0,d6
        cmp.b  #$FF,d6
        bne  L38b
        move.l  #L36b,(a7)
        move.l  a5,-(a7)
        bra  __err
L38b:
	move.l  a3,(a7)
        ext.w  d6
        move.w  d6,-(a7)
        move.l  $FFFC(a6),-(a7)
        bsr  __toasc
        addq.w  #6,a7
	move.l  a3,(a7)
        jsr  _strlen
        move.w  d0,(a7)
        addq.w  #1,(a7)
        jsr  __salloc		* sp lowered by d0+1 bytes with care
        movea.l  d0,a4
        move.l  a3,(a7)
        move.l  a4,-(a7)
        jsr  _strcpy
        addq.w  #4,a7
        move.l  a4,(a7)
        bsr  _addargv
        move.w  #$12,(a7)
        move.l  a5,-(a7)
        move.w  #$3,-(a7)
        jsr  ___open
        addq.w  #6,a7
        move.b  d0,d6
        cmp.b  #$FF,d0
        bne  L38b
        bra  L6b
L34b:
        move.l  a5,(a7)
        bsr  _addargv
L6b:
        adda.w  d7,a5
L7b:
        tst.b  (a5)
        bne  L11b
L5b:
        clr.l  (a7)
        bsr  _addargv
        subq.w  #1,argc
        move.l  argn,(a7)
        bsr  _brk
        cmp.w  #$FFFF,d0
        bne  L41b
        move.l  #L43b,(a7)
        move.l  #L42b,-(a7)
        bra  __err
L41b:
        clr.l  (a7)
        move.l  argv,-(a7)
        move.w  argc,-(a7)
        jsr  _main			execute user program
        addq.w  #6,a7
        move.w  d0,(a7)			return value to shell
        jsr  _exit			never returns
__err:
* s1 = 8 s2 = 12, buf = ff80

   	movea.l	4(a7),a0
	bsr	L45b
	movea.l	8(a7),a0
	bsr	L45b
	move.w	#$FFFF,(a7)
	jmp	_exit
L45b:
	move.b	(a0)+,d1
	beq	L44b
	moveq	#2,d0
	trap	#2
	bra	L45b
L44b:    rts
*
_addargv:
* ptr = a0    store argv to address pointed by argn, and increment argn
        movea.l  argn,a0
        move.l  4(a7),(a0)
        addq.l  #4,argn
        addq.w  #1,argc
        rts
__toasc:
* p = a2, c = d0, buf = a1, f = a0, i = d1
        link  a6,#-4
        movea.l  8(a6),a2
        move.b  13(a6),d0		* c
        movea.l  14(a6),a1
        clr.b  (a1)
        clr.w  d1
        lea  $34(a2),a0
        ext.w  d0			* c
        asl.w  #5,d0
        adda.w  d0,a0
        move.b  2(a2),d0
        beq  L51b
        ext.w  d0
        subq.w  #1,d0			* offset by one
        cmpi.w  #10,d0
        blt  L52b
        move.b  #'1',(a1)+
	subi.w	#10,d0
L52b:
        add.b  #'0',d0
        move.b  d0,(a1)+		* user number
        move.b  #$3A,(a1)+ 		* colon
        move.w  #1,d1
L51b:
        tst.b  $10(a2)			* drive name
        beq  L53b
        tst.w  d1
        beq  L54b
        subq.w  #1,a1			* erase colon
L54b:
        move.b  $10(a2),d0
        addi.b  #$60,d0
        move.b  d0,(a1)+
        move.b  #$3A,(a1)+
* tolower
L53b:	addq.w	#1,a0
        move.w  #7,d1
L58b:	bsr	L55b
	dbra	d1,L58b
        move.b  #$2E,(a1)+
        move.w  #2,d1
L63b:	bsr	L55b
	dbra	d1,L63b
        clr.b  (a1)+
        unlk  a6
        rts
*
*** tolower
L55b:	move.b	(a3)+,d0
	cmpi.b	#$20,d0
	beq	L56b
	cmpi.b	#'A',d0
	bcs	L57b
	cmpi.b	#'Z'+1,d0
	bcc	L57b
	ori.b	#%00100000,d0
L57b:	move.b	d0,(a4)+
L56b:	rts
	.data
L14b:	dc.b ': unmatched quote',0
L25b:	dc.b 'Cannot open ',0
L29b:	dc.b 'Cannot append ',0
L32b:	dc.b 'Cannot creat ',0
L36b:	dc.b ': No match',0
L42b:	dc.b 'Stack Overflow',0
L43b:	dc.b	0
*
	.bss
argc: 	ds.w	1
argv: 	ds.l	1		* pointer to the first address of argv
argn: 	ds.l	1		* pointer to the next argv
		.end

