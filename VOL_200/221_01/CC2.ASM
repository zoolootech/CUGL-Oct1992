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
 EXT addloc
 EXT addwhile
 EXT amatch
 EXT blanks
 EXT ch
 EXT comment
 EXT constexp
 EXT cout
 EXT debug
 EXT declloc
 EXT defname
 EXT defstora
 EXT delwhile
 EXT doexpres
 EXT dumplits
 EXT endst
 EXT entry
 EXT errrpt
 EXT findglb
 EXT findloc
 EXT gch
 EXT getlabel
 EXT getint
 EXT illname
 EXT inbyte
 EXT inchar
 EXT inline
 EXT junk
 EXT jump
 EXT kill
 EXT match
 EXT modstk
 EXT multidef
 EXT needbrac
 EXT nextsym
 EXT nl
 EXT number
 EXT outbyte
 EXT outdec
 EXT outstr
 EXT postlabe
 EXT printlab
 EXT putint
 EXT readwhil
 EXT ret
 EXT sout
 EXT streq
 EXT sw
 EXT symname
 EXT tab
 EXT test
 EXT argstk
 EXT ctext
 EXT cmode
 EXT DEFDEBUG
 EXT declared
 EXT declstat
 EXT eof
 EXT first_fu
 EXT lastst
 EXT litlab
 EXT litptr
 EXT lptr
 EXT monitor
 EXT ncmp
 EXT startcom
 EXT statlab
 EXT stdecl
 EXT stkp
 EXT stlab
 EXT swactive
 EXT swdefaul
 EXT swnext
 EXT swend
 EXT cptr
 EXT cptr2
 EXT line
 EXT locptr
 EXT glbptr
 EXT statptr
 EXT symtab
 NAME needsub
 GLOBAL needsub
needsub
 LEAS -2,S
 LDD  #cc1+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc2
 LDD  #0
 LEAS 2,S
 RTS
cc2
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR number
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc3
 LDD  #cc1+2
 PSHS D
 JSR errrpt
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #1
 STD  [,S++]
cc3
 LEAY 0,S
 TFR  Y,D
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
 LBLE cc4
 LDD  #cc1+19
 PSHS D
 JSR errrpt
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 COMA
 COMB
 ADDD #1
 STD  [,S++]
cc4
 LDD  #cc1+41
 PSHS D
 JSR needbrac
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #0
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 LEAS 2,S
 RTS
cc1 FCB 93,0,109,117,115,116,32,98,101,32
 FCB 99,111,110,115,116,97,110,116,0,110
 FCB 101,103,97,116,105,118,101,32,115,105
 FCB 122,101,32,105,108,108,101,103,97,108
 FCB 0,93,0
 GLOBAL newfunc
newfunc
 LEAS -13,S
 LDD  monitor
 CMPD #0
 LBEQ cc6
 LDD  #13
 PSHS D
 LDD  #2
 PSHS D
 JSR cout
 LEAS 4,S
 LDD  #line
 PSHS D
 LDD  #2
 PSHS D
 JSR sout
 LEAS 4,S
cc6
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR symname
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc7
 LDD  #cc5+0
 PSHS D
 JSR errrpt
 LEAS 2,S
 JSR kill
 LEAS 13,S
 RTS
cc7
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LEAY 4,S
 TFR  Y,D
 PSHS D
 JSR findglb
 LEAS 2,S
 STD  [,S++]
 CMPD #0
 LBEQ cc8
 LDD  0,S
 ADDD #9
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #4
 CMPD ,S++
 LBEQ cc9
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR multidef
 LEAS 2,S
 JMP cc10
cc9
 LDD  0,S
 ADDD #12
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #4
 CMPD ,S++
 LBNE cc11
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR multidef
 LEAS 2,S
 JMP cc12
cc11
 LDD  0,S
 ADDD #12
 PSHS D
 LDD  #4
 STB  [,S++]
cc12
cc10
 JMP cc13
cc8
 LEAY 2,S
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
 LDD  #4
 PSHS D
 JSR addglb
 LEAS 8,S
cc13
 LDD  #cc5+32
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc14
 LDD  #cc5+34
 PSHS D
 JSR errrpt
 LEAS 2,S
cc14
 LDD  first_fu
 CMPD #0
 LBEQ cc15
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR defname
 LEAS 2,S
 LDD  #0
 STD  first_fu
cc15
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  17,S
 PSHS D
 JSR entry
 LEAS 4,S
 LDD  DEFDEBUG
 CMPD #0
 LBEQ cc16
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR debug
 LEAS 2,S
