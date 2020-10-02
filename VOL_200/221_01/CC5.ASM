;small-c V2.3
 EXT ccdiv,ccmult
 EXT fopen,fclose,getc,putc,putchar,getchar,rewind,gets
 EXT exit
 EXT CCSWITCH
 EXT CCPOLL
 EXT isalpha,isupper,islower,isdigit,isspace
 EXT toupper,tolower
 EXT strlen,reverse,left,strcmp,strcpy
 EXT abs,itoa,atoi,printf
 EXT add
 EXT and
 EXT asl
 EXT asr
 EXT blanks
 EXT ch
 EXT clearsta
 EXT div
 EXT doublere
 EXT eq
 EXT ge
 EXT getlabel
 EXT gt
 EXT heir7
 EXT immed
 EXT inbyte
 EXT jump
 EXT le
 EXT loadsec
 EXT lt
 EXT match
 EXT mod
 EXT mult
 EXT needbrac
 EXT needlval
 EXT ne
 EXT nl
 EXT or
 EXT outdec
 EXT outstr
 EXT postlabe
 EXT pop
 EXT push
 EXT pushsec
 EXT rvalue
 EXT setstage
 EXT store
 EXT streq
 EXT sub
 EXT testjump
 EXT testnoop
 EXT testtrue
 EXT uge
 EXT ugt
 EXT ule
 EXT ult
 EXT xor
 EXT lptr
 EXT cptr
 EXT line
 NAME doexpres
 GLOBAL doexpres
doexpres
 LEAS -4,S
cc2
 LDD  #1
 CMPD #0
 LBEQ cc3
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR setstage
 LEAS 4,S
 JSR expressi
 LDD  2,S
 PSHS D
 LDD  2,S
 PSHS D
 JSR clearsta
 LEAS 4,S
 JSR ch
 PSHS D
 LDD  #44
 CMPD ,S++
 LBEQ cc4
 JMP cc3
cc4
 JSR inbyte
 JMP cc2
cc3
 LEAS 4,S
 RTS
 GLOBAL expressi
expressi
 LEAS -16,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc5
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc5
 LEAS 16,S
 RTS
 GLOBAL heir1
heir1
 LEAS -20,S
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  24,S
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #1
 STD  [,S++]
 STD  [,S++]
 LEAY 16,S
 TFR  Y,D
 PSHS D
 LDD  24,S
 PSHS D
 JSR heir1a
 LEAS 2,S
 STD  [,S++]
 JSR blanks
 LEAY 18,S
 TFR  Y,D
 PSHS D
 LDD  #line
 ADDD lptr
 STD  [,S++]
 LDD  18,S
 PSHS D
 LDD  #cc1+0
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc6
 LDD  18,S
 PSHS D
 LDD  #cc1+3
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc7
 LDD  18,S
 PSHS D
 LDD  #cc1+6
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc8
 LDD  18,S
 PSHS D
 LDD  #cc1+9
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc9
 LDD  18,S
 PSHS D
 LDD  #cc1+12
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc10
 LDD  18,S
 PSHS D
 LDD  #cc1+15
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc11
 LDD  18,S
 PSHS D
 LDD  #cc1+19
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc12
 LDD  18,S
 PSHS D
 LDD  #cc1+23
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc13
 LDD  18,S
 PSHS D
 LDD  #cc1+26
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc14
 LDD  18,S
 PSHS D
 LDD  #cc1+29
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc15
 JSR ch
 PSHS D
 LDD  #61
 CMPD ,S++
 LBEQ cc16
 LDD  16,S
 LEAS 20,S
 RTS
cc16
cc15
cc14
cc13
cc12
cc11
cc10
cc9
cc8
cc7
cc6
 LDD  16,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc17
 JSR needlval
 LDD  #0
 LEAS 20,S
 RTS
cc17
 LDD  22,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc18
 JSR push
 JMP cc19
cc18
 JSR ch
 PSHS D
 LDD  #61
 CMPD ,S++
 LBEQ cc20
 JSR immed
 LDD  22,S
 PSHS D
 LDD  [,S++]
 ADDD #0
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 JSR push
cc20
cc19
 LDD  #cc1+32
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc21
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc22
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc22
 JMP cc23
