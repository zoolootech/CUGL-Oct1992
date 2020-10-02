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
 GLOBAL _char
_char
 FCB 99,104,97,114,0
 GLOBAL _int
_int
 FCB 105,110,116,0
 GLOBAL stattab
stattab
 RZB 690
 GLOBAL symtab
symtab
 RZB 8008
 GLOBAL glbptr
glbptr
 FDB 0
 GLOBAL locptr
locptr
 FDB 0
 GLOBAL statptr
statptr
 FDB 0
 GLOBAL endsearc
endsearc
 FDB 0
 GLOBAL startcom
startcom
 FDB 0
 GLOBAL swq
swq
 RZB 100
 GLOBAL argcs
argcs
 RZB 2
 GLOBAL argvs
argvs
 FDB 0
 GLOBAL wq
wq
 RZB 200
 GLOBAL wqptr
wqptr
 FDB 0
 GLOBAL litq
litq
 RZB 1000
 GLOBAL litptr
litptr
 RZB 2
 GLOBAL macq
macq
 RZB 500
 GLOBAL macptr
macptr
 RZB 2
 GLOBAL stdecl
stdecl
 RZB 2
 GLOBAL stage
stage
 RZB 800
 GLOBAL stagenex
stagenex
 FDB 0
 GLOBAL stagelas
stagelas
 FDB 0
 GLOBAL line
line
 RZB 80
 GLOBAL mline
mline
 RZB 80
 GLOBAL lptr
lptr
 RZB 2
 GLOBAL mptr
mptr
 RZB 2
 GLOBAL nxtlab
nxtlab
 RZB 2
 GLOBAL statlab
statlab
 RZB 2
 GLOBAL stlab
stlab
 RZB 2
 GLOBAL litlab
litlab
 RZB 2
 GLOBAL constval
constval
 RZB 4
 GLOBAL monitor
monitor
 RZB 2
 GLOBAL pause
pause
 RZB 2
 GLOBAL DEFDEBUG
DEFDEBUG
 RZB 2
 GLOBAL first_fu
first_fu
 RZB 2
 GLOBAL declared
declared
 RZB 2
 GLOBAL stkp
stkp
 RZB 2
 GLOBAL argstk
argstk
 RZB 2
 GLOBAL ncmp
ncmp
 RZB 2
 GLOBAL swactive
swactive
 RZB 2
 GLOBAL swdefaul
swdefaul
 RZB 2
 GLOBAL swnext
swnext
 FDB 0
 GLOBAL swend
swend
 FDB 0
 GLOBAL errcnt
errcnt
 RZB 2
 GLOBAL eof
eof
 RZB 2
 GLOBAL input
input
 RZB 2
 GLOBAL output
output
 RZB 2
 GLOBAL input2
input2
 RZB 2
 GLOBAL iflevel
iflevel
 RZB 2
 GLOBAL skipleve
skipleve
 RZB 2
 GLOBAL ctext
ctext
 RZB 2
 GLOBAL cmode
cmode
 RZB 2
 GLOBAL lastst
lastst
 RZB 2
 GLOBAL quote
quote
 RZB 2
 GLOBAL cptr
cptr
 FDB 0
 GLOBAL cptr2
cptr2
 FDB 0
 GLOBAL iptr
iptr
 FDB 0
 EXT addglb
 EXT addloc
 EXT addmac
 EXT addstati
 EXT amatch
 EXT blanks
 EXT cnl
 EXT ch
 EXT constexp
 EXT defstora
 EXT doasm
 EXT dumpzero
 EXT entry
 EXT endst
 EXT errrpt
 EXT declexte
 EXT findglb
 EXT findloc
 EXT getint
 EXT getlabel
 EXT header
 EXT illname
 EXT kill
 EXT match
 EXT multidef
 EXT needbrac
 EXT needsub
 EXT newfunc
 EXT nl
 EXT number
 EXT ns
 EXT outbyte
 EXT outdec
 EXT outstr
 EXT pl
 EXT point
 EXT printlab
 EXT qstr
 EXT sout
 EXT stowlit
 EXT symname
 EXT trailer
 EXT upper
 NAME main
 GLOBAL main