cc16
 LDD  #symtab
 PSHS D
 LDD  #512
 PSHS D
 LDD  #14
 PULS X
 JSR ccmult
 ADDD ,S++
 ADDD #14
 STD  locptr
 LDD  #0
 STD  argstk
cc17
 LDD  #cc5+53
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc18
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR symname
 LEAS 2,S
 CMPD #0
 LBEQ cc19
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR findloc
 LEAS 2,S
 CMPD #0
 LBEQ cc20
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR multidef
 LEAS 2,S
 JMP cc21
cc20
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #0
 PSHS D
 LDD  #0
 PSHS D
 LDD  argstk
 PSHS D
 JSR addloc
 LEAS 8,S
 LDD  argstk
 ADDD #2
 STD  argstk
cc21
 JMP cc22
cc19
 LDD  #cc5+55
 PSHS D
 JSR errrpt
 LEAS 2,S
 JSR junk
cc22
 JSR blanks
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc5+77
 PSHS D
 JSR streq
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc23
 LDD  #cc5+79
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc24
 LDD  #cc5+81
 PSHS D
 JSR errrpt
 LEAS 2,S
cc24
cc23
 JSR endst
 CMPD #0
 LBEQ cc25
 JMP cc18
cc25
 JMP cc17
cc18
 LDD  #0
 STD  stkp
 LEAY 11,S
 TFR  Y,D
 PSHS D
 LDD  argstk
 STD  [,S++]
cc26
 LDD  argstk
 CMPD #0
 LBEQ cc27
 LDD  #cc5+96
 PSHS D
 LDD  #4
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc28
 LDD  11,S
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
 PSHS D
 JSR getarg
 LEAS 4,S
 JSR ns
 JMP cc29
cc28
 LDD  #cc5+101
 PSHS D
 LDD  #3
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc30
 LDD  11,S
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
 JSR getarg
 LEAS 4,S
 JSR ns
 JMP cc31
cc30
 LDD  #cc5+105
 PSHS D
 JSR errrpt
 LEAS 2,S
 JMP cc27
cc31
cc29
 JMP cc26
cc27
 JSR statemen
 PSHS D
 LDD  #3
 CMPD ,S++
 LBEQ cc32
 LDD  #0
 PSHS D
 JSR modstk
 LEAS 2,S
 JSR ret
cc32
 LDD  #0
 STD  stkp
 LDD  #symtab
 PSHS D
 LDD  #512
 PSHS D
 LDD  #14
 PULS X
 JSR ccmult
 ADDD ,S++
 ADDD #14
 STD  locptr
 LDD  litptr
 CMPD #0
 LBEQ cc33
 LDD  litlab
 PSHS D
 JSR printlab
 LEAS 2,S
 LDD  #1
 PSHS D
 JSR dumplits
 LEAS 2,S
 JSR getlabel
 STD  litlab
cc33
 LEAS 13,S
 RTS
cc5 FCB 105,108,108,101,103,97,108,32,102,117
 FCB 110,99,116,105,111,110,32,111,114,32
 FCB 100,101,99,108,97,114,97,116,105,111
 FCB 110,0,40,0,109,105,115,115,105,110
 FCB 103,32,111,112,101,110,32,112,97,114
 FCB 101,110,0,41,0,105,108,108,101,103
 FCB 97,108,32,97,114,103,117,109,101,110
 FCB 116,32,110,97,109,101,0,41,0,44
 FCB 0,101,120,112,101,99,116,101,100,32
 FCB 99,111,109,109,97,0,99,104,97,114
 FCB 0,105,110,116,0,119,114,111,110,103
 FCB 32,110,117,109,98,101,114,32,97,114
 FCB 103,115,0
 GLOBAL getarg
getarg
 LEAS -18,S
cc35
 LDD  #1
 CMPD #0
 LBEQ cc36
 LDD  argstk
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc37
 LEAS 18,S
 RTS
cc37
 LDD  #cc34+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc38
 LDD  #3
 STD  16,S
 JMP cc39
cc38
 LDD  #1
 STD  16,S
cc39
 LEAY 14,S
 TFR  Y,D
 PSHS D
 LEAY 5,S
 TFR  Y,D
 PSHS D
 JSR symname
 LEAS 2,S
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc40
 JSR illname
cc40
 LDD  #cc34+2
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc41
cc42
 JSR inbyte
 PSHS D
 LDD  #93
 CMPD ,S++
 LBEQ cc43
 JSR endst
 CMPD #0
 LBEQ cc44
 JMP cc43
