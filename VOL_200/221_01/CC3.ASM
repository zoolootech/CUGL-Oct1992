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
 EXT astreq
 EXT blanks
 EXT call
 EXT callstk
 EXT cnl
 EXT comment
 EXT cout
 EXT errrpt
 EXT expressi
 EXT findmac
 EXT immed
 EXT match
 EXT modstk
 EXT nl
 EXT outdec
 EXT outstr
 EXT preproce
 EXT push
 EXT sout
 EXT streq
 EXT swapstk
 EXT cmode
 EXT ctext
 EXT eof
 EXT iflevel
 EXT input
 EXT input2
 EXT lptr
 EXT nxtlab
 EXT output
 EXT skipleve
 EXT stkp
 EXT wq
 EXT wqptr
 EXT cptr
 EXT endsearc
 EXT glbptr
 EXT line
 EXT locptr
 EXT startcom
 EXT statptr
 EXT stattab
 EXT symtab
 NAME callfunc
 GLOBAL callfunc
callfunc
 LEAS -2,S
 LDD  #0
 STD  0,S
 JSR blanks
 LDD  4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc2
 JSR push
cc2
cc3
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc1+0
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc4
 JSR endst
 CMPD #0
 LBEQ cc5
 JMP cc4
cc5
 JSR expressi
 LDD  4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc6
 JSR swapstk
cc6
 JSR push
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  2,S
 ADDD #2
 STD  [,S++]
 LDD  #cc1+2
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc7
 JMP cc4
cc7
 JMP cc3
cc4
 LDD  #cc1+4
 PSHS D
 JSR needbrac
 LEAS 2,S
 LDD  4,S
 PSHS D
 LDD  #cc1+6
 PSHS D
 JSR streq
 LEAS 4,S
 CMPD #0
 LBEQ cc8
 JSR immed
 LDD  0,S
 PSHS D
 LDD  #1
 DECB
 BLT  *+8
 LSR  ,S
 ROR  1,S
 BRA  *-7
 PULS D
 PSHS D
 JSR outdec
 LEAS 2,S
 JSR nl
cc8
 LDD  4,S
 CMPD #0
 LBEQ cc9
 LDD  4,S
 PSHS D
 JSR call
 LEAS 2,S
 JMP cc10
cc9
 JSR callstk
cc10
 LDD  stkp
 PSHS D
 LDD  2,S
 ADDD ,S++
 PSHS D
 JSR modstk
 LEAS 2,S
 STD  stkp
 LEAS 2,S
 RTS
cc1 FCB 41,0,44,0,41,0,112,114,105,110
 FCB 116,102,0
 GLOBAL junk
junk
 JSR inbyte
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc12
cc13
 JSR ch
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc14
 JSR gch
 JMP cc13
cc14
 JMP cc15
cc12
cc16
 JSR ch
 PSHS D
 JSR an
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc17
 JSR ch
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc18
 JMP cc17
cc18
 JSR gch
 JMP cc16
cc17
cc15
 JSR blanks
 RTS
 GLOBAL endst
endst
 JSR blanks
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc11+0
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 JSR ch
 PSHS D
 LDD  #0
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ORA  ,S+
 ORB  ,S+
 RTS
cc11 FCB 59,0
 GLOBAL illname
illname
 LDD  #cc19+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 JSR junk
 RTS
cc19 FCB 105,108,108,101,103,97,108,32,115,121
 FCB 109,98,111,108,32,110,97,109,101,0
 GLOBAL multidef
multidef
 LDD  #cc20+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 JSR comment
 LDD  2,S
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 RTS
cc20 FCB 97,108,114,101,97,100,121,32,100,101
 FCB 102,105,110,101,100,0
 GLOBAL needbrac
needbrac
 LDD  2,S
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc22
 LDD  #cc21+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 JSR comment
 LDD  2,S
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
cc22
 RTS
cc21 FCB 109,105,115,115,105,110,103,32,116,111
 FCB 107,101,110,0
 GLOBAL needlval
needlval
 LDD  #cc23+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 RTS
cc23 FCB 109,117,115,116,32,98,101,32,108,118
 FCB 97,108,117,101,0
 GLOBAL hash
hash
 LEAS -4,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 LDB  [,S++]
 SEX
 STD  [,S++]
