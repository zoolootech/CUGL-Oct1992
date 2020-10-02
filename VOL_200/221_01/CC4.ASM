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
 EXT addglb
 EXT an
 EXT ch
 EXT comment
 EXT findglb
 EXT gch
 EXT getint
 EXT illname
 EXT inbyte
 EXT inchar
 EXT inline
 EXT kill
 EXT nch
 EXT symname
 EXT cmode
 EXT eof
 EXT errcnt
 EXT lptr
 EXT macptr
 EXT mptr
 EXT output
 EXT pause
 EXT cptr
 EXT line
 EXT macq
 EXT mline
 EXT stage
 EXT stagelas
 EXT stagenex
 NAME keepch
 GLOBAL keepch
keepch
 LDD  #mline
 ADDD mptr
 PSHS D
 LDB  5,S
 SEX
 STB  [,S++]
 LDD  mptr
 PSHS D
 LDD  #80
 PSHS D
 LDD  #1
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 CMPD ,S++
 LBLE cc2
 LDD  mptr
 ADDD #1
 STD  mptr
 SUBD #1
cc2
 LDB  3,S
 SEX
 RTS
 GLOBAL preproce
preproce
 LEAS -12,S
 LDD  cmode
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc3
 LEAS 12,S
 RTS
cc3
 LDD  #0
 STD  lptr
 STD  mptr
cc4
 JSR ch
 CMPD #0
 LBEQ cc5
 JSR ch
 PSHS D
 LDD  #32
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc7
 JSR ch
 PSHS D
 LDD  #9
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc7
 LDD  #0
 JMP cc8
cc7
 LDD  #1
cc8
 CMPD #0
 LBEQ cc6
 LDD  #32
 PSHS D
 JSR keepch
 LEAS 2,S
cc9
 JSR ch
 PSHS D
 LDD  #32
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 PSHS D
 JSR ch
 PSHS D
 LDD  #9
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ORA  ,S+
 ORB  ,S+
 CMPD #0
 LBEQ cc10
 JSR gch
 JMP cc9
cc10
 JMP cc11
cc6
 JSR ch
 PSHS D
 LDD  #34
 CMPD ,S++
 LBNE cc12
 JSR ch
 PSHS D
 JSR keepch
 LEAS 2,S
 JSR gch
cc13
 JSR ch
 PSHS D
 LDD  #34
 CMPD ,S++
 LBEQ cc14
 JSR ch
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc15
 LDD  #cc1+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 JMP cc14
cc15
 JSR gch
 PSHS D
 JSR keepch
 LEAS 2,S
 JMP cc13
cc14
 JSR gch
 LDD  #34
 PSHS D
 JSR keepch
 LEAS 2,S
 JMP cc16
cc12
 JSR ch
 PSHS D
 LDD  #39
 CMPD ,S++
 LBNE cc17
 LDD  #39
 PSHS D
 JSR keepch
 LEAS 2,S
 JSR gch
cc18
 JSR ch
 PSHS D
 LDD  #39
 CMPD ,S++
 LBEQ cc19
 JSR ch
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc20
 LDD  #cc1+14
 PSHS D
 JSR errrpt
 LEAS 2,S
 JMP cc19
cc20
 JSR gch
 PSHS D
 JSR keepch
 LEAS 2,S
 JMP cc18
cc19
 JSR gch
 LDD  #39
 PSHS D
 JSR keepch
 LEAS 2,S
 JMP cc21
cc17
 JSR ch
 PSHS D
 LDD  #47
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc23 _
 LDD  #1
 JSR nch
 PSHS D
 LDD  #42
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc23 _
 LDD  #1
 LDD  #1
cc23
 CMPD #0
 LBEQ cc22
 JSR inchar
 JSR inchar
cc24
 JSR ch
 PSHS D
 LDD  #42
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 PSHS D
 JSR nch
 PSHS D
 LDD  #47
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ANDA ,S+
 ANDB ,S+
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc25
 JSR ch
 CMPD #0
 LBEQ cc26
 JSR inchar
 JMP cc27
cc26
 JSR inline
 LDD  eof
 CMPD #0
 LBEQ cc28
 JMP cc25
cc28
cc27
 JMP cc24