cc44
 JMP cc42
cc43
 LDD  #3
 STD  16,S
cc41
 LDD  14,S
 CMPD #0
 LBEQ cc45
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LEAY 5,S
 TFR  Y,D
 PSHS D
 JSR findloc
 LEAS 2,S
 STD  [,S++]
 CMPD #0
 LBEQ cc46
 LDD  0,S
 ADDD #9
 PSHS D
 LDD  18,S
 STB  [,S++]
 LDD  0,S
 ADDD #10
 PSHS D
 LDD  22,S
 STB  [,S++]
 LEAY 12,S
 TFR  Y,D
 PSHS D
 LDD  24,S
 PSHS D
 LDD  4,S
 ADDD #12
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  6,S
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
 ADDD ,S++
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  [,S++]
 LDD  12,S
 PSHS D
 LDD  2,S
 ADDD #12
 PSHS D
 LDD  #2
 PSHS D
 JSR putint
 LEAS 6,S
 JMP cc47
cc46
 LDD  #cc34+4
 PSHS D
 JSR errrpt
 LEAS 2,S
cc47
cc45
 LDD  argstk
 PSHS D
 LDD  #2
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  argstk
 JSR endst
 CMPD #0
 LBEQ cc48
 LEAS 18,S
 RTS
cc48
 LDD  #cc34+27
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc49
 LDD  #cc34+29
 PSHS D
 JSR errrpt
 LEAS 2,S
cc49
 JMP cc35
cc36
 LEAS 18,S
 RTS
cc34 FCB 42,0,91,0,69,120,112,101,99,116
 FCB 101,100,32,97,114,103,117,109,101,110
 FCB 116,32,110,97,109,101,0,44,0,101
 FCB 120,112,101,99,116,101,100,32,99,111
 FCB 109,109,97,0
 GLOBAL statemen
statemen
 LEAS -2,S
 LDD  #ch
 PSHS D
 LDD  #0
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 LDD  #ch
 CMPD #0
 LBEQ cc52 _
 LDD  eof
 CMPD #0
 LBEQ cc52 _
 LDD  #1
cc52
 CMPD #0
 LBEQ cc51
 LEAS 2,S
 RTS
cc51
 LDD  #cc50+0
 PSHS D
 LDD  #6
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc53
 LDD  #1
 STD  0,S
 LDD  stdecl
 ADDD #1
 STD  stdecl
 PSHS D
 LDD  #1
 CMPD ,S++
 LBNE cc54
 JSR getlabel
 STD  stlab
 PSHS D
 JSR jump
 LEAS 2,S
cc54
 JMP cc55
cc53
 LDD  #2
 STD  0,S
cc55
 LDD  #cc50+7
 PSHS D
 LDD  #4
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc56
 LDD  #1
 PSHS D
 LDD  #2
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 PSHS D
 LDD  2,S
 PSHS D
 JSR declloc
 LEAS 4,S
 JSR ns
 JMP cc57
cc56
 LDD  #cc50+12
 PSHS D
 LDD  #3
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc58
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
 LDD  2,S
 PSHS D
 JSR declloc
 LEAS 4,S
 JSR ns
 JMP cc59
cc58
 LDD  stdecl
 PSHS D
 LDD  #0
 CMPD ,S++
 LBGE cc60
 LDD  stlab
 PSHS D
 JSR postlabe
 LEAS 2,S
 LDD  #0
 STD  stdecl
cc60
 LDD  declared
 PSHS D
 LDD  #0
 CMPD ,S++
 LBGT cc61
 LDD  stkp
 PSHS D
 LDD  declared
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 PSHS D
 JSR modstk
 LEAS 2,S
 STD  stkp
 LDD  #1
 COMA
 COMB
 ADDD #1
 STD  declared
cc61
 LDD  #cc50+16
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc62
 JSR compound
 JMP cc63
cc62
 LDD  #cc50+18
 PSHS D
 LDD  #2
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc64
 JSR doif
 LDD  #1
 STD  lastst
 JMP cc65
cc64
 LDD  #cc50+21
 PSHS D
 LDD  #5
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc66
 JSR dowhile
 LDD  #2
 STD  lastst
 JMP cc67
cc66
 LDD  #cc50+27
 PSHS D
 LDD  #2
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc68
 JSR dodo
 LDD  #8
 STD  lastst
 JMP cc69
cc68
 LDD  #cc50+30
 PSHS D
 LDD  #3
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc70
 JSR dofor
 LDD  #9
 STD  lastst
 JMP cc71