cc25
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 ADDD #1
 STD  8,S
 PSHS D
 LDB  [,S++]
 SEX
 STD  [,S++]
 CMPD #0
 LBEQ cc26
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  4,S
 PSHS D
 LDD  #1
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 PSHS D
 LDD  4,S
 ADDD ,S++
 STD  [,S++]
 JMP cc25
cc26
 LDD  2,S
 LEAS 4,S
 RTS
 GLOBAL findstat
findstat
 LEAS -2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  statptr
 STD  [,S++]
cc27
 LDD  0,S
 PSHS D
 LDD  #stattab
 CMPD ,S++
 LBHI cc28
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  2,S
 PSHS D
 LDD  #23
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  [,S++]
 LDD  4,S
 PSHS D
 LDD  2,S
 PSHS D
 LDD  #8
 PSHS D
 JSR astreq
 LEAS 6,S
 CMPD #0
 LBEQ cc29
 LDD  0,S
 LEAS 2,S
 RTS
cc29
 JMP cc27
cc28
 LDD  #0
 LEAS 2,S
 RTS
 GLOBAL findglb
findglb
 LEAS -2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  6,S
 PSHS D
 JSR hash
 LEAS 2,S
 PSHS D
 LDD  #511
 ANDA ,S+
 ANDB ,S+
 STD  [,S++]
 LDD  #symtab
 PSHS D
 LDD  2,S
 PSHS D
 LDD  #14
 PULS X
 JSR ccmult
 ADDD ,S++
 STD  cptr
cc30
 LDD  4,S
 PSHS D
 LDD  cptr
 PSHS D
 LDD  #8
 PSHS D
 JSR astreq
 LEAS 6,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc31
 LDD  cptr
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc32
 LDD  #0
 LEAS 2,S
 RTS
cc32
 LDD  cptr
 ADDD #14
 STD  cptr
 LDD  cptr
 PSHS D
 LDD  #symtab
 PSHS D
 LDD  #512
 PSHS D
 LDD  #14
 PULS X
 JSR ccmult
 ADDD ,S++
 CMPD ,S++
 LBHI cc33
 LDD  #symtab
 STD  cptr
cc33
 JMP cc30
cc31
 LDD  cptr
 LEAS 2,S
 RTS
 GLOBAL findloc
findloc
 LEAS -2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  locptr
 STD  [,S++]
cc34
 LDD  0,S
 PSHS D
 LDD  endsearc
 CMPD ,S++
 LBHS cc35
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  2,S
 PSHS D
 LDD  #14
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  [,S++]
 LDD  4,S
 PSHS D
 LDD  2,S
 PSHS D
 LDD  #8
 PSHS D
 JSR astreq
 LEAS 6,S
 CMPD #0
 LBEQ cc36
 LDD  0,S
 LEAS 2,S
 RTS
cc36
 JMP cc34
cc35
 LDD  #0
 LEAS 2,S
 RTS
 GLOBAL addstati
addstati
 LEAS -2,S
 LDD  startcom
 STD  endsearc
 LDD  12,S
 PSHS D
 JSR findloc
 LEAS 2,S
 CMPD #0
 LBEQ cc37
 LDD  statptr
 LEAS 2,S
 RTS
cc37
 LDD  #symtab
 PSHS D
 LDD  #512
 PSHS D
 LDD  #14
 PULS X
 JSR ccmult
 ADDD ,S++
 ADDD #14
 STD  endsearc
 LDD  statptr
 PSHS D
 LDD  #stattab
 ADDD #690
 CMPD ,S++
 LBHI cc38
 LDD  #cc24+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 LEAS 2,S
 RTS
cc38
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  statptr
 STD  [,S++]
cc39
 LDD  0,S
 ADDD #1
 STD  0,S
 SUBD #1
 PSHS D
 LDD  14,S
 ADDD #1
 STD  14,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc40
 JMP cc39
cc40
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  statptr
 ADDD #14
 STD  [,S++]
cc41
 LDD  0,S
 ADDD #1
 STD  0,S
 SUBD #1
 PSHS D
 LDD  12,S
 ADDD #1
 STD  12,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc42
 JMP cc41