cc21
 LDD  #cc1+34
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc24
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc25
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc25
 JSR loadsec
 JSR pushsec
 LDD  22,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  cptr
 CMPD #0
 LBEQ cc26
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc28 _
 LDD  #1
 LDD  cptr
 ADDD #10
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #2
 PSHS D
 LDD  #2
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc28 _
 LDD  #1
 LDD  #1
cc28
 CMPD #0
 LBEQ cc29 _
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc29 _
 LDD  #1
cc29
 CMPD #0
 LBEQ cc27
 JSR doublere
cc27
cc26
 JSR add
 JMP cc30
cc24
 LDD  #cc1+37
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc31
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc32
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc32
 JSR loadsec
 JSR pushsec
 LDD  22,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  cptr
 CMPD #0
 LBEQ cc33
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc35 _
 LDD  #1
 LDD  cptr
 ADDD #10
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #2
 PSHS D
 LDD  #2
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc35 _
 LDD  #1
 LDD  #1
cc35
 CMPD #0
 LBEQ cc36 _
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc36 _
 LDD  #1
cc36
 CMPD #0
 LBEQ cc34
 JSR doublere
cc34
cc33
 JSR sub
 JMP cc37
cc31
 LDD  #cc1+40
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc38
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc39
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc39
 JSR loadsec
 JSR mult
 JMP cc40
cc38
 LDD  #cc1+43
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc41
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc42
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc42
 JSR loadsec
 JSR div
 JMP cc43
cc41
 LDD  #cc1+46
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc44
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc45
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc45
 JSR loadsec
 JSR mod
 JMP cc46
cc44
 LDD  #cc1+49
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc47
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc48
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc48
 JSR loadsec
 JSR pushsec
 JSR asr
 JMP cc49
cc47
 LDD  #cc1+53
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc50
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc51
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc51
 JSR loadsec
 JSR pushsec
 JSR asl
 JMP cc52
cc50
 LDD  #cc1+57
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc53
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc54
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc54
 JSR loadsec
 JSR pushsec
 JSR and
 JMP cc55
cc53
 LDD  #cc1+60
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc56
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc57
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc57
 JSR loadsec
 JSR pushsec
 JSR xor
 JMP cc58
cc56
 LDD  #cc1+63
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc59
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc60
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc60
 JSR loadsec
 JSR pushsec
 JSR or
cc59
cc58
cc55
cc52
cc49
cc46
cc43
cc40
cc37
cc30
cc23
 LDD  22,S
 PSHS D
 JSR store
 LEAS 2,S
 LDD  #0
 LEAS 20,S
 RTS
cc1 FCB 43,61,0,45,61,0,42,61,0,47
 FCB 61,0,37,61,0,60,60,61,0,62
 FCB 62,61,0,38,61,0,124,61,0,94
 FCB 61,0,61,0,43,61,0,45,61,0
 FCB 42,61,0,47,61,0,37,61,0,62
 FCB 62,61,0,60,60,61,0,38,61,0
 FCB 94,61,0,124,61,0
 GLOBAL heir1a
heir1a
 LEAS -6,S
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  10,S
 PSHS D
 JSR heir1b
 LEAS 2,S
 STD  [,S++]
 LDD  #cc61+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc62
 LDD  4,S
 CMPD #0
 LBEQ cc63
 LDD  8,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc63
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 PSHS D
 JSR testjump
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 LDD  8,S
 PSHS D
 JSR heir1a
 LEAS 2,S
 CMPD #0
 LBEQ cc64
 LDD  8,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc64
 LDD  0,S
 PSHS D
 JSR jump
 LEAS 2,S
 LDD  #cc61+2
 PSHS D
 JSR needbrac
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 LDD  8,S
 PSHS D
 JSR heir1a
 LEAS 2,S
 CMPD #0
 LBEQ cc65
 LDD  8,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc65
 LDD  0,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 LDD  #0
 LEAS 6,S
 RTS
cc62
 LDD  4,S
 LEAS 6,S
 RTS
cc66
 LEAS 6,S
 RTS
cc61 FCB 63,0,58,0
 GLOBAL heir1b
