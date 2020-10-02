;small-c V2.3
 EXT exit
 EXT ccmult
 EXT ccdiv
 EXT putc
 EXT fputs
 NAME printf
 GLOBAL printf
printf
 LEAS -28,S
 STD ,S
 LEAY 18,S
 TFR  Y,D
 PSHS D
 LEAY 32,S
 TFR  Y,D
 PSHS D
 LDD  4,S
 PSHS D
 LDD  #1
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 PSHS D
 LDD  #1
 DECB
 BLT  *+8
 ASL  1,S
 ROL  ,S
 BRA  *-7
 PULS D
 ADDD ,S++
 STD  [,S++]
 LEAY 16,S
 TFR  Y,D
 PSHS D
 LDD  20,S
 PSHS D
 LDD  [,S++]
 STD  [,S++]
cc2
 LEAY 13,S
 TFR  Y,D
 PSHS D
 LDD  18,S
 ADDD #1
 STD  18,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 CMPD #0
 LBEQ cc3
 LDB  13,S
 SEX
 PSHS D
 LDD  #37
 CMPD ,S++
 LBEQ cc4
 LDB  13,S
 SEX
 PSHS D
 LDD  #1
 PSHS D
 JSR cout
 LEAS 4,S
 JMP cc2
cc4
 LDD  16,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #37
 CMPD ,S++
 LBNE cc5
 LDD  16,S
 ADDD #1
 STD  16,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
 PSHS D
 JSR cout
 LEAS 4,S
 JMP cc2
cc5
 LEAY 14,S
 TFR  Y,D
 PSHS D
 LDD  18,S
 STD  [,S++]
 LDD  14,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #45
 CMPD ,S++
 LBNE cc6
 LEAY 12,S
 TFR  Y,D
 PSHS D
 LDD  #0
 STB  [,S++]
 LDD  14,S
 ADDD #1
 STD  14,S
 JMP cc7
cc6
 LEAY 12,S
 TFR  Y,D
 PSHS D
 LDD  #1
 STB  [,S++]
cc7
 LDD  14,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #48
 CMPD ,S++
 LBNE cc8
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #48
 STB  [,S++]
 LDD  14,S
 ADDD #1
 STD  14,S
 JMP cc9
cc8
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #32
 STB  [,S++]
cc9
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  16,S
 PSHS D
 LEAY 30,S
 TFR  Y,D
 PSHS D
 JSR utoi
 LEAS 4,S
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBGT cc10
 LEAY 14,S
 TFR  Y,D
 PSHS D
 LDD  16,S
 PSHS D
 LDD  4,S
 ADDD ,S++
 STD  [,S++]
 JMP cc11
cc10
 JMP cc2
cc11
 LDD  14,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #46
 CMPD ,S++
 LBNE cc12
 LEAY 22,S
 TFR  Y,D
 PSHS D
 LDD  16,S
 ADDD #1
 STD  16,S
 PSHS D
 LEAY 28,S
 TFR  Y,D
 PSHS D
 JSR utoi
 LEAS 4,S
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBGT cc13
 LEAY 14,S
 TFR  Y,D
 PSHS D
 LDD  16,S
 PSHS D
 LDD  26,S
 ADDD ,S++
 STD  [,S++]
 JMP cc14
cc13
 JMP cc2
cc14
 JMP cc15
cc12
 LDD  #0
 STD  22,S
cc15
 LEAY 3,S
 TFR  Y,D
 PSHS D
 LEAY 7,S
 TFR  Y,D
 STD  [,S++]
 LEAY 13,S
 TFR  Y,D
 PSHS D
 LDD  16,S
 ADDD #1
 STD  16,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LEAY 20,S
 TFR  Y,D
 PSHS D
 PSHS D
 LDD  [,S++]
 SUBD #1
 SUBD #1
 STD  [,S++]
 PSHS D
 LDD  [,S++]
 STD  [,S++]
 LDB  13,S
 SEX
 PSHS D
 LDD  #100
 CMPD ,S++
 LBNE cc16
 LDD  0,S
 PSHS D
 LEAY 7,S
 TFR  Y,D
 PSHS D
 LDD  #7
 PSHS D
 JSR itod
 LEAS 6,S
 JMP cc17
cc16
 LDB  13,S
 SEX
 PSHS D
 LDD  #120
 CMPD ,S++
 LBNE cc18
 LDD  0,S
 PSHS D
 LEAY 7,S
 TFR  Y,D
 PSHS D
 LDD  #7
 PSHS D
 JSR itox
 LEAS 6,S
 JMP cc19