main
 LDD  4,S
 STD  argcs
 LDD  2,S
 STD  argvs
 LDD  #swq
 STD  swnext
 PSHS D
 LDD  #100
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  #4
 PSHS D
 ADDD ,S++
 ADDD ,S++
 STD  swend
 LDD  #1
 STD  first_fu
 LDD  #1
 COMA
 COMB
 ADDD #1
 STD  declared
 LDD  #symtab
 STD  glbptr
 LDD  #symtab
 PSHS D
 LDD  #14
 PSHS D
 LDD  #5
 PULS X
 JSR ccmult
 ADDD ,S++
 STD  glbptr
 LDD  #stattab
 STD  statptr
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
 STD  endsearc
 STD  startcom
 LDD  #stage
 ADDD #800
 PSHS D
 LDD  #1
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  stagelas
 LDD  #wq
 STD  wqptr
 LDD  #quote
 ADDD #1
 PSHS D
 LDD  #0
 STB  [,S++]
 STD  stagenex
 STD  skipleve
 STD  iflevel
 STD  lastst
 STD  ncmp
 STD  output
 STD  input2
 STD  input
 STD  eof
 STD  errcnt
 STD  stkp
 STD  litptr
 LDD  #quote
 ADDD #0
 PSHS D
 LDD  #34
 STB  [,S++]
 LDD  #1
 STD  macptr
 STD  cmode
 JSR ask
 JSR header
 JSR parse
 JSR trailer
 JSR closeout
 JSR errrptsu
 RTS
 GLOBAL parse
parse
 LEAS -2,S
cc2
 LDD  eof
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc3
 LDD  #4
 STD  0,S
 LDD  #cc1+0
 PSHS D
 LDD  #6
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc4
 LDD  #3
 STD  0,S
cc4
 LDD  #cc1+7
 PSHS D
 LDD  #6
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc5
 LDD  #1
 STD  0,S
cc5
 LDD  #_char
 PSHS D
 LDD  #4
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc6
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
 JSR declglb
 LEAS 4,S
 JSR ns
 JMP cc7
cc6
 LDD  #_int
 PSHS D
 LDD  #3
 PSHS D
 JSR amatch
 LEAS 4,S
 CMPD #0
 LBEQ cc8
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
 JSR declglb
 LEAS 4,S
 JSR ns
 JMP cc9
cc8
 LDD  #cc1+14
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc10
 JSR doasm
 JMP cc11
cc10
 LDD  #cc1+19
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc12
 JSR doinclud
 JMP cc13
cc12
 LDD  #cc1+28
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc14
 JSR addmac
 JMP cc15
cc14
 LDD  0,S
 PSHS D
 JSR newfunc
 LEAS 2,S
cc15
cc13
cc11
cc9
cc7
 JSR blanks
 JMP cc2
cc3
 LEAS 2,S
 RTS
cc1 FCB 101,120,116,101,114,110,0,115,116,97
 FCB 116,105,99,0,35,97,115,109,0,35
 FCB 105,110,99,108,117,100,101,0,35,100
 FCB 101,102,105,110,101,0
 GLOBAL dumplits
dumplits
 LEAS -4,S
 LDD  #0
 STD  0,S
cc17
 LDD  0,S
 PSHS D
 LDD  litptr
 CMPD ,S++
 LBLE cc18
 LDD  6,S
 PSHS D
 JSR defstora
 LEAS 2,S
 LDD  #10
 STD  2,S
