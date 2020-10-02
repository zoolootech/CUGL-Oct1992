;small-c V2.3
 NAME left
 GLOBAL left
left
 LEAS -2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  6,S
 STD  [,S++]
cc2
 LDD  0,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #32
 CMPD ,S++
 LBNE cc3
 LDD  0,S
 ADDD #1
 STD  0,S
 JMP cc2
cc3
cc4
 LDD  4,S
 ADDD #1
 STD  4,S
 SUBD #1
 PSHS D
 LDD  2,S
 ADDD #1
 STD  2,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 CMPD #0
 LBEQ cc5
 JMP cc4
cc5
 LEAS 2,S
 RTS
 END