cc42
 LDD  statptr
 ADDD #9
 PSHS D
 LDB  11,S
 SEX
 STB  [,S++]
 LDD  statptr
 ADDD #10
 PSHS D
 LDB  9,S
 SEX
 STB  [,S++]
 LDD  statptr
 ADDD #11
 PSHS D
 LDD  #1
 STB  [,S++]
 LDD  4,S
 PSHS D
 LDD  statptr
 ADDD #12
 PSHS D
 LDD  #2
 PSHS D
 JSR putint
 LEAS 6,S
 LDD  statptr
 ADDD #23
 STD  statptr
 LDD  statptr
 LEAS 2,S
 RTS
cc24 FCB 115,116,97,116,105,99,32,115,121,109
 FCB 98,111,108,32,116,97,98,108,101,32
 FCB 111,118,101,114,102,108,111,119,0
 GLOBAL addglb
addglb
 LEAS -2,S
 LDD  12,S
 PSHS D
 JSR findglb
 LEAS 2,S
 CMPD #0
 LBEQ cc44
 LDD  cptr
 LEAS 2,S
 RTS
cc44
 LDD  glbptr
 PSHS D
 LDD  #symtab
 PSHS D
 LDD  #512
 PSHS D
 LDD  #14
 PULS X
 JSR ccmult
 ADDD ,S++
 CMPD ,S++
 LBHI cc45
 LDD  #cc43+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 LEAS 2,S
 RTS
cc45
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  cptr
 STD  [,S++]
cc46
 LDD  0,S
 ADDD #1
 STD  0,S
 SUBD #1
 PSHS D
 LDD  14,S
 ADDD #1
 STD  14,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc47
 JMP cc46
cc47
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  13,S
 SEX
 STB  [,S++]
 LDD  cptr
 ADDD #10
 PSHS D
 LDB  11,S
 SEX
 STB  [,S++]
 LDD  cptr
 ADDD #11
 PSHS D
 LDD  6,S
 STB  [,S++]
 LDD  6,S
 PSHS D
 LDD  cptr
 ADDD #12
 PSHS D
 LDD  #2
 PSHS D
 JSR putint
 LEAS 6,S
 LDD  glbptr
 ADDD #14
 STD  glbptr
 LDD  cptr
 LEAS 2,S
 RTS
cc43 FCB 103,108,111,98,97,108,32,115,121,109
 FCB 98,111,108,32,116,97,98,108,101,32
 FCB 111,118,101,114,102,108,111,119,0
 GLOBAL addloc
addloc
 LEAS -2,S
 LDD  startcom
 STD  endsearc
 LDD  10,S
 PSHS D
 JSR findloc
 LEAS 2,S
 CMPD #0
 LBEQ cc49
 LDD  statptr
 LEAS 2,S
 RTS
cc49
 LDD  #symtab
 PSHS D
 LDD  #512
 PSHS D
 LDD  #14
 PULS X
 JSR ccmult
 ADDD ,S++
 ADDD #14
 STD  endsearc
 LDD  locptr
 PSHS D
 LDD  #symtab
 ADDD #8008
 PSHS D
 LDD  #14
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 CMPD ,S++
 LBHI cc50
 LDD  #cc48+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 LEAS 2,S
 RTS
cc50
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  locptr
 STD  [,S++]
 STD  cptr
cc51
 LDD  0,S
 ADDD #1
 STD  0,S
 SUBD #1
 PSHS D
 LDD  12,S
 ADDD #1
 STD  12,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc52
 JMP cc51
cc52
 LDB  9,S
 SEX
 PSHS D
 LDD  #1
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc54 _
 LDD  #1
 LDB  7,S
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
 JMP cc54 _
 LDD  #1
 LDD  #1
cc54
 CMPD #0
 LBEQ cc53
 LDD  4,S
 SUBD #1
 STD  4,S
cc53
 LDD  cptr
 ADDD #9
 PSHS D
 LDB  11,S
 SEX
 STB  [,S++]
 LDD  cptr
 ADDD #10
 PSHS D
 LDB  9,S
 SEX
 STB  [,S++]
 LDD  cptr
 ADDD #11
 PSHS D
 LDD  #2
 STB  [,S++]
 LDD  4,S
 PSHS D
 LDD  cptr
 ADDD #12
 PSHS D
 LDD  #2
 PSHS D
 JSR putint
 LEAS 6,S
 LDD  locptr
 ADDD #14
 STD  locptr
 LDD  cptr
 LEAS 2,S
 RTS
