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
 EXT nl
 EXT ol
 EXT ot
 EXT outbyte
 EXT outdec
 EXT outstr
 EXT printlab
 EXT postlabe
 EXT stkp
 NAME entry
 GLOBAL entry
entry
 LDD  2,S
 PSHS D
 LDD  #1
 CMPD ,S++
 LBEQ cc2
 LDD  #cc1+0
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  4,S
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
cc2
 LDD  4,S
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 RTS
cc1 FCB 71,76,79,66,65,76,32,0
 GLOBAL sw
sw
 LDD  #cc3+0
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc3 FCB 74,83,82,32,67,67,83,87,73,84
 FCB 67,72,0
 GLOBAL declexte
declexte
 LDD  #cc4+0
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 RTS
cc4 FCB 69,88,84,32,0
 GLOBAL comment
comment
 LDD  #59
 PSHS D
 JSR outbyte
 LEAS 2,S
 RTS
 GLOBAL defname
defname
 LDD  #cc5+0
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 RTS
cc5 FCB 78,65,77,69,32,0
 GLOBAL header
header
 JSR comment
 LDD  #cc6+0
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 RTS
cc6 FCB 115,109,97,108,108,45,99,32,86,50
 FCB 46,51,0
 GLOBAL trailer
trailer
 LDD  #cc7+0
 PSHS D
 JSR ot
 LEAS 2,S
 RTS
cc7 FCB 69,78,68,0
 GLOBAL getmem
getmem
 LEAS -2,S
 LDD  4,S
 ADDD #11
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
 CMPD ,S++
 LBNE cc9
 LDD  #14
 STD  0,S
 JMP cc10
cc9
 LDD  #0
 STD  0,S
cc10
 LDD  4,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BNE  *+8
 LDD  #0
 JMP cc12 _
 LDD  4,S
 ADDD #10
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
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
 JMP cc12 _
 LDD  #1
cc12
 LBEQ cc11
 LDD  #cc8+0
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  4,S
 PSHS D
 LDD  2,S
 ADDD ,S++
 PSHS D
 JSR outstr
 LEAS 2,S
 LDD  4,S
 ADDD #11
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
 CMPD ,S++
 LBNE cc13
 LDD  #66
 PSHS D
 JSR outbyte
 LEAS 2,S
cc13
 JSR nl
 LDD  #cc8+6
 PSHS D
 JSR ol
 LEAS 2,S
 JMP cc14
cc11
 LDD  #cc8+10
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  4,S
 PSHS D
 LDD  2,S
 ADDD ,S++
 PSHS D
 JSR outstr
 LEAS 2,S
 LDD  4,S
 ADDD #11
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
 CMPD ,S++
 LBNE cc15
 LDD  #66
 PSHS D
 JSR outbyte
 LEAS 2,S
cc15
 JSR nl
cc14
 LEAS 2,S
 RTS
cc8 FCB 76,68,66,32,32,0,83,69,88,0
 FCB 76,68,68,32,32,0
 GLOBAL loadsec
loadsec
 LDD  #cc16+0
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc16 FCB 76,68,88,32,32,91,44,83,93,0
 GLOBAL getloc
getloc
 LEAS -2,S
 LDD  #cc17+0
 PSHS D
 JSR ot
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  6,S
 ADDD #12
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #255
 ANDA ,S+
 ANDB ,S+
 PSHS D
 LDD  8,S
 PSHS D
 LDD  #12
 ADDD #1
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #8
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 ORA  ,S+
 ORB  ,S+
 PSHS D
 LDD  stkp
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  [,S++]
 LDD  4,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 PSHS D
 LDD  6,S
 ADDD #10
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
 PSHS D
 LDD  #2
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ANDA ,S+
 ANDB ,S+
 LBEQ cc18
 LDD  0,S
 ADDD #1
 STD  0,S
cc18
 LDD  0,S
 PSHS D
 JSR outdec
 LEAS 2,S
 LDD  #cc17+6
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 LDD  #cc17+9
 PSHS D
 JSR ol
 LEAS 2,S
 LEAS 2,S
 RTS