cc25
 JSR inchar
 JSR inchar
 JMP cc29
cc22
 JSR ch
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc30
 LDD  #0
 STD  10,S
cc31
 JSR ch
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc32
 LDD  10,S
 PSHS D
 LDD  #8
 CMPD ,S++
 LBLE cc33
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  12,S
 ADDD #1
 STD  12,S
 SUBD #1
 ADDD ,S++
 PSHS D
 JSR ch
 STB  [,S++]
cc33
 JSR gch
 JMP cc31
cc32
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  12,S
 ADDD ,S++
 PSHS D
 LDD  #0
 STB  [,S++]
 LEAY 10,S
 TFR  Y,D
 PSHS D
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR findmac
 LEAS 2,S
 STD  [,S++]
 CMPD #0
 LBEQ cc34
cc35
 LEAY 9,S
 TFR  Y,D
 PSHS D
 LDD  #macq
 PSHS D
 LDD  14,S
 ADDD #1
 STD  14,S
 SUBD #1
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 CMPD #0
 LBEQ cc36
 LDB  9,S
 SEX
 PSHS D
 JSR keepch
 LEAS 2,S
 JMP cc35
cc36
 JMP cc37
cc34
 LDD  #0
 STD  10,S
cc38
 LEAY 9,S
 TFR  Y,D
 PSHS D
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  14,S
 ADDD #1
 STD  14,S
 SUBD #1
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 CMPD #0
 LBEQ cc39
 LDB  9,S
 SEX
 PSHS D
 JSR keepch
 LEAS 2,S
 JMP cc38
cc39
cc37
 JMP cc40
cc30
 JSR gch
 PSHS D
 JSR keepch
 LEAS 2,S
cc40
cc29
cc21
cc16
cc11
 JMP cc4
cc5
 LDD  #0
 PSHS D
 JSR keepch
 LEAS 2,S
 LDD  mptr
 PSHS D
 LDD  #80
 PSHS D
 LDD  #1
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 CMPD ,S++
 LBGT cc41
 LDD  #cc1+33
 PSHS D
 JSR errrpt
 LEAS 2,S
cc41
 LDD  #0
 STD  mptr
 STD  lptr
cc42
 LDD  #line
 PSHS D
 LDD  lptr
 ADDD #1
 STD  lptr
 SUBD #1
 ADDD ,S++
 PSHS D
 LDD  #mline
 PSHS D
 LDD  mptr
 ADDD #1
 STD  mptr
 SUBD #1
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 CMPD #0
 LBEQ cc43
 JMP cc42
cc43
 LDD  #0
 STD  lptr
 LEAS 12,S
 RTS
cc1 FCB 109,105,115,115,105,110,103,32,113,117
 FCB 111,116,101,0,109,105,115,115,105,110
 FCB 103,32,97,112,111,115,116,114,111,112
 FCB 104,101,0,108,105,110,101,32,116,111
 FCB 111,32,108,111,110,103,0
 GLOBAL addmac
addmac
 LEAS -9,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR symname
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc45
 JSR illname
 JSR kill
 LEAS 9,S
 RTS
cc45
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #5
 PSHS D
 LDD  #0
 PSHS D
 LDD  macptr
 PSHS D
 LDD  #5
 PSHS D
 JSR addglb
 LEAS 10,S
cc46
 JSR ch
 PSHS D
 LDD  #32
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc48
 JSR ch
 PSHS D
 LDD  #9
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc48
 LDD  #0
 JMP cc49
cc48
 LDD  #1
cc49
 CMPD #0
 LBEQ cc47
 JSR gch
 JMP cc46
cc47
cc50
 JSR gch
 PSHS D
 JSR putmac
 LEAS 2,S
 CMPD #0
 LBEQ cc51
 JMP cc50
cc51
 LDD  macptr
 PSHS D
 LDD  #500
 PSHS D
 LDD  #1
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 CMPD ,S++
 LBGT cc52
 LDD  #cc44+0
 PSHS D
 JSR errrpt
 LEAS 2,S
cc52
 LEAS 9,S
 RTS
cc44 FCB 109,97,99,114,111,32,116,97,98,108
 FCB 101,32,102,117,108,108,0
 GLOBAL putmac