cc19
 LDD  2,S
 SUBD #1
 STD  2,S
 ADDD #1
 CMPD #0
 LBEQ cc20
 LDD  #litq
 PSHS D
 LDD  2,S
 ADDD ,S++
 PSHS D
 LDD  8,S
 PSHS D
 JSR getint
 LEAS 4,S
 PSHS D
 JSR outdec
 LEAS 2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  2,S
 PSHS D
 LDD  10,S
 ADDD ,S++
 STD  [,S++]
 LDD  2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 PSHS D
 LDD  2,S
 PSHS D
 LDD  litptr
 CMPD ,S++
 BLE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ORA  ,S+
 ORB  ,S+
 CMPD #0
 LBEQ cc21
 JSR nl
 JMP cc20
cc21
 LDD  #44
 PSHS D
 JSR outbyte
 LEAS 2,S
 JMP cc19
cc20
 JMP cc17
cc18
 LDD  #0
 STD  litptr
 LEAS 4,S
 RTS
 GLOBAL errrptsu
errrptsu
 LDD  ncmp
 CMPD #0
 LBEQ cc22
 LDD  #cc16+0
 PSHS D
 JSR errrpt
 LEAS 2,S
cc22
 LDD  #1
 STD  output
 JSR cnl
 LDD  errcnt
 PSHS D
 JSR outdec
 LEAS 2,S
 LDD  #cc16+24
 PSHS D
 JSR outstr
 LEAS 2,S
 RTS
cc16 FCB 109,105,115,115,105,110,103,32,99,108
 FCB 111,115,105,110,103,32,98,114,97,99
 FCB 107,101,116,0,32,101,114,114,111,114
 FCB 40,115,41,32,105,110,32,99,111,109
 FCB 112,105,108,97,116,105,111,110,46,0
 GLOBAL ask
ask
 LEAS -4,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #0
 STD  nxtlab
 STD  [,S++]
 JSR getlabel
 STD  litlab
 JSR kill
 LDD  #1
 STD  output
 LDD  #0
 STD  ctext
 STD  DEFDEBUG
 STD  pause
 STD  monitor
 LDD  argcs
 PSHS D
 LDD  #1
 CMPD ,S++
 LBNE cc24
 LDD  #cc23+0
 PSHS D
 LDD  #2
 PSHS D
 JSR sout
 LEAS 4,S
 LDD  #cc23+35
 PSHS D
 LDD  #2
 PSHS D
 JSR sout
 LEAS 4,S
 LDD  #cc23+51
 PSHS D
 LDD  #2
 PSHS D
 JSR sout
 LEAS 4,S
 LDD  #0
 PSHS D
 JSR exit
 LEAS 2,S
cc24
cc25
 LDD  argcs
 SUBD #1
 STD  argcs
 CMPD #0
 LBEQ cc26
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  argvs
 PSHS D
 LDD  6,S
 ADDD #1
 STD  6,S
 PSHS D
 ADDD ,S++
 ADDD ,S++
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 LDD  0,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #60
 CMPD ,S++
 LBNE cc27
 LDD  0,S
 ADDD #1
 STD  0,S
 PSHS D
 LDD  #cc23+53
 PSHS D
 JSR fopen
 LEAS 4,S
 STD  input
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc28
 LDD  #cc23+55
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 PSHS D
 JSR exit
 LEAS 2,S
 JMP cc29
cc28
 JMP cc25
cc29
 JMP cc30
cc27
 LDD  0,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #62
 CMPD ,S++
 LBNE cc31
 LDD  0,S
 ADDD #1
 STD  0,S
 PSHS D
 LDD  #cc23+72
 PSHS D
 JSR fopen
 LEAS 4,S
 STD  output
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc32
 LDD  #cc23+74
 PSHS D
 JSR errrpt
 LEAS 2,S
 LDD  #0
 PSHS D
 JSR exit
 LEAS 2,S
 JMP cc33
cc32
 JMP cc25
cc33
 JMP cc34
