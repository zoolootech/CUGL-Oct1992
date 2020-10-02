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
 EXT addglb
 EXT blanks
 EXT callfunc
 EXT ch
 EXT clearsta
 EXT doublere
 EXT gch
 EXT getloc
 EXT getmem
 EXT errrpt
 EXT expressi
 EXT findglb
 EXT findloc
 EXT findstat
 EXT heir1
 EXT immed
 EXT inbyte
 EXT indirect
 EXT junk
 EXT match
 EXT nch
 EXT needbrac
 EXT nl
 EXT numeric
 EXT outbyte
 EXT outdec
 EXT outstr
 EXT printlab
 EXT push
 EXT putint
 EXT putmem
 EXT putstk
 EXT setstage
 EXT symname
 EXT testjump
 EXT constval
 EXT litlab
 EXT litptr
 EXT litq
 EXT quote
 NAME heir11
 GLOBAL heir11
heir11
 LEAS -4,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 JSR primary
 LEAS 2,S
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
 JSR blanks
 JSR ch
 PSHS D
 LDD  #91
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc3
 JSR ch
 PSHS D
 LDD  #40
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc3
 LDD  #0
 JMP cc4
cc3
 LDD  #1
cc4
 CMPD #0
 LBEQ cc2
cc5
 LDD  #1
 CMPD #0
 LBEQ cc6
 LDD  #cc1+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc7
 LDD  0,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc8
 LDD  #cc1+2
 PSHS D
 JSR errrpt
 LEAS 2,S
 JSR junk
 LDD  #cc1+18
 PSHS D
 JSR needbrac
 LEAS 2,S
 LDD  #0
 LEAS 4,S
 RTS
cc8
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 LBNE cc10
 LDD  6,S
 PSHS D
 JSR rvalue
 LEAS 2,S
 JMP cc11
cc10
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #2
 CMPD ,S++
 LBEQ cc12
 LDD  #cc1+20
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 STD  2,S
cc12
cc11
cc9
 JSR push
 JSR expressi
 LDD  #cc1+36
 PSHS D
 JSR needbrac
 LEAS 2,S
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
 LBNE cc13
 JSR doublere
cc13
 JSR add
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
 LDD  #1
 STD  2,S
 JMP cc14
cc7
 LDD  #cc1+38
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc15
 LDD  0,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc16
 LDD  #0
 PSHS D
 JSR callfunc
 LEAS 2,S
 JMP cc17
cc16
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #4
 CMPD ,S++
 LBEQ cc18
 LDD  6,S
 PSHS D
 JSR rvalue
 LEAS 2,S
 LDD  #0
 PSHS D
 JSR callfunc
 LEAS 2,S
 JMP cc19
cc18
 LDD  0,S
 PSHS D
 JSR callfunc
 LEAS 2,S
cc19
cc17
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #0
 STD  [,S++]
 STD  [,S++]
 JMP cc20
cc15
 LDD  2,S
 LEAS 4,S
 RTS
cc20
cc14
 JMP cc5
cc6
cc2
 LDD  0,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc21
 LDD  2,S
 LEAS 4,S
 RTS
cc21
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #4
 CMPD ,S++
 LBNE cc22
 JSR immed
 LDD  0,S
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 LDD  #0
 LEAS 4,S
 RTS
cc22
 LDD  2,S
 LEAS 4,S
 RTS
cc1 FCB 91,0,99,97,110,39,116,32,115,117
 FCB 98,115,99,114,105,112,116,0,93,0
 FCB 99,97,110,39,116,32,115,117,98,115
 FCB 99,114,105,112,116,0,93,0,40,0
 GLOBAL primary
primary
 LEAS -17,S
 LDD  #cc23+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc24
cc25
 LDD  #1
 CMPD #0
 LBEQ cc26
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  21,S
 PSHS D
 JSR heir1
 LEAS 2,S
 STD  [,S++]
 LDD  #cc23+2
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc27
 JMP cc26
cc27
 JMP cc25
cc26
 LDD  #cc23+4
 PSHS D
 JSR needbrac
 LEAS 2,S
 LDD  0,S
 LEAS 17,S
 RTS
cc24
 LEAY 4,S
 TFR  Y,D
 PSHS D
 JSR symname
 LEAS 2,S
 CMPD #0
 LBEQ cc28
 LEAY 13,S
 TFR  Y,D
 PSHS D
 LEAY 6,S
 TFR  Y,D
 PSHS D
 JSR findloc
 LEAS 2,S
 STD  [,S++]
 CMPD #0
 LBEQ cc29
 LEAY 15,S
 TFR  Y,D
 PSHS D
 LEAY 6,S
 TFR  Y,D
 PSHS D
 JSR findstat
 LEAS 2,S
 STD  [,S++]
 CMPD #0
 LBEQ cc30
 LDD  19,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  17,S
 STD  [,S++]
 LDD  19,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #0
 STD  [,S++]
 LDD  15,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #2
 CMPD ,S++
 LBEQ cc31
 LDD  #1
 LEAS 17,S
 RTS