putmac
 LDD  #macq
 ADDD macptr
 PSHS D
 LDB  5,S
 SEX
 STB  [,S++]
 LDD  macptr
 PSHS D
 LDD  #500
 PSHS D
 LDD  #1
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 CMPD ,S++
 LBLT cc54
 LDD  macptr
 ADDD #1
 STD  macptr
cc54
 LDB  3,S
 SEX
 RTS
 GLOBAL findmac
findmac
 LDD  2,S
 PSHS D
 JSR findglb
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 BNE  *+8
 LDD  #0
 JMP cc56 _
 LDD  #1
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #5
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc56 _
 LDD  #1
 LDD  #1
cc56
 CMPD #0
 LBEQ cc55
 LDD  cptr
 ADDD #12
 PSHS D
 LDD  #2
 PSHS D
 JSR getint
 LEAS 4,S
 RTS
cc55
 LDD  #0
 RTS
 GLOBAL outbyte
outbyte
 LDB  3,S
 SEX
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc57
 LDD  #0
 RTS
cc57
 LDD  stagenex
 CMPD #0
 LBEQ cc58
 LDD  stagenex
 PSHS D
 LDD  stagelas
 CMPD ,S++
 LBNE cc59
 LDD  #cc53+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 RTS
cc59
 LDD  stagenex
 ADDD #1
 STD  stagenex
 SUBD #1
 PSHS D
 LDB  5,S
 SEX
 STB  [,S++]
cc60
 JMP cc61
cc58
 LDB  3,S
 SEX
 PSHS D
 LDD  output
 PSHS D
 JSR cout
 LEAS 4,S
cc61
 LDB  3,S
 SEX
 RTS
cc53 FCB 115,116,97,103,105,110,103,32,98,117
 FCB 102,102,101,114,32,111,118,101,114,102
 FCB 108,111,119,0
 GLOBAL cout
cout
 LDB  5,S
 SEX
 PSHS D
 LDD  4,S
 PSHS D
 JSR putc
 LEAS 4,S
 PSHS D
 LDD  #1
 COMA
 COMB
 ADDD #1
 CMPD ,S++
 LBNE cc63
 JSR xout
cc63
 RTS
 GLOBAL sout
sout
 LDD  4,S
 PSHS D
 LDD  4,S
 PSHS D
 JSR fputs
 LEAS 4,S
 PSHS D
 LDD  #1
 COMA
 COMB
 ADDD #1
 CMPD ,S++
 LBNE cc64
 JSR xout
cc64
 RTS
 GLOBAL xout
xout
 LDD  #cc62+0
 PSHS D
 LDD  #2
 PSHS D
 JSR fputs
 LEAS 4,S
 LDD  #0
 PSHS D
 JSR exit
 LEAS 2,S
 RTS
cc62 FCB 111,117,116,112,117,116,32,101,114,114
 FCB 111,114,13,0
 GLOBAL fputs
fputs
 LEAS -1,S
cc66
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  7,S
 ADDD #1
 STD  7,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 CMPD #0
 LBEQ cc67
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDB  2,S
 SEX
 PSHS D
 LDD  7,S
 PSHS D
 JSR putc
 LEAS 4,S
 STB  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBLT cc68
 LDB  0,S
 SEX
 LEAS 1,S
 RTS
cc68
 JMP cc66
cc67
 LEAS 1,S
 RTS
 GLOBAL outstr
outstr
 LEAS -2,S
 LDD  #0
 STD  0,S
cc69
 LDD  4,S
 PSHS D
 LDD  2,S
 ADDD #1
 STD  2,S
 SUBD #1
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 JSR outbyte
 LEAS 2,S
 CMPD #0
 LBEQ cc70
 JMP cc69
cc70
 LEAS 2,S
 RTS
 GLOBAL cnl
cnl
 LDD  #13
 PSHS D
 JSR putchar
 LEAS 2,S
 RTS
 GLOBAL nl
nl
 LDD  #13
 PSHS D
 JSR outbyte
 LEAS 2,S
 RTS
 GLOBAL tab
tab
 LDD  #32
 PSHS D
 JSR outbyte
 LEAS 2,S
 RTS
 GLOBAL errrpt