cc70
 LDD  #cc50+34
 PSHS D
 LDD  #6
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc72
 JSR doswitch
 LDD  #10
 STD  lastst
 JMP cc73
cc72
 LDD  #cc50+41
 PSHS D
 LDD  #4
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc74
 JSR docase
 LDD  #12
 STD  lastst
 JMP cc75
cc74
 LDD  #cc50+46
 PSHS D
 LDD  #7
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc76
 JSR dodefaul
 LDD  #13
 STD  lastst
 JMP cc77
cc76
 LDD  #cc50+54
 PSHS D
 LDD  #6
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc78
 JSR doreturn
 JSR ns
 LDD  #3
 STD  lastst
 JMP cc79
cc78
 LDD  #cc50+61
 PSHS D
 LDD  #5
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc80
 JSR dobreak
 JSR ns
 LDD  #4
 STD  lastst
 JMP cc81
cc80
 LDD  #cc50+67
 PSHS D
 LDD  #8
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc82
 JSR docont
 JSR ns
 LDD  #5
 STD  lastst
 JMP cc83
cc82
 LDD  #cc50+76
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc84
 JMP cc85
cc84
 LDD  #cc50+78
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc86
 JSR doasm
 LDD  #6
 STD  lastst
 JMP cc87
cc86
 JSR doexpres
 JSR ns
 LDD  #7
 STD  lastst
cc87
cc85
cc83
cc81
cc79
cc77
cc75
cc73
cc71
cc69
cc67
cc65
cc63
cc59
cc57
 LDD  lastst
 LEAS 2,S
 RTS
cc50 FCB 115,116,97,116,105,99,0,99,104,97
 FCB 114,0,105,110,116,0,123,0,105,102
 FCB 0,119,104,105,108,101,0,100,111,0
 FCB 102,111,114,0,115,119,105,116,99,104
 FCB 0,99,97,115,101,0,100,101,102,97
 FCB 117,108,116,0,114,101,116,117,114,110
 FCB 0,98,114,101,97,107,0,99,111,110
 FCB 116,105,110,117,101,0,59,0,35,97
 FCB 115,109,0
 GLOBAL dodo
dodo
 LEAS -10,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR addwhile
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 PSHS D
 JSR postlabe
 LEAS 2,S
 JSR statemen
 LDD  #cc88+0
 PSHS D
 JSR needbrac
 LEAS 2,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR postlabe
 LEAS 2,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 LDD  #1
 PSHS D
 JSR test
 LEAS 4,S
 LDD  0,S
 PSHS D
 JSR jump
 LEAS 2,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR postlabe
 LEAS 2,S
 JSR delwhile
 JSR ns
 LEAS 10,S
 RTS
cc88 FCB 119,104,105,108,101,0
 GLOBAL dofor
dofor
 LEAS -12,S
 LEAY 4,S
 TFR  Y,D
 PSHS D
 JSR addwhile
 LEAS 2,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 LDD  #cc89+0
 PSHS D
 JSR needbrac
 LEAS 2,S
 LDD  #cc89+2
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc90
 JSR doexpres
 JSR ns
cc90
 LDD  2,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 LDD  #cc89+4
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc91
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 LDD  #0
 PSHS D
 JSR test
 LEAS 4,S
 JSR ns
cc91
 LDD  0,S
 PSHS D
 JSR jump
 LEAS 2,S
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR postlabe
 LEAS 2,S
 LDD  #cc89+6
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc92
 JSR doexpres
 LDD  #cc89+8
 PSHS D
 JSR needbrac
 LEAS 2,S
cc92
 LDD  2,S
 PSHS D
 JSR jump
 LEAS 2,S
 LDD  0,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 JSR statemen
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR jump
 LEAS 2,S
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR postlabe
 LEAS 2,S
 JSR delwhile
 LEAS 12,S
 RTS
cc89 FCB 40,0,59,0,59,0,41,0,41,0
 GLOBAL doswitch
doswitch
 LEAS -18,S
 LEAY 6,S
 TFR  Y,D
 PSHS D
 LDD  swactive
 STD  [,S++]
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  swdefaul
 STD  [,S++]
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  swnext
 STD  [,S++]
 STD  [,S++]
 LEAY 10,S
 TFR  Y,D
 PSHS D
 JSR addwhile
 LEAS 2,S
 LDD  #cc93+0
 PSHS D
 JSR needbrac
 LEAS 2,S
 JSR doexpres
 LDD  #cc93+2
 PSHS D
 JSR needbrac
 LEAS 2,S
 LDD  #0
 STD  swdefaul
 LDD  #1
 STD  swactive
 LEAY 8,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 PSHS D
 JSR jump
 LEAS 2,S
 JSR statemen
 LEAY 10,S
 TFR  Y,D
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR jump
 LEAS 2,S
 LDD  8,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 JSR sw