cc31
 JSR immed
 LDD  15,S
 ADDD #14
 PSHS D
 JSR outstr
 LEAS 2,S
 LDD  #66
 PSHS D
 JSR outbyte
 LEAS 2,S
 JSR nl
 LDD  19,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  17,S
 ADDD #10
 PSHS D
 LDB  [,S++]
 SEX
 STD  [,S++]
 LDD  #0
 LEAS 17,S
 RTS
cc30
 LEAY 15,S
 TFR  Y,D
 PSHS D
 LDD  15,S
 STD  [,S++]
 LDD  15,S
 PSHS D
 JSR getloc
 LEAS 2,S
 LDD  19,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  17,S
 STD  [,S++]
 LDD  19,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  17,S
 ADDD #10
 PSHS D
 LDB  [,S++]
 SEX
 STD  [,S++]
 LDD  15,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 LBNE cc33
 LDD  19,S
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
cc33
 LDD  15,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #2
 CMPD ,S++
 LBNE cc34
 LDD  #0
 LEAS 17,S
 RTS
cc34
 LDD  #1
 LEAS 17,S
 RTS
cc35
cc32
cc29
 LEAY 15,S
 TFR  Y,D
 PSHS D
 LEAY 6,S
 TFR  Y,D
 PSHS D
 JSR findglb
 LEAS 2,S
 STD  [,S++]
 CMPD #0
 LBEQ cc36
 LDD  15,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #4
 CMPD ,S++
 LBEQ cc37
 LDD  19,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  17,S
 STD  [,S++]
 LDD  19,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #0
 STD  [,S++]
 LDD  15,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #2
 CMPD ,S++
 LBEQ cc38
 LDD  #1
 LEAS 17,S
 RTS
cc38
 JSR immed
 LDD  15,S
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 LDD  19,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  17,S
 ADDD #10
 PSHS D
 LDB  [,S++]
 SEX
 STD  [,S++]
 LDD  #0
 LEAS 17,S
 RTS
cc37
cc36
 LEAY 15,S
 TFR  Y,D
 PSHS D
 LEAY 6,S
 TFR  Y,D
 PSHS D
 LDD  #4
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
 PSHS D
 LDD  #0
 PSHS D
 LDD  #1
 PSHS D
 JSR addglb
 LEAS 10,S
 STD  [,S++]
 LDD  19,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  17,S
 STD  [,S++]
 LDD  19,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #0
 STD  [,S++]
 LDD  #0
 LEAS 17,S
 RTS
cc28
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR constant
 LEAS 2,S
 CMPD #0
 LBEQ cc39
 LDD  #constval
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #1
 STD  [,S++]
 LDD  #constval
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 LDD  19,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  21,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #0
 STD  [,S++]
 STD  [,S++]
 LEAS 17,S
 RTS
cc39
 LDD  #cc23+6
 PSHS D
 JSR errrpt
 LEAS 2,S
 JSR immed
 LDD  #0
 PSHS D
 JSR outdec
 LEAS 2,S
 JSR nl
 JSR junk
 LDD  #0
 LEAS 17,S
 RTS
cc40
 LEAS 17,S
 RTS
cc23 FCB 40,0,44,0,41,0,105,110,118,97
 FCB 108,105,100,32,101,120,112,114,101,115
 FCB 115,105,111,110,0
 GLOBAL store
store
 LDD  2,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc42
 LDD  2,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR putmem
 LEAS 2,S
 JMP cc43
cc42
 LDD  2,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR putstk
 LEAS 2,S
cc43
 RTS
 GLOBAL rvalue
rvalue
 LDD  2,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc44
 RTS
cc44
 LDD  2,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc45
 LDD  2,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR getmem
 LEAS 2,S
 JMP cc46
cc45
 LDD  2,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR indirect
 LEAS 2,S
cc46
 RTS
 GLOBAL test
test
 LEAS -20,S
 LDD  22,S
 CMPD #0
 LBEQ cc47
 LDD  #cc41+0
 PSHS D
 JSR needbrac
 LEAS 2,S