cc31
 LDD  0,S
 ADDD #1
 STD  0,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #45
 CMPD ,S++
 LBNE cc35
 LDD  0,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 JSR upper
 LEAS 2,S
 PSHS D
 LDD  #67
 CMPD ,S++
 LBNE cc36
 LDD  #1
 STD  ctext
 JMP cc37
cc36
 LDD  0,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 JSR upper
 LEAS 2,S
 PSHS D
 LDD  #77
 CMPD ,S++
 LBNE cc38
 LDD  #1
 STD  monitor
 JMP cc39
cc38
 LDD  0,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 JSR upper
 LEAS 2,S
 PSHS D
 LDD  #80
 CMPD ,S++
 LBNE cc40
 LDD  #1
 STD  pause
 JMP cc41
cc40
 LDD  0,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 JSR upper
 LEAS 2,S
 PSHS D
 LDD  #84
 CMPD ,S++
 LBNE cc42
 LDD  #1
 STD  DEFDEBUG
cc42
cc41
cc39
cc37
cc35
cc34
cc30
 JMP cc25
cc26
 LDD  input
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc43
 LDD  #cc23+93
 PSHS D
 LDD  #2
 PSHS D
 JSR sout
 LEAS 4,S
 LDD  #0
 PSHS D
 JSR exit
 LEAS 2,S
cc43
 LEAS 4,S
 RTS
cc23 FCB 13,117,115,97,103,101,58,32,99,99
 FCB 32,60,105,110,102,105,108,101,32,91
 FCB 62,111,117,116,102,105,108,101,93,32
 FCB 91,45,99,93,0,32,91,45,109,93
 FCB 32,91,45,112,93,32,91,45,116,93
 FCB 0,13,0,114,0,105,110,112,117,116
 FCB 32,102,105,108,101,32,101,114,114,111
 FCB 114,0,119,0,111,117,116,112,117,116
 FCB 32,102,105,108,101,32,101,114,114,114
 FCB 112,116,0,13,110,111,32,105,110,112
 FCB 117,116,32,102,105,108,101,46,46,46
 FCB 13,0
 GLOBAL doinclud
doinclud
 JSR blanks
 LDD  #line
 ADDD lptr
 PSHS D
 LDD  #cc44+0
 PSHS D
 JSR fopen
 LEAS 4,S
 STD  input2
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc45
 LDD  #0
 STD  input2
 LDD  #cc44+2
 PSHS D
 JSR errrpt
 LEAS 2,S
cc45
 JSR kill
 RTS
cc44 FCB 114,0,79,112,101,110,32,102,97,105
 FCB 108,117,114,101,32,111,110,32,105,110
 FCB 99,108,117,100,101,32,102,105,108,101
 FCB 0
 GLOBAL closeout
closeout
 LDD  output
 CMPD #0
 LBEQ cc47
 LDD  output
 PSHS D
 JSR fclose
 LEAS 2,S
cc47
 LDD  #0
 STD  output
 RTS
 GLOBAL declglb
declglb
 LEAS -13,S
cc48
 LDD  #1
 CMPD #0
 LBEQ cc49
 JSR endst
 CMPD #0
 LBEQ cc50
 LEAS 13,S
 RTS
cc50
 LDD  #cc46+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc51
 LDD  #3
 STD  9,S
 LDD  #0
 STD  11,S
 JMP cc52
cc51
 LDD  #1
 STD  9,S
 LDD  #1
 STD  11,S
cc52
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR symname
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc53
 JSR illname
cc53
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR findglb
 LEAS 2,S
 CMPD #0
 LBEQ cc54
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR multidef
 LEAS 2,S
cc54
 LDD  #cc46+2
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc55
 LDD  #4
 STD  9,S
 JMP cc56
cc55
 LDD  #cc46+5
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc57
 LEAY 11,S
 TFR  Y,D
 PSHS D
 JSR needsub
 STD  [,S++]
 LDD  #2
 STD  9,S