heir1b
 LEAS -22,S
 LEAY 20,S
 TFR  Y,D
 PSHS D
 LDD  26,S
 PSHS D
 JSR heir1c
 LEAS 2,S
 STD  [,S++]
 JSR blanks
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc67+0
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc68
 LDD  20,S
 LEAS 22,S
 RTS
cc68
 LDD  20,S
 CMPD #0
 LBEQ cc69
 LDD  24,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc69
cc70
 LDD  #1
 CMPD #0
 LBEQ cc71
 LDD  #cc67+3
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc72
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 PSHS D
 JSR testtrue
 LEAS 2,S
 LEAY 4,S
 TFR  Y,D
 PSHS D
 JSR heir1c
 LEAS 2,S
 CMPD #0
 LBEQ cc73
 LEAY 4,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc73
 LDD  0,S
 PSHS D
 JSR testtrue
 LEAS 2,S
 JSR immed
 LDD  #0
 PSHS D
 JSR outdec
 LEAS 2,S
 JSR nl
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 PSHS D
 JSR jump
 LEAS 2,S
 LDD  0,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 JSR immed
 LDD  #1
 PSHS D
 JSR outdec
 LEAS 2,S
 JSR nl
 LDD  2,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 JMP cc74
cc72
 LDD  #0
 LEAS 22,S
 RTS
cc74
 JMP cc70
cc71
 LEAS 22,S
 RTS
cc67 FCB 124,124,0,124,124,0
 GLOBAL heir1c
heir1c
 LEAS -20,S
 LEAY 18,S
 TFR  Y,D
 PSHS D
 LDD  24,S
 PSHS D
 JSR heir2
 LEAS 2,S
 STD  [,S++]
 JSR blanks
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc75+0
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc76
 LDD  18,S
 LEAS 20,S
 RTS
cc76
 LDD  18,S
 CMPD #0
 LBEQ cc77
 LDD  22,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc77
cc78
 LDD  #1
 CMPD #0
 LBEQ cc79
 LDD  #cc75+3
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc80
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 PSHS D
 JSR testnoop
 LEAS 2,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir2
 LEAS 2,S
 CMPD #0
 LBEQ cc81
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc81
 LDD  0,S
 PSHS D
 JSR testnoop
 LEAS 2,S
 JSR immed
 LDD  #1
 PSHS D
 JSR outdec
 LEAS 2,S
 JSR nl
 LDD  0,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 JMP cc82
cc80
 LDD  #0
 LEAS 20,S
 RTS
cc82
 JMP cc78
cc79
 LEAS 20,S
 RTS
cc75 FCB 38,38,0,38,38,0
 GLOBAL heir2
heir2
 LEAS -18,S
 LEAY 16,S
 TFR  Y,D
 PSHS D
 LDD  22,S
 PSHS D
 JSR heir3
 LEAS 2,S
 STD  [,S++]
 JSR blanks
 JSR ch
 PSHS D
 LDD  #124
 CMPD ,S++
 LBEQ cc84
 LDD  16,S
 LEAS 18,S
 RTS
cc84
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc83+0
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc85
 LDD  16,S
 LEAS 18,S
 RTS
cc85
 LDD  16,S
 CMPD #0
 LBEQ cc86
 LDD  20,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc86
cc87
 LDD  #1
 CMPD #0
 LBEQ cc88
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc83+3
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc89
 LDD  #0
 LEAS 18,S
 RTS
cc89
 LDD  #cc83+6
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc90
 JSR push
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir3
 LEAS 2,S
 CMPD #0
 LBEQ cc91
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc91
 JSR or
 JMP cc92
cc90
 LDD  #0
 LEAS 18,S
 RTS
cc92
 JMP cc87
cc88
 LEAS 18,S
 RTS
cc83 FCB 124,61,0,124,124,0,124,0
 GLOBAL heir3
heir3
 LEAS -18,S
 LEAY 16,S
 TFR  Y,D
 PSHS D
 LDD  22,S
 PSHS D
 JSR heir4
 LEAS 2,S
 STD  [,S++]
 JSR blanks
 JSR ch
 PSHS D
 LDD  #94
 CMPD ,S++
 LBEQ cc94
 LDD  16,S
 LEAS 18,S
 RTS