cc17 FCB 76,69,65,89,32,0,44,83,0,84
 FCB 70,82,32,32,89,44,68,0
 GLOBAL putmem
putmem
 LEAS -2,S
 LDD  4,S
 ADDD #11
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
 CMPD ,S++
 LBNE cc20
 LDD  #14
 STD  0,S
 JMP cc21
cc20
 LDD  #0
 STD  0,S
cc21
 LDD  4,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #3
 CMPD ,S++
 BNE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 PSHS D
 LDD  6,S
 ADDD #10
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
 PSHS D
 LDD  #2
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ANDA ,S+
 ANDB ,S+
 LBEQ cc22
 LDD  #cc19+0
 PSHS D
 JSR ot
 LEAS 2,S
 JMP cc23
cc22
 LDD  #cc19+6
 PSHS D
 JSR ot
 LEAS 2,S
cc23
 LDD  4,S
 PSHS D
 LDD  2,S
 ADDD ,S++
 PSHS D
 JSR outstr
 LEAS 2,S
 LDD  4,S
 ADDD #11
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
 CMPD ,S++
 LBNE cc24
 LDD  #66
 PSHS D
 JSR outbyte
 LEAS 2,S
cc24
 JSR nl
 LEAS 2,S
 RTS
cc19 FCB 83,84,66,32,32,0,83,84,68,32
 FCB 32,0
 GLOBAL putstk
putstk
 LDB  3,S
 SEX
 PSHS D
 LDD  #1
 PSHS D
 LDD  #2
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 CMPD ,S++
 LBNE cc26
 LDD  #cc25+0
 PSHS D
 JSR ol
 LEAS 2,S
 JMP cc27
cc26
 LDD  #cc25+12
 PSHS D
 JSR ol
 LEAS 2,S
cc27
 LDD  stkp
 ADDD #2
 STD  stkp
 RTS
cc25 FCB 83,84,66,32,32,91,44,83,43,43
 FCB 93,0,83,84,68,32,32,91,44,83
 FCB 43,43,93,0
 GLOBAL indirect
indirect
 LDD  #cc28+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDB  3,S
 SEX
 PSHS D
 LDD  #1
 PSHS D
 LDD  #2
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 CMPD ,S++
 LBNE cc29
 LDD  #cc28+7
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc28+19
 PSHS D
 JSR ol
 LEAS 2,S
 JMP cc30
cc29
 LDD  #cc28+23
 PSHS D
 JSR ol
 LEAS 2,S
cc30
 RTS
cc28 FCB 80,83,72,83,32,68,0,76,68,66
 FCB 32,32,91,44,83,43,43,93,0,83
 FCB 69,88,0,76,68,68,32,32,91,44
 FCB 83,43,43,93,0
 GLOBAL swap
swap
 LDD  #cc31+0
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc31 FCB 69,88,71,32,32,68,44,88,0
 GLOBAL immed
immed
 LDD  #cc32+0
 PSHS D
 JSR ot
 LEAS 2,S
 RTS
cc32 FCB 76,68,68,32,32,35,0
 GLOBAL push
push
 LDD  #cc33+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 PSHS D
 LDD  #2
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  stkp
 RTS
cc33 FCB 80,83,72,83,32,68,0
 GLOBAL pushsec
pushsec
 LDD  #cc34+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 PSHS D
 LDD  #2
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  stkp
 RTS
cc34 FCB 80,83,72,83,32,88,0
 GLOBAL pop
pop
 LDD  #cc35+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 ADDD #2
 STD  stkp
 RTS
cc35 FCB 80,85,76,83,32,88,0
 GLOBAL swapstk
swapstk
 LDD  #cc36+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc36+7
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc36+14
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc36 FCB 80,85,76,83,32,88,0,80,83,72
 FCB 83,32,68,0,84,70,82,32,32,88
 FCB 44,68,0
 GLOBAL call
call
 LDD  #cc37+0
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 RTS
cc37 FCB 74,83,82,32,0
 GLOBAL ret
ret
 LDD  #cc38+0
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc38 FCB 82,84,83,0
 GLOBAL callstk
callstk
 JSR pop
 LDD  #cc39+0
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc39 FCB 74,83,82,32,32,44,88,0
 GLOBAL jump