cc18
 LDB  13,S
 SEX
 PSHS D
 LDD  #99
 CMPD ,S++
 LBNE cc20
 LEAY 5,S
 TFR  Y,D
 ADDD #0
 PSHS D
 LDD  2,S
 STB  [,S++]
 LEAY 5,S
 TFR  Y,D
 ADDD #1
 PSHS D
 LDD  #0
 STB  [,S++]
 JMP cc21
cc20
 LDB  13,S
 SEX
 PSHS D
 LDD  #115
 CMPD ,S++
 LBNE cc22
 LEAY 3,S
 TFR  Y,D
 PSHS D
 LDD  2,S
 STD  [,S++]
 JMP cc23
cc22
 LDB  13,S
 SEX
 PSHS D
 LDD  #117
 CMPD ,S++
 LBNE cc24
 LDD  0,S
 PSHS D
 LEAY 7,S
 TFR  Y,D
 PSHS D
 LDD  #7
 PSHS D
 JSR itou
 LEAS 6,S
 JMP cc25
cc24
 JMP cc2
cc25
cc23
cc21
cc19
cc17
 LEAY 16,S
 TFR  Y,D
 PSHS D
 LDD  16,S
 STD  [,S++]
 LDB  13,S
 SEX
 PSHS D
 LDD  #115
 CMPD ,S++
 LBEQ cc26
cc27
 LDD  3,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #32
 CMPD ,S++
 LBNE cc28
 LDD  3,S
 ADDD #1
 STD  3,S
 JMP cc27
cc28
cc26
 LEAY 20,S
 TFR  Y,D
 PSHS D
 LDD  #1
 COMA
 COMB
 ADDD #1
 STD  [,S++]
cc29
 LDD  3,S
 PSHS D
 LDD  22,S
 ADDD #1
 STD  22,S
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 CMPD #0
 LBEQ cc30
 JMP cc29
cc30
 LDB  13,S
 SEX
 PSHS D
 LDD  #115
 CMPD ,S++
 BEQ  *+8
 LDD  #0
 JMP cc32 _
 LDD  #1
 LDD  20,S
 PSHS D
 LDD  26,S
 CMPD ,S++
 BLT  *+8
 LDD  #0
 JMP cc32 _
 LDD  #1
 LDD  #1
cc32
 CMPD #0
 LBEQ cc33 _
 LDD  22,S
 PSHS D
 LDD  #0
 CMPD ,S++
 BLT  *+8
 LDD  #0
 JMP cc33 _
 LDD  #1
 LDD  #1
cc33
 CMPD #0
 LBEQ cc31
 LEAY 20,S
 TFR  Y,D
 PSHS D
 LDD  26,S
 STD  [,S++]
cc31
 LDB  12,S
 SEX
 CMPD #0
 LBEQ cc34
cc35
 LDD  26,S
 SUBD #1
 STD  26,S
 ADDD #1
 PSHS D
 LDD  22,S
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 PSHS D
 LDD  #0
 CMPD ,S++
 LBGE cc36
 LDB  2,S
 SEX
 PSHS D
 LDD  #1
 PSHS D
 JSR cout
 LEAS 4,S
 JMP cc35
cc36
cc34
cc37
 LDD  20,S
 CMPD #0
 LBEQ cc38
 LDD  3,S
 ADDD #1
 STD  3,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #1
 PSHS D
 JSR cout
 LEAS 4,S
 LDD  20,S
 SUBD #1
 STD  20,S
 LDD  26,S
 SUBD #1
 STD  26,S
 JMP cc37
cc38
cc39
 LDD  26,S
 SUBD #1
 STD  26,S
 ADDD #1
 PSHS D
 LDD  22,S
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 PSHS D
 LDD  #0
 CMPD ,S++
 LBGE cc40
 LDB  2,S
 SEX
 PSHS D
 LDD  #1
 PSHS D
 JSR cout
 LEAS 4,S
 JMP cc39
cc40
 JMP cc2
cc3
 LEAS 28,S
 RTS
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
 LBNE cc41
 JSR xout
cc41
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
 LBNE cc42
 JSR xout
cc42
 RTS
 GLOBAL lout
lout
 LDD  4,S
 PSHS D
 LDD  4,S
 PSHS D
 JSR sout
 LEAS 4,S
 LDD  #13
 PSHS D
 LDD  4,S
 PSHS D
 JSR cout
 LEAS 4,S
 RTS
 GLOBAL xout
xout
 LDD  #cc1+0
 PSHS D
 LDD  #2
 PSHS D
 JSR fputs
 LEAS 4,S
 LDD  #99
 PSHS D
 JSR exit
 LEAS 2,S
 RTS
cc1 FCB 111,117,116,112,117,116,32,101,114,114
 FCB 111,114,13,0
 END