cc94
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc93+0
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc95
 LDD  16,S
 LEAS 18,S
 RTS
cc95
 LDD  16,S
 CMPD #0
 LBEQ cc96
 LDD  20,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc96
cc97
 LDD  #1
 CMPD #0
 LBEQ cc98
 LDD  #cc93+3
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc99
 JSR push
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir4
 LEAS 2,S
 CMPD #0
 LBEQ cc100
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc100
 JSR xor
 JMP cc101
cc99
 LDD  #0
 LEAS 18,S
 RTS
cc101
 JMP cc97
cc98
 LEAS 18,S
 RTS
cc93 FCB 94,61,0,94,0
 GLOBAL heir4
heir4
 LEAS -18,S
 LEAY 16,S
 TFR  Y,D
 PSHS D
 LDD  22,S
 PSHS D
 JSR heir5
 LEAS 2,S
 STD  [,S++]
 JSR blanks
 JSR ch
 PSHS D
 LDD  #38
 CMPD ,S++
 LBEQ cc103
 LDD  16,S
 LEAS 18,S
 RTS
cc103
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc102+0
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc104
 LDD  16,S
 LEAS 18,S
 RTS
cc104
 LDD  16,S
 CMPD #0
 LBEQ cc105
 LDD  20,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc105
cc106
 LDD  #1
 CMPD #0
 LBEQ cc107
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc102+3
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc108
 LDD  #0
 LEAS 18,S
 RTS
cc108
 LDD  #cc102+6
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc109
 JSR push
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir5
 LEAS 2,S
 CMPD #0
 LBEQ cc110
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc110
 JSR and
 JMP cc111
cc109
 LDD  #0
 LEAS 18,S
 RTS
cc111
 JMP cc106
cc107
 LEAS 18,S
 RTS
cc102 FCB 38,61,0,38,38,0,38,0
 GLOBAL heir5
heir5
 LEAS -20,S
 LEAY 18,S
 TFR  Y,D
 PSHS D
 LDD  24,S
 PSHS D
 JSR heir6
 LEAS 2,S
 STD  [,S++]
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #line
 ADDD lptr
 STD  [,S++]
 JSR blanks
 LDD  0,S
 PSHS D
 LDD  #cc112+0
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc114 _
 LDD  #1
 LDD  0,S
 PSHS D
 LDD  #cc112+3
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc114 _
 LDD  #1
 LDD  #1
cc114
 CMPD #0
 LBEQ cc113
 LDD  18,S
 LEAS 20,S
 RTS
cc113
 LDD  18,S
 CMPD #0
 LBEQ cc115
 LDD  22,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc115
cc116
 LDD  #1
 CMPD #0
 LBEQ cc117
 LDD  #cc112+6
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc118
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir6
 LEAS 2,S
 CMPD #0
 LBEQ cc119
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc119
 JSR eq
 JMP cc120
cc118
 LDD  #cc112+9
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc121
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir6
 LEAS 2,S
 CMPD #0
 LBEQ cc122
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc122
 JSR ne
 JMP cc123
cc121
 LDD  #0
 LEAS 20,S
 RTS
cc123
cc120
 JMP cc116
cc117
 LEAS 20,S
 RTS
cc112 FCB 61,61,0,33,61,0,61,61,0,33
 FCB 61,0
 GLOBAL heir6
heir6
 LEAS -20,S
 LEAY 18,S
 TFR  Y,D
 PSHS D
 LDD  24,S
 PSHS D
 JSR heir7
 LEAS 2,S
 STD  [,S++]
 JSR blanks
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #line
 ADDD lptr
 STD  [,S++]
 JSR ch
 PSHS D
 LDD  #60
 CMPD ,S++
 BNE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 PSHS D
 JSR ch
 PSHS D
 LDD  #62
 CMPD ,S++
 BNE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ANDA ,S+
 ANDB ,S+
 PSHS D
 LDD  2,S
 PSHS D
 LDD  #cc124+0
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ANDA ,S+
 ANDB ,S+
 PSHS D
 LDD  2,S
 PSHS D
 LDD  #cc124+3
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ANDA ,S+
 ANDB ,S+
 CMPD #0
 LBEQ cc125
 LDD  18,S
 LEAS 20,S
 RTS