jump
 LDD  #cc40+0
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 RTS
cc40 FCB 74,77,80,32,0
 GLOBAL testnoop
testnoop
 LDD  #cc41+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc41+8
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR printlab
 LEAS 2,S
 LDD  #cc41+14
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 RTS
cc41 FCB 67,77,80,68,32,35,48,0,76,66
 FCB 69,81,32,0,32,95,0
 GLOBAL testjump
testjump
 LDD  #cc42+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc42+8
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 RTS
cc42 FCB 67,77,80,68,32,35,48,0,76,66
 FCB 69,81,32,0
 GLOBAL testtrue
testtrue
 LDD  #cc43+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc43+8
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 RTS
cc43 FCB 67,77,80,68,32,35,48,0,76,66
 FCB 78,69,32,0
 GLOBAL debug
debug
 LDD  #cc44+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc44+10
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  #34
 PSHS D
 JSR outbyte
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR outstr
 LEAS 2,S
 LDD  #34
 PSHS D
 JSR outbyte
 LEAS 2,S
 LDD  #cc44+15
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 RTS
cc44 FCB 74,83,82,32,68,69,66,85,71,0
 FCB 70,67,67,32,0,44,48,0
 GLOBAL defstora
defstora
 LDD  2,S
 PSHS D
 LDD  #1
 CMPD ,S++
 LBNE cc46
 LDD  #cc45+0
 PSHS D
 JSR ot
 LEAS 2,S
 JMP cc47
cc46
 LDD  #cc45+5
 PSHS D
 JSR ot
 LEAS 2,S
cc47
 RTS
cc45 FCB 70,67,66,32,0,70,68,66,32,0
 GLOBAL dumpzero
dumpzero
 LDD  2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBLT cc49
 RTS
cc49
 LDD  #cc48+0
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR outdec
 LEAS 2,S
 JSR nl
 RTS
cc48 FCB 82,90,66,32,0
 GLOBAL point
point
 LDD  #cc50+0
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc50 FCB 70,68,66,32,42,43,50,0
 GLOBAL modstk
modstk
 LEAS -2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  6,S
 PSHS D
 LDD  stkp
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  [,S++]
 LDD  0,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc52
 LDD  4,S
 LEAS 2,S
 RTS
cc52
 LDD  #cc51+0
 PSHS D
 JSR ot
 LEAS 2,S
 LDD  0,S
 PSHS D
 JSR outdec
 LEAS 2,S
 LDD  #cc51+6
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 LDD  4,S
 LEAS 2,S
 RTS
cc51 FCB 76,69,65,83,32,0,44,83,0
 GLOBAL doublere
doublere
 LDD  #cc53+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc53+7
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc53 FCB 80,83,72,83,32,68,0,65,68,68
 FCB 68,32,44,83,43,43,0
 GLOBAL add
add
 LDD  #cc54+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 ADDD #2
 STD  stkp
 RTS
cc54 FCB 65,68,68,68,32,44,83,43,43,0
 GLOBAL sub
sub
 JSR swapstk
 LDD  #cc55+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 ADDD #2
 STD  stkp
 RTS
cc55 FCB 83,85,66,68,32,44,83,43,43,0
 GLOBAL mult
mult
 LDD  #cc56+0
 PSHS D
 JSR call
 LEAS 2,S
 RTS
cc56 FCB 99,99,109,117,108,116,0
 GLOBAL div
div
 LDD  #cc57+0
 PSHS D
 JSR call
 LEAS 2,S
 RTS
cc57 FCB 99,99,100,105,118,0
 GLOBAL mod
mod
 JSR div
 JSR swap
 RTS
 GLOBAL or
or
 LDD  #cc58+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc58+9
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 ADDD #2
 STD  stkp
 RTS
cc58 FCB 79,82,65,32,32,44,83,43,0,79
 FCB 82,66,32,32,44,83,43,0
 GLOBAL xor
xor
 LDD  #cc59+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc59+9
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 ADDD #2
 STD  stkp
 RTS
cc59 FCB 69,79,82,65,32,44,83,43,0,69
 FCB 79,82,66,32,44,83,43,0
 GLOBAL and
