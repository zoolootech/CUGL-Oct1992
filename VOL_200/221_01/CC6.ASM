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
 EXT asl
 EXT asr
 EXT blanks
 EXT ch
 EXT com
 EXT dec
 EXT div
 EXT doublere
 EXT errrpt
 EXT heir11
 EXT immed
 EXT inc
 EXT lognot
 EXT match
 EXT mod
 EXT mult
 EXT needlval
 EXT neg
 EXT nl
 EXT outstr
 EXT pop
 EXT push
 EXT rvalue
 EXT store
 EXT streq
 EXT sub
 EXT lptr
 EXT cptr
 EXT line
 NAME heir7
 GLOBAL heir7
heir7
 LEAS -20,S
 LEAY 18,S
 TFR  Y,D
 PSHS D
 LDD  24,S
 PSHS D
 JSR heir8
 LEAS 2,S
 STD  [,S++]
 JSR blanks
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #line
 ADDD lptr
 STD  [,S++]
 LDD  0,S
 PSHS D
 LDD  #cc1+0
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc2
 LDD  18,S
 LEAS 20,S
 RTS
cc2
 LDD  0,S
 PSHS D
 LDD  #cc1+4
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc3
 LDD  18,S
 LEAS 20,S
 RTS
cc3
 LDD  0,S
 PSHS D
 LDD  #cc1+8
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc5 _
 LDD  #1
 LDD  0,S
 PSHS D
 LDD  #cc1+11
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc5 _
 LDD  #1
 LDD  #1
cc5
 CMPD #0
 LBEQ cc4
 LDD  18,S
 LEAS 20,S
 RTS
cc4
 LDD  18,S
 CMPD #0
 LBEQ cc6
 LDD  22,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc6
cc7
 LDD  #1
 CMPD #0
 LBEQ cc8
 LDD  #cc1+14
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc9
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir8
 LEAS 2,S
 CMPD #0
 LBEQ cc10
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc10
 JSR asr
 JMP cc11
cc9
 LDD  #cc1+17
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc12
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir8
 LEAS 2,S
 CMPD #0
 LBEQ cc13
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc13
 JSR asl
 JMP cc14
cc12
 LDD  #0
 LEAS 20,S
 RTS
cc14
cc11
 JMP cc7
cc8
 LEAS 20,S
 RTS
cc1 FCB 62,62,61,0,60,60,61,0,62,62
 FCB 0,60,60,0,62,62,0,60,60,0
 GLOBAL heir8
heir8
 LEAS -20,S
 LEAY 18,S
 TFR  Y,D
 PSHS D
 LDD  24,S
 PSHS D
 JSR heir9
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
 LDD  #43
 CMPD ,S++
 BNE  *+8
 LDD  #0
 JMP cc17 _
 LDD  #1
 JSR ch
 PSHS D
 LDD  #45
 CMPD ,S++
 BNE  *+8
 LDD  #0
 JMP cc17 _
 LDD  #1
 LDD  #1
cc17
 CMPD #0
 LBEQ cc16
 LDD  18,S
 LEAS 20,S
 RTS
cc16
 LDD  0,S
 PSHS D
 LDD  #cc15+0
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc18
 LDD  18,S
 LEAS 20,S
 RTS
cc18
 LDD  0,S
 PSHS D
 LDD  #cc15+3
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc19
 LDD  18,S
 LEAS 20,S
 RTS
cc19
 LDD  18,S
 CMPD #0
 LBEQ cc20
 LDD  22,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc20
cc21
 LDD  #1
 CMPD #0
 LBEQ cc22
 LDD  #cc15+6
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc23
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir9
 LEAS 2,S
 CMPD #0
 LBEQ cc24
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc24
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
 LBEQ cc25
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
 JMP cc27 _
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
 JMP cc27 _
 LDD  #1
 LDD  #1
cc27
 CMPD #0
 LBEQ cc28 _
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc28 _
 LDD  #1
cc28
 CMPD #0
 LBEQ cc26
 JSR doublere
cc26
cc25
 JSR add
 JMP cc29
cc23
 LDD  #cc15+8
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc30
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir9
 LEAS 2,S
 CMPD #0
 LBEQ cc31
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc31
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
 LBEQ cc32
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
 JMP cc34 _
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
 JMP cc34 _
 LDD  #1
 LDD  #1
cc34
 CMPD #0
 LBEQ cc35 _
 LDD  22,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc35 _
 LDD  #1
cc35
 CMPD #0
 LBEQ cc33
 JSR doublere
cc33
cc32
 JSR sub
 JMP cc36
cc30
 LDD  #0
 LEAS 20,S
 RTS
cc36
cc29
 JMP cc21
cc22
 LEAS 20,S
 RTS