cc57
cc56
 LDD  15,S
 PSHS D
 LDD  #3
 CMPD ,S++
 LBNE cc58
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR declexte
 LEAS 2,S
 JMP cc59
cc58
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  17,S
 PSHS D
 JSR entry
 LEAS 4,S
 LEAY 9,S
 TFR  Y,D
 PSHS D
 LDD  19,S
 PSHS D
 LDD  #2
 DECB
 BLT  *+8
 LSR  ,S
 ROR  1,S
 BRA  *-7
 PULS D
 PSHS D
 LDD  13,S
 PSHS D
 LDD  17,S
 PSHS D
 JSR initials
 LEAS 6,S
 STD  [,S++]
cc59
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  11,S
 PSHS D
 LDD  21,S
 PSHS D
 LDD  17,S
 PSHS D
 LDD  23,S
 PSHS D
 JSR addglb
 LEAS 10,S
 LDD  #cc46+7
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc60
 LEAS 13,S
 RTS
cc60
 JMP cc48
cc49
 LEAS 13,S
 RTS
cc46 FCB 42,0,40,41,0,91,0,44,0
 GLOBAL declloc
declloc
 LEAS -22,S
 LDD  declared
 PSHS D
 LDD  #0
 CMPD ,S++
 LBLE cc62
 LDD  #cc61+0
 PSHS D
 JSR errrpt
 LEAS 2,S
cc62
cc63
 LDD  #1
 CMPD #0
 LBEQ cc64
cc65
 LDD  #1
 CMPD #0
 LBEQ cc66
 JSR endst
 CMPD #0
 LBEQ cc67
 LEAS 22,S
 RTS
cc67
 LDD  #cc61+28
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc68
 LDD  #3
 STD  18,S
 JMP cc69
cc68
 LDD  #1
 STD  18,S
cc69
 LEAY 9,S
 TFR  Y,D
 PSHS D
 JSR symname
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc70
 JSR illname
cc70
 LDD  startcom
 STD  endsearc
 LEAY 9,S
 TFR  Y,D
 PSHS D
 JSR findloc
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBEQ cc71
 LEAY 9,S
 TFR  Y,D
 PSHS D
 JSR multidef
 LEAS 2,S
cc71
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
 LDD  #2
 STD  20,S
 LDD  #cc61+30
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc72
 LEAY 20,S
 TFR  Y,D
 PSHS D
 JSR needsub
 STD  [,S++]
 LDD  20,S
 CMPD #0
 LBNE cc74
 LDD  24,S
 PSHS D
 LDD  #1
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #0
 LBNE cc74
 LDD  #0
 JMP cc75
cc74
 LDD  #1
cc75
 CMPD #0
 LBEQ cc73
 LDD  #2
 STD  18,S
 LDD  26,S
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
 LBNE cc76
 LEAY 20,S
 TFR  Y,D
 PSHS D
 LDD  22,S
 PSHS D
 LDD  24,S
 ADDD ,S++
 STD  [,S++]
cc76
 JMP cc77
cc73
 LDD  #3
 STD  18,S
 LDD  #2
 STD  20,S
cc77
 JMP cc78
cc72
 LDD  #cc61+32
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc79
 LDD  #4
 STD  18,S
 JMP cc80
cc79
 LDD  26,S
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
 JMP cc82 _
 LDD  #1
 LDD  18,S
 PSHS D
 LDD  #1
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc82 _
 LDD  #1
 LDD  #1
cc82
 CMPD #0
 LBEQ cc81
 LDD  #1
 STD  20,S