cc48 FCB 108,111,99,97,108,32,115,121,109,98
 FCB 111,108,32,116,97,98,108,101,32,111
 FCB 118,101,114,102,108,111,119,0
 GLOBAL getint
getint
 LEAS -2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 LDD  8,S
 SUBD #1
 STD  8,S
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 STD  [,S++]
cc56
 LDD  4,S
 SUBD #1
 STD  4,S
 ADDD #1
 CMPD #0
 LBEQ cc57
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  2,S
 PSHS D
 LDD  #8
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 PSHS D
 LDD  10,S
 PSHS D
 LDD  10,S
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #255
 ANDA ,S+
 ANDB ,S+
 ORA  ,S+
 ORB  ,S+
 STD  [,S++]
 JMP cc56
cc57
 LDD  0,S
 LEAS 2,S
 RTS
 GLOBAL putint
putint
cc58
 LDD  2,S
 SUBD #1
 STD  2,S
 ADDD #1
 CMPD #0
 LBEQ cc59
 LDD  4,S
 ADDD #1
 STD  4,S
 SUBD #1
 PSHS D
 LDD  8,S
 STB  [,S++]
 LEAY 6,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 PSHS D
 LDD  #8
 DECB
 BLT  *+8
 LSR  ,S
 ROR  1,S
 BRA  *-7
 PULS D
 STD  [,S++]
 JMP cc58
cc59
 RTS
 GLOBAL symname
symname
 JSR blanks
 JSR ch
 PSHS D
 JSR alpha
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc60
 LDD  #0
 RTS
cc60
cc61
 JSR ch
 PSHS D
 JSR an
 LEAS 2,S
 CMPD #0
 LBEQ cc62
 LDD  2,S
 ADDD #1
 STD  2,S
 SUBD #1
 PSHS D
 JSR gch
 STB  [,S++]
 JMP cc61
cc62
 LDD  2,S
 PSHS D
 LDD  #0
 STB  [,S++]
 LDD  #1
 RTS
 GLOBAL getlabel
getlabel
 LDD  nxtlab
 ADDD #1
 STD  nxtlab
 RTS
 GLOBAL postlabe
postlabe
 LDD  2,S
 PSHS D
 JSR printlab
 LEAS 2,S
 JSR nl
 RTS
 GLOBAL printlab
printlab
 LDD  #cc55+0
 PSHS D
 JSR outstr
 LEAS 2,S
 LDD  2,S
 PSHS D
 JSR outdec
 LEAS 2,S
 RTS
cc55 FCB 99,99,0
 GLOBAL alpha
alpha
 LEAY 3,S
 TFR  Y,D
 PSHS D
 LDB  5,S
 SEX
 PSHS D
 LDD  #127
 ANDA ,S+
 ANDB ,S+
 STB  [,S++]
 LDB  3,S
 SEX
 PSHS D
 LDD  #97
 CMPD ,S++
 LBGT cc64
 LDB  3,S
 SEX
 PSHS D
 LDD  #122
 CMPD ,S++
 BGE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 RTS
cc64
 LDB  3,S
 SEX
 PSHS D
 LDD  #90
 CMPD ,S++
 LBLT cc65
 LDB  3,S
 SEX
 PSHS D
 LDD  #65
 CMPD ,S++
 BLE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 RTS
cc65
 LDB  3,S
 SEX
 PSHS D
 LDD  #95
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 RTS
 GLOBAL numeric
numeric
 LEAY 3,S
 TFR  Y,D
 PSHS D
 LDB  5,S
 SEX
 PSHS D
 LDD  #127
 ANDA ,S+
 ANDB ,S+
 STB  [,S++]
 LDB  3,S
 SEX
 PSHS D
 LDD  #48
 CMPD ,S++
 LBGT cc66
 LDB  3,S
 SEX
 PSHS D
 LDD  #57
 CMPD ,S++
 BGE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 RTS
cc66
 LDD  #0
 RTS
 GLOBAL an
an
 LDB  3,S
 SEX
 PSHS D
 JSR alpha
 LEAS 2,S
 CMPD #0
 LBEQ cc67
 LDD  #1
 RTS
cc67
 LDB  3,S
 SEX
 PSHS D
 JSR numeric
 LEAS 2,S
 RTS
 GLOBAL pl
pl
 JSR cnl
