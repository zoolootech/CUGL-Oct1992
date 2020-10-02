*	data	2
i	DS  	80

*	text
main	link	A6,#-4
	movem.l	A4/,-(A7)
	move	#0,D3
L0000	NOP
	cmp.l	#20,D3
	bge	L0001
	move.l	D3,D0
	add.l	D3,D0
	move.l	D3,D1
	asl.l	#2,D1
	move.l	D1,A0
	add.l	#i,A0
	move.l	D0,(A0)
	add.l	#1,D3
	bra	L0000
L0001	NOP
L0002	NOP
	movem.l	(A7)+,A4/
	unlk	A6
	rts
*	global	i
*	global	main

	END