cc47
cc48
 LDD  #1
 CMPD #0
 LBEQ cc49
 LEAY 18,S
 TFR  Y,D
 PSHS D
 LEAY 18,S
 TFR  Y,D
 PSHS D
 JSR setstage
 LEAS 4,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR heir1
 LEAS 2,S
 CMPD #0
 LBEQ cc50
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR rvalue
 LEAS 2,S
cc50
 LDD  #cc41+2
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc51
 LDD  18,S
 PSHS D
 LDD  18,S
 PSHS D
 JSR clearsta
 LEAS 4,S
 JMP cc52
cc51
 JMP cc49
cc52
 JMP cc48
cc49
 LDD  22,S
 CMPD #0
 LBEQ cc53
 LDD  #cc41+4
 PSHS D
 JSR needbrac
 LEAS 2,S
cc53
 LDD  24,S
 PSHS D
 JSR testjump
 LEAS 2,S
 LDD  18,S
 PSHS D
 LDD  18,S
 PSHS D
 JSR clearsta
 LEAS 4,S
 LEAS 20,S
 RTS
cc41 FCB 40,0,44,0,41,0
 GLOBAL constexp
constexp
 LEAS -6,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR setstage
 LEAS 4,S
 LDD  #constval
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #constval
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #0
 STD  [,S++]
 STD  [,S++]
 JSR expressi
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  #constval
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 LDD  8,S
 PSHS D
 LDD  #constval
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 LDD  2,S
 PSHS D
 LDD  #0
 PSHS D
 JSR clearsta
 LEAS 4,S
 LDD  4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc55
 LDD  #cc54+0
 PSHS D
 JSR errrpt
 LEAS 2,S
cc55
 LDD  4,S
 LEAS 6,S
 RTS
cc54 FCB 109,117,115,116,32,98,101,32,99,111
 FCB 110,115,116,97,110,116,32,101,120,112
 FCB 114,101,115,115,105,111,110,0
 GLOBAL constant
constant
 LDD  2,S
 PSHS D
 JSR number
 LEAS 2,S
 CMPD #0
 LBEQ cc57
 JSR immed
 JMP cc58
cc57
 LDD  2,S
 PSHS D
 JSR pstr
 LEAS 2,S
 CMPD #0
 LBEQ cc59
 JSR immed
 JMP cc60
cc59
 LDD  2,S
 PSHS D
 JSR qstr
 LEAS 2,S
 CMPD #0
 LBEQ cc61
 JSR immed
 LDD  litlab
 PSHS D
 JSR printlab
 LEAS 2,S
 LDD  #43
 PSHS D
 JSR outbyte
 LEAS 2,S
 JMP cc62
cc61
 LDD  #0
 RTS
cc62
cc60
cc58
 LDD  2,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR outdec
 LEAS 2,S
 JSR nl
 LDD  #1
 RTS
 GLOBAL number
number
 LEAS -5,S
 LEAY 3,S
 TFR  Y,D
 PSHS D
 LDD  #1
 STD  3,S
 STD  [,S++]
cc63
 LDD  3,S
 CMPD #0
 LBEQ cc64
 LDD  #0
 STD  3,S
 LDD  #cc56+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc65
 LDD  #1
 STD  3,S
cc65
 LDD  #cc56+2
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc66
 LEAY 1,S
 TFR  Y,D
 PSHS D
 LDD  3,S
 COMA
 COMB
 ADDD #1
 STD  [,S++]
 LDD  #1
 STD  3,S
cc66
 JMP cc63
cc64
 JSR ch
 PSHS D
 JSR numeric
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc67
 LDD  #0
 LEAS 5,S
 RTS
cc67
 LDD  #cc56+4
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc68
 LDD  #cc56+6
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #cc56+8
 PSHS D
 JSR match
 LEAS 2,S
 ORA  ,S+
 ORB  ,S+
 CMPD #0
 LBEQ cc69
cc70
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR ch
 PSHS D
 JSR upper
 LEAS 2,S
 STB  [,S++]
 PSHS D
 JSR numeric
 LEAS 2,S
 CMPD #1
 LBEQ cc72
 LDB  0,S
 SEX
 PSHS D
 LDD  #65
 CMPD ,S++
 BLE  *+8
 LDD  #0
 JMP cc73 _
 LDD  #1
 LDB  0,S
 SEX
 PSHS D
 LDD  #70
 CMPD ,S++
 BGE  *+8
 LDD  #0
 JMP cc73 _
 LDD  #1
 LDD  #1
cc73
 CMPD #1
 LBEQ cc72
 LDD  #0
 JMP cc74
cc72
 LDD  #1
cc74
 CMPD #0
 LBEQ cc71
 LDB  0,S
 SEX
 PSHS D
 JSR numeric
 LEAS 2,S
 CMPD #0
 LBEQ cc75
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDB  2,S
 SEX
 PSHS D
 LDD  #48
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STB  [,S++]
 JMP cc76