cc94
 LDD  0,S
 PSHS D
 LDD  swnext
 CMPD ,S++
 LBLS cc95
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
 LDD  #2
 DECB
 BLT  *+8
 LSR  ,S
 ROR  1,S
 BRA  *-7
 PULS D
 PSHS D
 JSR defstora
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 PSHS D
 LDD  [,S++]
 ADDD #1
 ADDD #1
 STD  [,S++]
 SUBD #1
 SUBD #1
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR printlab
 LEAS 2,S
 LDD  #44
 PSHS D
 JSR outbyte
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 PSHS D
 LDD  [,S++]
 ADDD #1
 ADDD #1
 STD  [,S++]
 SUBD #1
 SUBD #1
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR outdec
 LEAS 2,S
 JSR nl
 JMP cc94
cc95
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
 LDD  #2
 DECB
 BLT  *+8
 LSR  ,S
 ROR  1,S
 BRA  *-7
 PULS D
 PSHS D
 JSR defstora
 LEAS 2,S
 LDD  #0
 PSHS D
 JSR outdec
 LEAS 2,S
 JSR nl
 LDD  swdefaul
 CMPD #0
 LBEQ cc96
 LDD  swdefaul
 PSHS D
 JSR jump
 LEAS 2,S
cc96
 LEAY 10,S
 TFR  Y,D
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR postlabe
 LEAS 2,S
 JSR delwhile
 LDD  2,S
 STD  swnext
 LDD  4,S
 STD  swdefaul
 LDD  6,S
 STD  swactive
 LEAS 18,S
 RTS
cc93 FCB 40,0,41,0
 GLOBAL docase
docase
 LDD  swactive
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc98
 LDD  #cc97+0
 PSHS D
 JSR errrpt
 LEAS 2,S
cc98
 LDD  swnext
 PSHS D
 LDD  swend
 CMPD ,S++
 LBHS cc99
 LDD  #cc97+14
 PSHS D
 JSR errrpt
 LEAS 2,S
 RTS
cc99
 LDD  swnext
 ADDD #1
 ADDD #1
 STD  swnext
 SUBD #1
 SUBD #1
 PSHS D
 JSR getlabel
 STD  [,S++]
 PSHS D
 JSR postlabe
 LEAS 2,S
 LDD  swnext
 ADDD #1
 ADDD #1
 STD  swnext
 SUBD #1
 SUBD #1
 PSHS D
 JSR constexp
 LEAS 2,S
 LDD  #cc97+29
 PSHS D
 JSR needbrac
 LEAS 2,S
 RTS
cc97 FCB 110,111,116,32,105,110,32,115,119,105
 FCB 116,99,104,0,116,111,111,32,109,97
 FCB 110,121,32,99,97,115,101,115,0,58
 FCB 0
 GLOBAL dodefaul
dodefaul
 LDD  swactive
 CMPD #0
 LBEQ cc101
 LDD  swdefaul
 CMPD #0
 LBEQ cc102
 LDD  #cc100+0
 PSHS D
 JSR errrpt
 LEAS 2,S
cc102
 JMP cc103
cc101
 LDD  #cc100+18
 PSHS D
 JSR errrpt
 LEAS 2,S
cc103
 LDD  #cc100+32
 PSHS D
 JSR needbrac
 LEAS 2,S
 JSR getlabel
 STD  swdefaul
 PSHS D
 JSR postlabe
 LEAS 2,S
 RTS
cc100 FCB 109,117,108,116,105,112,108,101,32,100
 FCB 101,102,97,117,108,116,115,0,110,111
 FCB 116,32,105,110,32,115,119,105,116,99
 FCB 104,0,58,0
 GLOBAL ns
ns
 LDD  #cc104+0
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc105
 LDD  #cc104+2
 PSHS D
 JSR errrpt
 LEAS 2,S
cc105
 RTS
cc104 FCB 59,0,109,105,115,115,105,110,103,32
 FCB 115,101,109,105,99,111,108,111,110,0
 GLOBAL compound