errrpt
 LEAS -2,S
 LDD  #1
 PSHS D
 LDD  output
 CMPD ,S++
 LBEQ cc71
 LDD  #13
 PSHS D
 LDD  #2
 PSHS D
 JSR putc
 LEAS 4,S
 LDD  #line
 PSHS D
 LDD  #2
 PSHS D
 JSR fputs
 LEAS 4,S
 LDD  #cc65+0
 PSHS D
 LDD  #2
 PSHS D
 JSR fputs
 LEAS 4,S
 LDD  4,S
 PSHS D
 LDD  #2
 PSHS D
 JSR fputs
 LEAS 4,S
 LDD  #cc65+9
 PSHS D
 LDD  #2
 PSHS D
 JSR fputs
 LEAS 4,S
cc71
 LDD  pause
 CMPD #0
 LBEQ cc72
 LDD  #cc65+18
 PSHS D
 LDD  #2
 PSHS D
 JSR fputs
 LEAS 4,S
 LDD  #2
 PSHS D
 JSR getc
 LEAS 2,S
 PSHS D
 LDD  #3
 CMPD ,S++
 LBNE cc73
 LDD  #0
 PSHS D
 JSR exit
 LEAS 2,S
cc73
cc72
 JSR comment
 LDD  #line
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 JSR comment
 LDD  #0
 STD  0,S
cc74
 LDD  0,S
 PSHS D
 LDD  lptr
 CMPD ,S++
 LBLE cc75
 LDD  #line
 PSHS D
 LDD  2,S
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #9
 CMPD ,S++
 LBNE cc76
 JSR tab
 JMP cc77
cc76
 LDD  #32
 PSHS D
 JSR outbyte
 LEAS 2,S
cc77
 LDD  0,S
 ADDD #1
 STD  0,S
 JMP cc74
cc75
 LDD  #94
 PSHS D
 JSR outbyte
 LEAS 2,S
 JSR nl
 JSR comment
 LDD  #cc65+22
 PSHS D
 JSR outstr
 LEAS 2,S
 LDD  4,S
 PSHS D
 JSR outstr
 LEAS 2,S
 LDD  #cc65+31
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 LDD  errcnt
 ADDD #1
 STD  errcnt
 LEAS 2,S
 RTS
cc65 FCB 13,42,42,42,42,42,32,32,0,32
 FCB 32,42,42,42,42,42,13,0,13,63
 FCB 32,0,42,42,42,42,42,42,32,32
 FCB 0,32,32,42,42,42,42,42,42,0
 GLOBAL ol
ol
 LDD  2,S
 PSHS D
 JSR ot
 LEAS 2,S
 JSR nl
 RTS
 GLOBAL ot
ot
 JSR tab
 LDD  2,S
 PSHS D
 JSR outstr
 LEAS 2,S
 RTS
 GLOBAL streq
streq
 LEAS -2,S
 LDD  #0
 STD  0,S
cc79
 LDD  4,S
 PSHS D
 LDB  [,S++]
 SEX
 CMPD #0
 LBEQ cc80
 LDD  6,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  6,S
 PSHS D
 LDB  [,S++]
 SEX
 CMPD ,S++
 LBEQ cc81
 LDD  #0
 LEAS 2,S
 RTS
cc81
 LDD  0,S
 ADDD #1
 STD  0,S
 LDD  6,S
 ADDD #1
 STD  6,S
 LDD  4,S
 ADDD #1
 STD  4,S
 JMP cc79
cc80
 LDD  0,S
 LEAS 2,S
 RTS
 GLOBAL astreq
astreq
 LEAS -2,S
 LDD  #0
 STD  0,S
cc82
 LDD  0,S
 PSHS D
 LDD  6,S
 CMPD ,S++
 LBLE cc83
 LDD  8,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  8,S
 PSHS D
 LDB  [,S++]
 SEX
 CMPD ,S++
 LBEQ cc84
 JMP cc83
cc84
 LDD  8,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc85
 JMP cc83
cc85
 LDD  6,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc86
 JMP cc83
cc86
 LDD  0,S
 ADDD #1
 STD  0,S
 LDD  8,S
 ADDD #1
 STD  8,S
 LDD  6,S
 ADDD #1
 STD  6,S
 JMP cc82