cc15 FCB 43,61,0,45,61,0,43,0,45,0
 GLOBAL heir9
heir9
 LEAS -20,S
 LEAY 18,S
 TFR  Y,D
 PSHS D
 LDD  24,S
 PSHS D
 JSR heir10
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
 LDD  #42
 CMPD ,S++
 BNE  *+8
 LDD  #0
 JMP cc39 _
 LDD  #1
 JSR ch
 PSHS D
 LDD  #47
 CMPD ,S++
 BNE  *+8
 LDD  #0
 JMP cc39 _
 LDD  #1
 LDD  #1
cc39
 CMPD #0
 LBEQ cc40 _
 JSR ch
 PSHS D
 LDD  #37
 CMPD ,S++
 BNE  *+8
 LDD  #0
 JMP cc40 _
 LDD  #1
 LDD  #1
cc40
 CMPD #0
 LBEQ cc38
 LDD  18,S
 LEAS 20,S
 RTS
cc38
 LDD  0,S
 PSHS D
 LDD  #cc37+0
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc41
 LDD  18,S
 LEAS 20,S
 RTS
cc41
 LDD  0,S
 PSHS D
 LDD  #cc37+3
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc42
 LDD  18,S
 LEAS 20,S
 RTS
cc42
 LDD  0,S
 PSHS D
 LDD  #cc37+6
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc43
 LDD  18,S
 LEAS 20,S
 RTS
cc43
 LDD  18,S
 CMPD #0
 LBEQ cc44
 LDD  22,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc44
cc45
 LDD  #1
 CMPD #0
 LBEQ cc46
 LDD  #cc37+9
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc47
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir9
 LEAS 2,S
 CMPD #0
 LBEQ cc48
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc48
 JSR pop
 JSR mult
 JMP cc49
cc47
 LDD  #cc37+11
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc50
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir10
 LEAS 2,S
 CMPD #0
 LBEQ cc51
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc51
 JSR pop
 JSR div
 JMP cc52
cc50
 LDD  #cc37+13
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc53
 JSR push
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR heir10
 LEAS 2,S
 CMPD #0
 LBEQ cc54
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc54
 JSR pop
 JSR mod
 JMP cc55
cc53
 LDD  #0
 LEAS 20,S
 RTS
cc55
cc52
cc49
 JMP cc45
cc46
 LEAS 20,S
 RTS
cc37 FCB 42,61,0,47,61,0,37,61,0,42
 FCB 0,47,0,37,0
 GLOBAL heir10
heir10
 LEAS -4,S
 LDD  #cc56+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc57
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 JSR heir10
 LEAS 2,S
 STD  [,S++]
 LDD  2,S
 CMPD #0
 LBEQ cc58
 LDD  6,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc58
 JSR lognot
 LDD  #0
 LEAS 4,S
 RTS
cc57
 LDD  #cc56+2
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc60
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 JSR heir10
 LEAS 2,S
 STD  [,S++]
 LDD  2,S
 CMPD #0
 LBEQ cc61
 LDD  6,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc61
 JSR com
 LDD  #0
 LEAS 4,S
 RTS
cc60
 LDD  #cc56+4
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc63
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 JSR heir10
 LEAS 2,S
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc64
 JSR needlval
 LDD  #0
 LEAS 4,S
 RTS
cc64
 LDD  6,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc65
 JSR push
cc65
 LDD  6,S
 PSHS D
 JSR rvalue
 LEAS 2,S
 JSR inc
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc67 _
 LDD  #1
 LDD  0,S
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
 JMP cc67 _
 LDD  #1
 LDD  #1
cc67
 CMPD #0
 LBEQ cc68 _
 LDD  6,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc68 _
 LDD  #1
cc68
 CMPD #0
 LBEQ cc66
 JSR inc
cc66
 LDD  6,S
 PSHS D
 JSR store
 LEAS 2,S
 LDD  #0
 LEAS 4,S
 RTS
cc63
 LDD  #cc56+7
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc70
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 JSR heir10
 LEAS 2,S
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc71
 JSR needlval
 LDD  #0
 LEAS 4,S
 RTS
cc71
 LDD  6,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc72
 JSR push
cc72
 LDD  6,S
 PSHS D
 JSR rvalue
 LEAS 2,S
 JSR dec
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc74 _
 LDD  #1
 LDD  0,S
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
 JMP cc74 _
 LDD  #1
 LDD  #1
cc74
 CMPD #0
 LBEQ cc75 _
 LDD  6,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc75 _
 LDD  #1
cc75
 CMPD #0
 LBEQ cc73
 JSR dec
cc73
 LDD  6,S
 PSHS D
 JSR store
 LEAS 2,S
 LDD  #0
 LEAS 4,S
 RTS
cc70
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc56+10
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc77
 LDD  2,S
 LEAS 4,S
 RTS