cc81
cc80
cc78
 LDD  24,S
 PSHS D
 LDD  #1
 CMPD ,S++
 LBNE cc83
 LDD  statlab
 ADDD #1
 STD  statlab
 SUBD #1
 PSHS D
 LEAY 2,S
 TFR  Y,D
 PSHS D
 JSR convert
 LEAS 4,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  26,S
 PSHS D
 JSR entry
 LEAS 4,S
 LDD  26,S
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
 LBNE cc84
 LEAY 20,S
 TFR  Y,D
 PSHS D
 LDD  22,S
 PSHS D
 LDD  #1
 DECB
 BLT  *+8
 LSR  ,S
 ROR  1,S
 BRA  *-7
 PULS D
 STD  [,S++]
cc84
 LEAY 18,S
 TFR  Y,D
 PSHS D
 LDD  28,S
 PSHS D
 LDD  #2
 DECB
 BLT  *+8
 LSR  ,S
 ROR  1,S
 BRA  *-7
 PULS D
 PSHS D
 LDD  22,S
 PSHS D
 LDD  26,S
 PSHS D
 JSR initials
 LEAS 6,S
 STD  [,S++]
 LEAY 9,S
 TFR  Y,D
 PSHS D
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  22,S
 PSHS D
 LDD  32,S
 PSHS D
 LDD  28,S
 PSHS D
 JSR addstati
 LEAS 10,S
 LEAY 9,S
 TFR  Y,D
 PSHS D
 LDD  20,S
 PSHS D
 LDD  30,S
 PSHS D
 LDD  #0
 PSHS D
 JSR addloc
 LEAS 8,S
 JMP cc85
cc83
 LDD  declared
 PSHS D
 LDD  22,S
 ADDD ,S++
 STD  declared
 LEAY 9,S
 TFR  Y,D
 PSHS D
 LDD  20,S
 PSHS D
 LDD  30,S
 PSHS D
 LDD  stkp
 PSHS D
 LDD  declared
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 PSHS D
 JSR addloc
 LEAS 8,S
cc85
 JMP cc66
 JMP cc65
cc66
 LDD  #cc61+35
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc86
 LEAS 22,S
 RTS
cc86
 JMP cc63
cc64
 LEAS 22,S
 RTS
cc61 FCB 109,117,115,116,32,100,101,99,108,97
 FCB 114,101,32,102,105,114,115,116,32,105
 FCB 110,32,98,108,111,99,107,0,42,0
 FCB 91,0,40,41,0,44,0
 GLOBAL convert
convert
 LEAS -3,S
 LEAY 1,S
 TFR  Y,D
 PSHS D
 LDD  7,S
 STD  [,S++]
cc90
 LDD  5,S
 ADDD #1
 STD  5,S
 SUBD #1
 PSHS D
 LDD  9,S
 PSHS D
 LDD  #10
 PULS X
 JSR ccdiv
 EXG  D,X
 ADDD #48
 STB  [,S++]
cc88
 LEAY 7,S
 TFR  Y,D
 PSHS D
 LDD  #10
 LDX  [,S]
 JSR ccdiv
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBGE cc89
 JMP cc90
cc89
 LDD  5,S
 SUBD #1
 STD  5,S
 ADDD #1
 PSHS D
 LDD  #0
 STB  [,S++]
cc91
 LDD  1,S
 PSHS D
 LDD  7,S
 CMPD ,S++
 LBLS cc92
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  3,S
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 LDD  1,S
 ADDD #1
 STD  1,S
 SUBD #1
 PSHS D
 LDD  7,S
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 LDD  5,S
 SUBD #1
 STD  5,S
 ADDD #1
 PSHS D
 LDB  2,S
 SEX
 STB  [,S++]
 JMP cc91
cc92
 LEAS 3,S
 RTS
 GLOBAL initials
initials
 LEAS -2,S
 LDD  4,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc93
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  #1
 COMA
 COMB
 ADDD #1
 STD  [,S++]
cc93
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  6,S
 STD  [,S++]
 LDD  #cc87+0
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc94
 LDD  #cc87+2
 PSHS D
 JSR match
 LEAS 2,S
 CMPD #0
 LBEQ cc95