cc75
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDB  2,S
 SEX
 PSHS D
 LDD  #55
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STB  [,S++]
cc76
 LEAY 3,S
 TFR  Y,D
 PSHS D
 LDD  5,S
 PSHS D
 LDD  #4
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 PSHS D
 LDB  4,S
 SEX
 ADDD ,S++
 STD  [,S++]
 JSR inbyte
 JMP cc70
cc71
 JMP cc77
cc69
cc78
 JSR ch
 PSHS D
 LDD  #48
 CMPD ,S++
 BLE  *+8
 LDD  #0
 JMP cc80 _
 LDD  #1
 JSR ch
 PSHS D
 LDD  #55
 CMPD ,S++
 BGE  *+8
 LDD  #0
 JMP cc80 _
 LDD  #1
 LDD  #1
cc80
 CMPD #0
 LBEQ cc79
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR inbyte
 STB  [,S++]
 LEAY 3,S
 TFR  Y,D
 PSHS D
 LDD  5,S
 PSHS D
 LDD  #3
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 PSHS D
 LDB  4,S
 SEX
 ADDD ,S++
 PSHS D
 LDD  #48
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  [,S++]
 JMP cc78
cc79
cc77
 JMP cc81
cc68
cc82
 JSR ch
 PSHS D
 JSR numeric
 LEAS 2,S
 CMPD #0
 LBEQ cc83
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR inbyte
 STB  [,S++]
 LEAY 3,S
 TFR  Y,D
 PSHS D
 LDD  5,S
 PSHS D
 LDD  #10
 PULS X
 JSR ccmult
 PSHS D
 LDB  4,S
 SEX
 PSHS D
 LDD  #48
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 ADDD ,S++
 STD  [,S++]
 JMP cc82
cc83
cc81
 LDD  1,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBLE cc84
 LEAY 3,S
 TFR  Y,D
 PSHS D
 LDD  5,S
 COMA
 COMB
 ADDD #1
 STD  [,S++]
cc84
 LDD  7,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  5,S
 STD  [,S++]
 LDD  #1
 LEAS 5,S
 RTS
cc56 FCB 43,0,45,0,48,0,120,0,88,0
 GLOBAL upper
upper
 LDB  3,S
 SEX
 PSHS D
 LDD  #97
 CMPD ,S++
 BLE  *+8
 LDD  #0
 JMP cc87 _
 LDD  #1
 LDB  3,S
 SEX
 PSHS D
 LDD  #122
 CMPD ,S++
 BGE  *+8
 LDD  #0
 JMP cc87 _
 LDD  #1
 LDD  #1
cc87
 CMPD #0
 LBEQ cc86
 LDB  3,S
 SEX
 PSHS D
 LDD  #32
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 RTS
cc86
 LDB  3,S
 SEX
 RTS
 GLOBAL pstr
pstr
 LEAS -3,S
 LDD  #0
 STD  1,S
 LDD  #cc85+0
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc88
 LDD  #0
 LEAS 3,S
 RTS
cc88
cc89
 JSR ch
 PSHS D
 LDD  #39
 CMPD ,S++
 LBEQ cc90
 LEAY 1,S
 TFR  Y,D
 PSHS D
 LDD  3,S
 PSHS D
 LDD  #255
 ANDA ,S+
 ANDB ,S+
 PSHS D
 LDD  #256
 PULS X
 JSR ccmult
 PSHS D
 JSR litchar
 PSHS D
 LDD  #255
 ANDA ,S+
 ANDB ,S+
 ADDD ,S++
 STD  [,S++]
 JMP cc89
cc90
 LDD  5,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  3,S
 STD  [,S++]
 JSR gch
 LDD  #1
 LEAS 3,S
 RTS
cc85 FCB 39,0
 GLOBAL qstr
qstr
 LEAS -1,S
 LDD  #quote
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc92
 LDD  #0
 LEAS 1,S
 RTS
cc92
 LDD  3,S
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  litptr
 STD  [,S++]
cc93
 JSR ch
 PSHS D
 LDD  #34
 CMPD ,S++
 LBEQ cc94
 JSR ch
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc95
 JMP cc94
cc95
 LDD  litptr
 PSHS D
 LDD  #1000
 PSHS D
 LDD  #1
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 CMPD ,S++
 LBGT cc96
 LDD  #cc91+0
 PSHS D
 JSR errrpt
 LEAS 2,S
cc97
 LDD  #quote
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc98
 JSR gch
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc99
 JMP cc98
cc99
 JMP cc97