compound
 LEAS -6,S
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  stkp
 STD  [,S++]
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  locptr
 STD  [,S++]
 STD  startcom
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  statptr
 STD  [,S++]
 LDD  #0
 STD  statlab
 STD  stdecl
 STD  declared
 LDD  ncmp
 ADDD #1
 STD  ncmp
cc107
 LDD  #cc106+0
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc108
 LDD  eof
 CMPD #0
 LBEQ cc109
 LDD  #cc106+2
 PSHS D
 JSR errrpt
 LEAS 2,S
 JMP cc108
 JMP cc110
cc109
 JSR statemen
cc110
 JMP cc107
cc108
 LDD  ncmp
 SUBD #1
 STD  ncmp
 LDD  lastst
 PSHS D
 LDD  #3
 CMPD ,S++
 LBEQ cc111
 LDD  4,S
 PSHS D
 JSR modstk
 LEAS 2,S
cc111
 LDD  4,S
 STD  stkp
 LDD  #1
 COMA
 COMB
 ADDD #1
 STD  declared
 LDD  2,S
 STD  locptr
 LDD  0,S
 STD  statptr
 LEAS 6,S
 RTS
cc106 FCB 125,0,110,111,32,102,105,110,97,108
 FCB 32,125,0
 GLOBAL doif
doif
 LEAS -8,S
 LEAY 6,S
 TFR  Y,D
 PSHS D
 LDD  locptr
 STD  [,S++]
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  stkp
 STD  [,S++]
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 LDD  2,S
 PSHS D
 LDD  #1
 PSHS D
 JSR test
 LEAS 4,S
 JSR statemen
 LDD  4,S
 PSHS D
 JSR modstk
 LEAS 2,S
 STD  stkp
 LDD  6,S
 STD  locptr
 LDD  #cc112+0
 PSHS D
 LDD  #4
 PSHS D
 JSR amatch
 LEAS 4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc113
 LDD  2,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 LEAS 8,S
 RTS
cc113
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR getlabel
 STD  [,S++]
 LDD  lastst
 PSHS D
 LDD  #3
 CMPD ,S++
 LBEQ cc114
 LDD  0,S
 PSHS D
 JSR jump
 LEAS 2,S
cc114
 LDD  2,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 JSR statemen
 LDD  4,S
 PSHS D
 JSR modstk
 LEAS 2,S
 STD  stkp
 LDD  6,S
 STD  locptr
 LDD  0,S
 PSHS D
 JSR postlabe
 LEAS 2,S
 LEAS 8,S
 RTS
cc112 FCB 101,108,115,101,0
 GLOBAL dowhile
dowhile
 LEAS -8,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR addwhile
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR postlabe
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 LDD  #1
 PSHS D
 JSR test
 LEAS 4,S
 JSR statemen
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR jump
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR postlabe
 LEAS 2,S
 JSR delwhile
 LEAS 8,S
 RTS
 GLOBAL doreturn
doreturn
 JSR endst
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc116
 JSR doexpres
cc116
 LDD  #0
 PSHS D
 JSR modstk
 LEAS 2,S
 JSR ret
 RTS
 GLOBAL dobreak
dobreak
 LEAS -2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR readwhil
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc117
 LEAS 2,S
 RTS
cc117
 LDD  0,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR modstk
 LEAS 2,S
 LDD  0,S
 PSHS D
 LDD  #3
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR jump
 LEAS 2,S
 LEAS 2,S
 RTS
 GLOBAL docont
docont
 LEAS -2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR readwhil
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc118
 LEAS 2,S
 RTS
cc118
 LDD  0,S
 PSHS D
 LDD  #1
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR modstk
 LEAS 2,S
 LDD  0,S
 PSHS D
 LDD  #2
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 PSHS D
 JSR jump
 LEAS 2,S
 LEAS 2,S
 RTS
 GLOBAL doasm
doasm
 LDD  #0
 STD  cmode
cc119
 LDD  #1
 CMPD #0
 LBEQ cc120
 JSR inline
 LDD  eof
 CMPD #0
 LBEQ cc121
 JMP cc120
cc121
 LDD  #cc115+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc122
 LDD  ctext
 CMPD #0
 LBEQ cc123
 JSR comment
 LDD  #line
 PSHS D
 JSR outstr
 LEAS 2,S
cc123
 JMP cc120
cc122
 LDD  #line
 PSHS D
 JSR outstr
 LEAS 2,S
 JSR nl
 JMP cc119
cc120
 JSR kill
 LDD  #1
 STD  cmode
 RTS
cc115 FCB 35,101,110,100,97,115,109,0
 END