cc96
 LDD  4,S
 CMPD #0
 LBEQ cc97
 LDD  8,S
 PSHS D
 LDD  8,S
 PSHS D
 LEAY 8,S
 TFR  Y,D
 PSHS D
 JSR init
 LEAS 6,S
 LDD  #cc87+4
 PSHS D
 JSR match
 LEAS 2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc98
 JMP cc97
cc98
 JMP cc96
cc97
 LDD  #cc87+6
 PSHS D
 JSR needbrac
 LEAS 2,S
 JMP cc99
cc95
 LDD  8,S
 PSHS D
 LDD  8,S
 PSHS D
 LEAY 8,S
 TFR  Y,D
 PSHS D
 JSR init
 LEAS 6,S
cc99
cc94
 LDD  4,S
 PSHS D
 LDD  #1
 COMA
 COMB
 ADDD #1
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc101 _
 LDD  #1
 LDD  4,S
 PSHS D
 LDD  2,S
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc101 _
 LDD  #1
 LDD  #1
cc101
 CMPD #0
 LBEQ cc100
 LDD  #0
 PSHS D
 LDD  #2
 STD  10,S
 PSHS D
 JSR stowlit
 LEAS 4,S
 LDD  #3
 STD  6,S
cc100
 LDD  8,S
 PSHS D
 JSR dumplits
 LEAS 2,S
 LDD  8,S
 PSHS D
 LDD  6,S
 PULS X
 JSR ccmult
 PSHS D
 JSR dumpzero
 LEAS 2,S
 LDD  6,S
 LEAS 2,S
 RTS
cc87 FCB 61,0,123,0,44,0,125,0
 GLOBAL init
init
 LEAS -2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR qstr
 LEAS 2,S
 CMPD #0
 LBEQ cc103
 LDD  6,S
 PSHS D
 LDD  #1
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #0
 LBNE cc105
 LDD  8,S
 PSHS D
 LDD  #1
 CMPD ,S++
 BNE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #0
 LBNE cc105
 LDD  #0
 JMP cc106
cc105
 LDD  #1
cc106
 CMPD #0
 LBEQ cc104
 LDD  #cc102+0
 PSHS D
 JSR errrpt
 LEAS 2,S
cc104
 LDD  4,S
 PSHS D
 LDD  litptr
 PSHS D
 LDD  4,S
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 COMA
 COMB
 ADDD #1
 PSHS D
 LDD  8,S
 PSHS D
 LDD  [,S++]
 ADDD ,S++
 STD  [,S++]
 LDD  6,S
 PSHS D
 LDD  #3
 CMPD ,S++
 LBNE cc107
 JSR point
cc107
 JMP cc108
cc103
 LEAY 0,S
 TFR  Y,D
 PSHS D
 JSR constexp
 LEAS 2,S
 CMPD #0
 LBEQ cc109
 LDD  6,S
 PSHS D
 LDD  #3
 CMPD ,S++
 LBNE cc110
 LDD  #cc102+37
 PSHS D
 JSR errrpt
 LEAS 2,S
cc110
 LDD  0,S
 PSHS D
 LDD  10,S
 PSHS D
 JSR stowlit
 LEAS 4,S
 LDD  4,S
 PSHS D
 LDD  #1
 COMA
 COMB
 ADDD #1
 PSHS D
 LDD  8,S
 PSHS D
 LDD  [,S++]
 ADDD ,S++
 STD  [,S++]
cc109
cc108
 LEAS 2,S
 RTS
cc102 FCB 109,117,115,116,32,97,115,115,105,103
 FCB 110,32,116,111,32,99,104,97,114,32
 FCB 112,111,105,110,116,101,114,32,111,114
 FCB 32,97,114,114,97,121,0,99,97,110
 FCB 110,111,116,32,97,115,115,105,103,110
 FCB 32,116,111,32,112,111,105,110,116,101
 FCB 114,0
 END