cc83
 LDD  8,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc87
 LDD  #0
 LEAS 2,S
 RTS
cc87
 LDD  6,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc88
 LDD  #0
 LEAS 2,S
 RTS
cc88
 LDD  0,S
 LEAS 2,S
 RTS
 GLOBAL match
match
 LEAS -2,S
 JSR blanks
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  8,S
 PSHS D
 JSR streq
 LEAS 4,S
 STD  [,S++]
 CMPD #0
 LBEQ cc89
 LDD  lptr
 PSHS D
 LDD  2,S
 ADDD ,S++
 STD  lptr
 LDD  #1
 LEAS 2,S
 RTS
cc89
 LDD  #0
 LEAS 2,S
 RTS
 GLOBAL amatch
amatch
 LEAS -2,S
 JSR blanks
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  10,S
 PSHS D
 LDD  10,S
 PSHS D
 JSR astreq
 LEAS 6,S
 STD  [,S++]
 CMPD #0
 LBEQ cc90
 LDD  lptr
 PSHS D
 LDD  2,S
 ADDD ,S++
 STD  lptr
cc91
 JSR ch
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc92
 JSR inbyte
 JMP cc91
cc92
 LDD  #1
 LEAS 2,S
 RTS
cc90
 LDD  #0
 LEAS 2,S
 RTS
 GLOBAL blanks
blanks
cc93
 LDD  #1
 CMPD #0
 LBEQ cc94
cc95
 JSR ch
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc96
 JSR inline
 JSR preproce
 LDD  eof
 CMPD #0
 LBEQ cc97
 JMP cc96
cc97
 JMP cc95
cc96
 JSR ch
 PSHS D
 LDD  #32
 CMPD ,S++
 LBNE cc98
 JSR gch
 JMP cc99
cc98
 JSR ch
 PSHS D
 LDD  #9
 CMPD ,S++
 LBNE cc100
 JSR gch
 JMP cc101
cc100
 RTS
cc101
cc99
 JMP cc93
cc94
 RTS
 GLOBAL PDBody
PDBody
 LEAS -1,S
 LDD  3,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBEQ cc102
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  5,S
 PSHS D
 LDD  #10
 PULS X
 JSR ccdiv
 EXG  D,X
 ADDD #48
 STB  [,S++]
 LDD  3,S
 PSHS D
 LDD  #10
 PULS X
 JSR ccdiv
 PSHS D
 JSR PDBody
 LEAS 2,S
 LDB  0,S
 SEX
 PSHS D
 JSR outbyte
 LEAS 2,S
cc102
 LEAS 1,S
 RTS
 GLOBAL setstage
setstage
 LDD  4,S
 PSHS D
 LDD  stagenex
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc103
 LDD  #stage
 STD  stagenex
cc103
 LDD  2,S
 PSHS D
 LDD  stagenex
 STD  [,S++]
 RTS
 GLOBAL clearsta
clearsta
 LDD  stagenex
 PSHS D
 LDD  #0
 STB  [,S++]
 LDD  4,S
 STD  stagenex
 CMPD #0
 LBEQ cc104
 RTS
cc104
 LDD  2,S
 CMPD #0
 LBEQ cc105
 LDD  2,S
 PSHS D
 LDD  output
 PSHS D
 JSR sout
 LEAS 4,S
cc105
 RTS
 GLOBAL outdec
outdec
 LDD  2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc106
 LDD  #48
 PSHS D
 JSR outbyte
 LEAS 2,S
 JMP cc107
cc106
 LDD  2,S
 PSHS D
 LDD  #32768
 COMA
 COMB
 ADDD #1
 CMPD ,S++
 LBNE cc108
 LDD  #cc78+0
 PSHS D
 JSR outstr
 LEAS 2,S
 JMP cc109
cc108
 LDD  2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBLE cc110
 LDD  #45
 PSHS D
 JSR outbyte
 LEAS 2,S
 LDD  2,S
 COMA
 COMB
 ADDD #1
 PSHS D
 JSR PDBody
 LEAS 2,S
 JMP cc111
cc110
 LDD  2,S
 PSHS D
 JSR PDBody
 LEAS 2,S
cc111
cc109
cc107
 RTS
cc78 FCB 45,51,50,55,54,56,0
 END