cc77
 LDD  #cc56+13
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc79
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 JSR heir10
 LEAS 2,S
 STD  [,S++]
 LDD  2,S
 CMPD #0
 LBEQ cc80
 LDD  6,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc80
 JSR neg
 LDD  #0
 LEAS 4,S
 RTS
cc79
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc56+15
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc82
 LDD  2,S
 LEAS 4,S
 RTS
cc82
 LDD  #cc56+18
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc84
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 JSR heir10
 LEAS 2,S
 STD  [,S++]
 LDD  2,S
 CMPD #0
 LBEQ cc85
 LDD  6,S
 PSHS D
 JSR rvalue
 LEAS 2,S
cc85
 LDD  6,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
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
 STD  [,S++]
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 CMPD #0
 LBEQ cc86
 LDD  6,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  2,S
 ADDD #10
 PSHS D
 LDB  [,S++]
 SEX
 STD  [,S++]
cc86
 LDD  6,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #0
 STD  [,S++]
 LDD  6,S
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #0
 STD  [,S++]
 LDD  #1
 LEAS 4,S
 RTS
cc84
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc56+20
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc88
 LDD  2,S
 LEAS 4,S
 RTS
cc88
 LDD  #cc56+23
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc90
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 JSR heir10
 LEAS 2,S
 STD  [,S++]
 LDD  2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc91
 LDD  #cc56+25
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 LEAS 4,S
 RTS
cc91
 LDD  6,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc93
 LDD  #0
 LEAS 4,S
 RTS
cc93
 JSR immed
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 LDD  6,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  2,S
 ADDD #10
 PSHS D
 LDB  [,S++]
 SEX
 STD  [,S++]
 LDD  #0
 LEAS 4,S
 RTS
cc94
cc92
 JMP cc95
cc90
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 JSR heir11
 LEAS 2,S
 STD  [,S++]
 LDD  #cc56+41
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc96
 LDD  2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc97
 JSR needlval
 LDD  #0
 LEAS 4,S
 RTS
cc97
 LDD  6,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc98
 JSR push
cc98
 LDD  6,S
 PSHS D
 JSR rvalue
 LEAS 2,S
 JSR inc
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc100 _
 LDD  #1
 LDD  0,S
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
 JMP cc100 _
 LDD  #1
 LDD  #1
cc100
 CMPD #0
 LBEQ cc101 _
 LDD  6,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc101 _
 LDD  #1
cc101
 CMPD #0
 LBEQ cc99
 JSR inc
cc99
 LDD  6,S
 PSHS D
 JSR store
 LEAS 2,S
 JSR dec
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc103 _
 LDD  #1
 LDD  0,S
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
 JMP cc103 _
 LDD  #1
 LDD  #1
cc103
 CMPD #0
 LBEQ cc104 _
 LDD  6,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc104 _
 LDD  #1
cc104
 CMPD #0
 LBEQ cc102
 JSR dec
cc102
 LDD  #0
 LEAS 4,S
 RTS
cc96
 LDD  #cc56+44
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc106
 LDD  2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc107
 JSR needlval
 LDD  #0
 LEAS 4,S
 RTS
cc107
 LDD  6,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc108
 JSR push
cc108
 LDD  6,S
 PSHS D
 JSR rvalue
 LEAS 2,S
 JSR dec
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc110 _
 LDD  #1
 LDD  0,S
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
 JMP cc110 _
 LDD  #1
 LDD  #1
cc110
 CMPD #0
 LBEQ cc111 _
 LDD  6,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc111 _
 LDD  #1
cc111
 CMPD #0
 LBEQ cc109
 JSR dec
cc109
 LDD  6,S
 PSHS D
 JSR store
 LEAS 2,S
 JSR inc
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc113 _
 LDD  #1
 LDD  0,S
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
 JMP cc113 _
 LDD  #1
 LDD  #1
cc113
 CMPD #0
 LBEQ cc114 _
 LDD  6,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 CMPD #0
 LBEQ cc114 _
 LDD  #1
cc114
 CMPD #0
 LBEQ cc112
 JSR inc
cc112
 LDD  #0
 LEAS 4,S
 RTS
cc106
 LDD  2,S
 LEAS 4,S
 RTS
cc115
cc105
cc95
cc89
cc87
cc83
cc81
cc78
cc76
cc69
cc62
cc59
;
;^
;******  no final }  ******
 LEAS 4,S
 RTS
cc56 FCB 33,0,126,0,43,43,0,45,45,0
 FCB 45,61,0,45,0,42,61,0,42,0
 FCB 38,61,0,38,0,105,108,108,101,103
 FCB 97,108,32,97,100,100,114,101,115,115
 FCB 0,43,43,0,45,45,0
 END