cc98
 LDD  #1
 LEAS 1,S
 RTS
cc96
 LDD  #litq
 PSHS D
 LDD  litptr
 ADDD #1
 STD  litptr
 SUBD #1
 ADDD ,S++
 PSHS D
 JSR litchar
 STB  [,S++]
 JMP cc93
cc94
 JSR gch
 LDD  #litq
 PSHS D
 LDD  litptr
 ADDD #1
 STD  litptr
 SUBD #1
 ADDD ,S++
 PSHS D
 LDD  #0
 STB  [,S++]
 LDD  #1
 LEAS 1,S
 RTS
cc91 FCB 115,116,114,105,110,103,32,115,112,97
 FCB 99,101,32,101,120,104,97,117,115,116
 FCB 101,100,0
 GLOBAL stowlit
stowlit
 LDD  litptr
 PSHS D
 LDD  4,S
 ADDD ,S++
 PSHS D
 LDD  #1000
 PSHS D
 LDD  #1
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 CMPD ,S++
 LBGT cc101
 LDD  #cc100+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 PSHS D
 JSR exit
 LEAS 2,S
cc101
 LDD  4,S
 PSHS D
 LDD  #litq
 ADDD litptr
 PSHS D
 LDD  6,S
 PSHS D
 JSR putint
 LEAS 6,S
 LDD  litptr
 PSHS D
 LDD  4,S
 ADDD ,S++
 STD  litptr
 RTS
cc100 FCB 108,105,116,101,114,97,108,32,113,117
 FCB 101,32,111,118,101,114,102,108,111,119
 FCB 0
 GLOBAL litchar
litchar
 LEAS -4,S
 JSR ch
 PSHS D
 LDD  #92
 CMPD ,S++
 BNE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 PSHS D
 JSR nch
 PSHS D
 LDD  #0
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ORA  ,S+
 ORB  ,S+
 CMPD #0
 LBEQ cc103
 JSR gch
 LEAS 4,S
 RTS
cc103
 JSR gch
 JSR ch
 PSHS D
 LDD  #110
 CMPD ,S++
 LBNE cc104
 JSR gch
 LDD  #13
 LEAS 4,S
 RTS
cc104
 JSR ch
 PSHS D
 LDD  #116
 CMPD ,S++
 LBNE cc105
 JSR gch
 LDD  #9
 LEAS 4,S
 RTS
cc105
 JSR ch
 PSHS D
 LDD  #98
 CMPD ,S++
 LBNE cc106
 JSR gch
 LDD  #8
 LEAS 4,S
 RTS
cc106
 JSR ch
 PSHS D
 LDD  #102
 CMPD ,S++
 LBNE cc107
 JSR gch
 LDD  #12
 LEAS 4,S
 RTS
cc107
 JSR ch
 PSHS D
 LDD  #92
 CMPD ,S++
 LBNE cc108
 JSR gch
 LDD  #92
 LEAS 4,S
 RTS
cc108
 JSR ch
 PSHS D
 LDD  #39
 CMPD ,S++
 LBNE cc109
 JSR gch
 LDD  #39
 LEAS 4,S
 RTS
cc109
 JSR ch
 PSHS D
 LDD  #114
 CMPD ,S++
 LBNE cc110
 JSR gch
 LDD  #13
 LEAS 4,S
 RTS
cc110
 LDD  #3
 STD  2,S
 LDD  #0
 STD  0,S
cc111
 LDD  2,S
 SUBD #1
 STD  2,S
 ADDD #1
 PSHS D
 LDD  #0
 CMPD ,S++
 BLT  *+8
 LDD  #0
 JMP cc113 _
 LDD  #1
 JSR ch
 PSHS D
 LDD  #48
 CMPD ,S++
 BLE  *+8
 LDD  #0
 JMP cc113 _
 LDD  #1
 LDD  #1
cc113
 CMPD #0
 LBEQ cc114 _
 JSR ch
 PSHS D
 LDD  #55
 CMPD ,S++
 BGE  *+8
 LDD  #0
 JMP cc114 _
 LDD  #1
 LDD  #1
cc114
 CMPD #0
 LBEQ cc112
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  2,S
 PSHS D
 LDD  #3
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 PSHS D
 JSR gch
 ADDD ,S++
 PSHS D
 LDD  #48
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  [,S++]
 JMP cc111
cc112
 LDD  2,S
 PSHS D
 LDD  #2
 CMPD ,S++
 LBNE cc115
 JSR gch
 LEAS 4,S
 RTS
cc115
 LDD  0,S
 LEAS 4,S
 RTS
cc116
 LEAS 4,S
 RTS
 END
