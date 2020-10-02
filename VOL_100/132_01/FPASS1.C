/*
	Overlap tests for two objects in the same space.
	Entry	: A pointer to each scan list entry.
	Exit	: Null if no overlap, 1 if overlap
	Calls	: None
		Linkage to object definition list via scan pointers.
*/
	#ASM
XOFF	EQU	2*2
YOFF	EQU	3*2
OBJOFF	EQU	5*2
XMAXOF	EQU	2*2
YMAXOF	EQU	2*3
;overlap(scan,scan1)
xleft	rmb	2
xright	rmb	2
obj	rmb	2
xm	rmb	2
yupper	rmb	2
ylower	rmb	2
ym	rmb	2
overlap:
;int	scan[],scan1[];
;	Ascertain the left-most (x) and upper (y) objects.
;	xleft= scan[X];
	ldx	4,s	; x -> scan
	ldy	2,s	; y -> scan1
	ldd	XOFF,x
	std	xleft
;	xright	= scan1[X];	/* assume this is correct */
	ldd	XOFF,y
	std	xright
;	obj	= scan[OBJPTR];
	ldd	OBJOFF,x
	std	obj
;	if(xleft > xright) 	{ /* if assumption invalid,switch */
	ldd	xleft
	cmpd	xright	; xleft - xright
	ble	ovl1	; if xleft <= xright
		ldd	xright
		ldu	xleft
		stu	xright
		std	xleft
;		obj	= scan1[OBJPTR]
		ldd	OBJOFF,Y
		std	obj
;	}
ovl1:
;	xm= 255&(PIXBYT*obj[XMAX]);
	pshs	x
	ldx	obj
	ldd	XMAXOF,x
	clra
	lslb
	lslb		; multiply offset by 8
	lslb
	std	xm
	puls	x
;	yupper	= scan[Y];	/* same procedure for y axis */
	ldd	YOFF,x
	std	yupper
;	ylower  = scan1[Y];
	ldd	YOFF,y
	std	ylower
;	obj 	= scan[OBJPTR];
	ldd	OBJOFF,x
	std	obj
;	if(yupper > scan1[Y]) {
	ldd	yupper
	cmpd	ylower	; yupper - ylower
	ble	ovl2
		ldu	ylower
		ldd	yupper
		stu	yupper
		std	ylower
;		obj	= scan1[OBJPTR];
		ldd	OBJOFF,y
		std	obj
ovl2:
;	ym	= 255&obj[YMAX];
		ldu	obj
		ldd	YMAXOF,u
		clra
		std	ym
;	if((xleft + xm)>= xright)
		ldd	xleft
		addd	xm
		cmpd	xright ; xleft - xright
		bls	ovl3
;		if((yupper + ym)>= ylower)
			ldd	yupper
			addd	ym
			cmpd	ylower	; (yupper + ym) - ylower
			bls	ovl3
;			return 1;
			ldd	#1
			rts
;	return 0;
ovl3:
	ldd	#0
	rts
	#ENDASM
 

s
	#ENDASM
}
 
¡ÊñzæÊí¡Êñ>@°Gzæ¡°GG·ÊEõzæÊ>+2R3*a8e3~#þÂëÃ%!e3>¾#Â 