cc68
 LDD  2,S
 PSHS D
 LDB  [,S++]
 SEX
 CMPD #0
 LBEQ cc69
 LDD  2,S
 ADDD #1
 STD  2,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 JSR putchar
 LEAS 2,S
 JMP cc68
cc69
 RTS
 GLOBAL addwhile
addwhile
 LEAS -2,S
 LDD  4,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  stkp
 STD  [,S++]
 LDD  4,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 JSR getlabel
 STD  [,S++]
 LDD  4,S
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 JSR getlabel
 STD  [,S++]
 LDD  wqptr
 PSHS D
 LDD  #wq
 ADDD #100
 PSHS D
 LDD  #4
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 CMPD ,S++
 LBNE cc70
 LDD  #cc63+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 PSHS D
 JSR exit
 LEAS 2,S
cc70
 LDD  #0
 STD  0,S
cc71
 LDD  0,S
 PSHS D
 LDD  #4
 CMPD ,S++
 LBLE cc72
 LDD  wqptr
 ADDD #1
 ADDD #1
 STD  wqptr
 SUBD #1
 SUBD #1
 PSHS D
 LDD  6,S
 PSHS D
 LDD  4,S
 ADDD #1
 STD  4,S
 SUBD #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 JMP cc71
cc72
 LEAS 2,S
 RTS
cc63 FCB 116,111,111,32,109,97,110,121,32,97
 FCB 99,116,105,118,101,32,119,104,105,108
 FCB 101,115,0
 GLOBAL delwhile
delwhile
 JSR readwhil
 CMPD #0
 LBEQ cc74
 LDD  wqptr
 PSHS D
 LDD  #4
 PSHS D
 ADDD ,S++
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  wqptr
cc74
 RTS
 GLOBAL readwhil
readwhil
 LDD  wqptr
 PSHS D
 LDD  #wq
 CMPD ,S++
 LBNE cc75
 LDD  #cc73+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 RTS
cc75
 LDD  wqptr
 PSHS D
 LDD  #4
 PSHS D
 ADDD ,S++
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 RTS
cc76
 RTS
cc73 FCB 110,111,32,97,99,116,105,118,101,32
 FCB 119,104,105,108,101,115,0
 GLOBAL ch
ch
 LDD  #line
 ADDD lptr
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #127
 ANDA ,S+
 ANDB ,S+
 RTS
 GLOBAL nch
nch
 JSR ch
 CMPD #0
 LBEQ cc78
 LDD  #line
 PSHS D
 LDD  lptr
 ADDD #1
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #127
 ANDA ,S+
 ANDB ,S+
 RTS
cc78
 LDD  #0
 RTS
 GLOBAL gch
gch
 JSR ch
 CMPD #0
 LBEQ cc79
 LDD  #line
 PSHS D
 LDD  lptr
 ADDD #1
 STD  lptr
 SUBD #1
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #127
 ANDA ,S+
 ANDB ,S+
 RTS
cc79
 LDD  #0
 RTS
 GLOBAL kill
kill
 LDD  #0
 STD  lptr
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #0
 STB  [,S++]
 RTS
 GLOBAL inbyte
inbyte
cc80
 JSR ch
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc81
 JSR inline
 LDD  eof
 CMPD #0
 LBEQ cc82
 LDD  #0
 RTS
cc82
 JSR preproce
 JMP cc80
cc81
 JSR gch
 RTS
 GLOBAL inchar
inchar
 JSR ch
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc83
 JSR inline
cc83
 LDD  eof
 CMPD #0
 LBEQ cc84
 LDD  #0
 RTS
cc84
 JSR gch
 RTS
 GLOBAL inline
inline
 LEAS -4,S
cc85
 LDD  #1
 CMPD #0
 LBEQ cc86
 LDD  input
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc87
 LDD  #1
 STD  eof
cc87
 LDD  eof
 CMPD #0
 LBEQ cc88
 LEAS 4,S
 RTS
cc88
 JSR CCPOLL
 CMPD #0
 LBEQ cc89
 LDD  #2
 PSHS D
 JSR getc
 LEAS 2,S
 PSHS D
 LDD  #3
 CMPD ,S++
 LBNE cc90
 LDD  #0
 PSHS D
 JSR exit
 LEAS 2,S
cc90
cc89
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  input2
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc91
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  input
 STD  [,S++]
cc91
 JSR kill