cc125
 LDD  0,S
 PSHS D
 LDD  #cc124+6
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc126
 LDD  18,S
 LEAS 20,S
 RTS
cc126
 LDD  0,S
 PSHS D
 LDD  #cc124+9
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc127
 LDD  18,S
 LEAS 20,S
 RTS
cc127
 LDD  0,S
 PSHS D
 LDD  #cc124+12
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc128
 LDD  18,S
 LEAS 20,S
 RTS
cc128
 LDD  0,S
 PSHS D
 LDD  #cc124+16
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc129
 LDD  18,S
 LEAS 20,S
 RTS
cc129
 LDD  18,S
 CMPD #0
 LBEQ cc130
 LDD  22,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc130
cc131
 LDD  #1
 CMPD #0
 LBEQ cc132
 LDD  #cc124+20
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc133
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir7
 LEAS 2,S
 CMPD #0
 LBEQ cc134
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc134
 LDD  22,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  cptr
 CMPD #0
 LBEQ cc135
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc137 _
 LDD  #1
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc137 _
 LDD  #1
cc137
 CMPD #0
 LBEQ cc136
 JSR ule
 JMP cc131
cc136
cc135
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  cptr
 CMPD #0
 LBEQ cc138
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc140 _
 LDD  #1
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc140 _
 LDD  #1
cc140
 CMPD #0
 LBEQ cc139
 JSR ule
 JMP cc131
cc139
cc138
 JSR le
 JMP cc141
cc133
 LDD  #cc124+23
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc142
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir7
 LEAS 2,S
 CMPD #0
 LBEQ cc143
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc143
 LDD  22,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  cptr
 CMPD #0
 LBEQ cc144
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc146 _
 LDD  #1
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc146 _
 LDD  #1
cc146
 CMPD #0
 LBEQ cc145
 JSR uge
 JMP cc131
cc145
cc144
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  cptr
 CMPD #0
 LBEQ cc147
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc149 _
 LDD  #1
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc149 _
 LDD  #1
cc149
 CMPD #0
 LBEQ cc148
 JSR uge
 JMP cc131
cc148
cc147
 JSR ge
 JMP cc150
cc142
 JSR ch
 PSHS D
 LDD  #60
 CMPD ,S++
 LBNE cc151
 JSR inbyte
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir7
 LEAS 2,S
 CMPD #0
 LBEQ cc152
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc152
 LDD  22,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  cptr
 CMPD #0
 LBEQ cc153
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc155 _
 LDD  #1
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc155 _
 LDD  #1
cc155
 CMPD #0
 LBEQ cc154
 JSR ult
 JMP cc131
cc154
cc153
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  cptr
 CMPD #0
 LBEQ cc156
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc158 _
 LDD  #1
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc158 _
 LDD  #1
cc158
 CMPD #0
 LBEQ cc157
 JSR ult
 JMP cc131
cc157
cc156
 JSR lt
 JMP cc159
cc151
 JSR ch
 PSHS D
 LDD  #62
 CMPD ,S++
 LBNE cc160
 JSR inbyte
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir7
 LEAS 2,S
 CMPD #0
 LBEQ cc161
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc161
 LDD  22,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  cptr
 CMPD #0
 LBEQ cc162
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc164 _
 LDD  #1
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc164 _
 LDD  #1
cc164
 CMPD #0
 LBEQ cc163
 JSR ugt
 JMP cc131
cc163
cc162
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  cptr
 CMPD #0
 LBEQ cc165
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc167 _
 LDD  #1
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc167 _
 LDD  #1
cc167
 CMPD #0
 LBEQ cc166
 JSR ugt
 JMP cc131
cc166
cc165
 JSR gt
 JMP cc168
cc160
 LDD  #0
 LEAS 20,S
 RTS
cc168
cc159
cc150
cc141
 JMP cc131
cc132
 LEAS 20,S
 RTS
cc124 FCB 60,61,0,62,61,0,62,62,0,60
 FCB 60,0,62,62,61,0,60,60,61,0
 FCB 60,61,0,62,61,0
 END
