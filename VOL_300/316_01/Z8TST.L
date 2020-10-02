Z8 Assembler: h.-g. willers	page 1		Fri Jan 12 22:08:40 1990


                                      1 	.hlist
                                      2 /
             0099                     3 very_long_identifier = 0x99
                                      4 /
                                      5 /----   comment
                                      6 /
             000a                     7 w16     =       w10
                                      8 /
             0000  ef                 9 	ccf
             0001  8f                10 	di
             0002  9f                11 	ei
             0003  ff                12 	nop
             0004  cf                13 	rcf
             0005  af                14 	ret
                                     15 /
             0006  31  40            16 	srp     $0x40
a            0008                    17 	srp     $8       / should generate error message
                                     18 /
             0008  c2  96            19 here:   movp    (w6),w9
             000a  a0  e6            20 	incw    w6
             000c  a2  93            21 	cmp     w3,w9
             000e  6b  03            22 	beq     bye
             0010  8a  f6            23 	sob     w8,here
             0012  df                24 	scf
                                     25 /
             0013  ff                26 bye:    nop
             0014  aa  f2            27 	sob     w10,here
             0016  aa  f0            28 	sob     w16,here
                                     29 /
             0018  d6  00  13        30 	call    bye
             001b  d4  00            31 	call    (r0)
             001d  d4  e0            32 	call    (w0)
e            001f  d4  01            33 	call    (r1)            / must give error message
                                     34 /
             0021  fe                35 	inc     w15
             0022  b0  00            36 	clr     r0
             0024  b0  e1            37 	clr     w1
             0026  b1  02            38 	clr     (r2)
             0028  b1  e2            39 	clr     (w2)
                                     40 /
             002a  1d  00  08        41 	jlt     here
e            002d  30  01            42 	jmp     (r1)            / must give error message
             002f  30  e0            43 	jmp     (w0)
                                     44 /
             0031  02  10            45 	add     w0,w1
             0033  04  64  65        46 	add     r100,r101
             0036  05  01  00        47 	add     (r1),r0
             0039  03  00            48 	add     (w0),w0
             003b  06  e0  01        49 	add     $1,w0
             003e  07  00  01        50 	add     $1,(r0)
             0041  17  14  01        51 	adc     $1,(r20)
             0044  16  00  01        52 	adc     $1,r0
a            0047                    53 	add     (w0),(w1)       / truly not implemented
             0047  06  32  01        54 	add     $1,r50
                                     55 /
             004a  0e                56 	inc     w0
             004b  00  64            57 	dec     r100
Z8 Assembler: h.-g. willers	page 2		Fri Jan 12 22:08:40 1990


             004d  21  e1            58 	inc     (w1)
             004f  01  64            59 	dec     (r100)
                                     60 /
             0051  c3  20            61 	movfpi  (w0),(w2)
             0053  d3  02            62 	movtpi  (w2),(w0)
             0055  82  30            63 	movd    (w0),w3
                                     64 /
             0057  70  e1            65 	push    w1
             0059  50  05            66 	pop     r5
             005b  71  e4            67 	push    (w4)
             005d  71  11            68 	push    (r17)
                                     69 /
             005f  46  f1  0c        70 	or      $0x0c,r241
             0062  8f                71 	di
             0063  46  fb  20        72 	or      $0x20,r251
             0066  9f                73 	ei
             0067  af                74 	ret
             0068  bf                75 	reti
                                     76 /
             0069  0c  5f            77 	mov     $0x5f,w0
             006b  18  64            78 	mov     r100,w1
             006d  19  64            79 	mov     w1,r100
             006f  48  e0            80 	mov     w0,w4
             0071  e6  f3  93        81 	mov     $0x93,r243
             0074  e4  64  66        82 	mov     r100,r102
             0077  c7  10  64        83 	mov     r100(w0),w1
                                     84 /
q            007a                    85 	mov	2(w0),w2	/ ERROR!
Z8 Assembler: h.-g. willers	page 3		Fri Jan 12 22:08:40 1990


Symboltable dump

bye                                  0013
very_long_identifier                 0099
here                                 0008
w16                                  000a



 symboltable-size is 503 ; 94 entries used