cc92
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  2,S
 PSHS D
 JSR getc
 LEAS 2,S
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBGE cc93
 LDD  2,S
 PSHS D
 LDD  #13
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc95
 LDD  lptr
 PSHS D
 LDD  #80
 PSHS D
 LDD  #1
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 CMPD ,S++
 BLE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc95
 LDD  #0
 JMP cc96
cc95
 LDD  #1
cc96
 CMPD #0
 LBEQ cc94
 JMP cc93
cc94
 LDD  #line
 PSHS D
 LDD  lptr
 ADDD #1
 STD  lptr
 SUBD #1
 ADDD ,S++
 PSHS D
 LDD  4,S
 STB  [,S++]
 JMP cc92
cc93
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #0
 STB  [,S++]
 LDD  2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBLT cc97
 LDD  0,S
 PSHS D
 JSR fclose
 LEAS 2,S
 LDD  input2
 CMPD #0
 LBEQ cc98
 LDD  #0
 STD  input2
 JMP cc99
cc98
 LDD  #0
 STD  input
cc99
cc97
 LDD  lptr
 CMPD #0
 LBEQ cc100
 LDD  ctext
 CMPD #0
 LBEQ cc102 _
 LDD  cmode
 CMPD #0
 LBEQ cc102 _
 LDD  #1
cc102
 CMPD #0
 LBEQ cc101
 LDD  #59
 PSHS D
 LDD  output
 PSHS D
 JSR cout
 LEAS 4,S
 LDD  #line
 PSHS D
 LDD  output
 PSHS D
 JSR sout
 LEAS 4,S
 LDD  #13
 PSHS D
 LDD  output
 PSHS D
 JSR cout
 LEAS 4,S
cc101
 LDD  #0
 STD  lptr
 LDD  #cc77+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc103
 LDD  iflevel
 ADDD #1
 STD  iflevel
 LDD  skipleve
 CMPD #0
 LBEQ cc104
 JMP cc85
cc104
 JSR blanks
 LDD  #line
 ADDD lptr
 PSHS D
 JSR findmac
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc105
 LDD  iflevel
 STD  skipleve
cc105
 JMP cc85
cc103
 LDD  #cc77+7
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc106
 LDD  iflevel
 ADDD #1
 STD  iflevel
 LDD  skipleve
 CMPD #0
 LBEQ cc107
 JMP cc85
cc107
 JSR blanks
 LDD  #line
 ADDD lptr
 PSHS D
 JSR findmac
 LEAS 2,S
 CMPD #0
 LBEQ cc108
 LDD  iflevel
 STD  skipleve
cc108
 JMP cc85
cc106
 LDD  #cc77+15
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc109
 LDD  iflevel
 CMPD #0
 LBEQ cc110
 LDD  skipleve
 PSHS D
 LDD  iflevel
 CMPD ,S++
 LBNE cc111
 LDD  #0
 STD  skipleve
 JMP cc112
cc111
 LDD  skipleve
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc113
 LDD  iflevel
 STD  skipleve
cc113
cc112
 JMP cc114
cc110
 LDD  #cc77+21
 PSHS D
 JSR errrpt
 LEAS 2,S
cc114
 JMP cc85
cc109
 LDD  #cc77+40
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc115
 LDD  iflevel
 CMPD #0
 LBEQ cc116
 LDD  skipleve
 PSHS D
 LDD  iflevel
 CMPD ,S++
 LBNE cc117
 LDD  #0
 STD  skipleve
cc117
 LDD  iflevel
 SUBD #1
 STD  iflevel
 JMP cc118
cc116
 LDD  #cc77+47
 PSHS D
 JSR errrpt
 LEAS 2,S
cc118
 JMP cc85
cc115
 LDD  skipleve
 CMPD #0
 LBEQ cc119
 JMP cc85
cc119
 JMP cc86
cc100
 JMP cc85
cc86
 LEAS 4,S
 RTS
cc77 FCB 35,105,102,100,101,102,0,35,105,102
 FCB 110,100,101,102,0,35,101,108,115,101
 FCB 0,110,111,32,109,97,116,99,104,105
 FCB 110,103,32,35,105,102,46,46,46,0
 FCB 35,101,110,100,105,102,0,110,111,32
 FCB 109,97,116,99,104,105,110,103,32,35
 FCB 105,102,46,46,46,0
 END