and
 LDD  #cc60+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc60+9
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 ADDD #2
 STD  stkp
 RTS
cc60 FCB 65,78,68,65,32,44,83,43,0,65
 FCB 78,68,66,32,44,83,43,0
 GLOBAL asr
asr
 LDD  #cc61+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc61+5
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc61+14
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc61+22
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc61+31
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc61+40
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 ADDD #2
 STD  stkp
 RTS
cc61 FCB 68,69,67,66,0,66,76,84,32,32
 FCB 42,43,56,0,76,83,82,32,32,44
 FCB 83,0,82,79,82,32,32,49,44,83
 FCB 0,66,82,65,32,32,42,45,55,0
 FCB 80,85,76,83,32,68,0
 GLOBAL asl
asl
 LDD  #cc62+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc62+5
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc62+14
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc62+23
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc62+31
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc62+40
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 ADDD #2
 STD  stkp
 RTS
cc62 FCB 68,69,67,66,0,66,76,84,32,32
 FCB 42,43,56,0,65,83,76,32,32,49
 FCB 44,83,0,82,79,76,32,32,44,83
 FCB 0,66,82,65,32,32,42,45,55,0
 FCB 80,85,76,83,32,68,0
 GLOBAL lognot
lognot
 LDD  #cc63+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc63 FCB 66,69,81,32,32,42,43,55,0
 GLOBAL neg
neg
 JSR com
 LDD  #cc64+0
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc64 FCB 65,68,68,68,32,35,49,0
 GLOBAL com
com
 LDD  #cc65+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc65+5
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc65 FCB 67,79,77,65,0,67,79,77,66,0
 GLOBAL inc
inc
 LDD  #cc66+0
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc66 FCB 65,68,68,68,32,35,49,0
 GLOBAL dec
dec
 LDD  #cc67+0
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc67 FCB 83,85,66,68,32,35,49,0
 GLOBAL restcom
restcom
 LDD  #cc68+0
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc68+8
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  #cc68+17
 PSHS D
 JSR ol
 LEAS 2,S
 LDD  stkp
 ADDD #2
 STD  stkp
 RTS
cc68 FCB 76,68,68,32,32,35,48,0,66,82
 FCB 65,32,32,42,43,53,0,76,68,68
 FCB 32,32,35,49,0
 GLOBAL cmpd
cmpd
 LDD  #cc69+0
 PSHS D
 JSR ol
 LEAS 2,S
 RTS
cc69 FCB 67,77,80,68,32,44,83,43,43,0
 GLOBAL eq
eq
 JSR cmpd
 LDD  #cc70+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc70 FCB 66,69,81,32,32,42,43,55,0
 GLOBAL ne
ne
 JSR cmpd
 LDD  #cc71+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc71 FCB 66,78,69,32,32,42,43,55,0
 GLOBAL lt
lt
 JSR cmpd
 LDD  #cc72+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc72 FCB 66,71,84,32,32,42,43,55,0
 GLOBAL le
le
 JSR cmpd
 LDD  #cc73+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc73 FCB 66,71,69,32,32,42,43,55,0
 GLOBAL gt
gt
 JSR cmpd
 LDD  #cc74+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc74 FCB 66,76,84,32,32,42,43,55,0
 GLOBAL ge
ge
 JSR cmpd
 LDD  #cc75+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc75 FCB 66,76,69,32,32,42,43,55,0
 GLOBAL ult
ult
 JSR cmpd
 LDD  #cc76+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc76 FCB 66,72,73,32,32,42,43,55,0
 GLOBAL ule
ule
 JSR cmpd
 LDD  #cc77+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc77 FCB 66,72,83,32,32,42,43,55,0
 GLOBAL ugt
ugt
 JSR cmpd
 LDD  #cc78+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc78 FCB 66,76,79,32,32,42,43,55,0
 GLOBAL uge
uge
 JSR cmpd
 LDD  #cc79+0
 PSHS D
 JSR ol
 LEAS 2,S
 JSR restcom
 RTS
cc79 FCB 66,76,83,32,32,42,43,55,0
 END
