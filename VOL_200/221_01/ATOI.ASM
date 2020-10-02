;small-c V2.3
 EXT ccmult
 NAME atoi
 GLOBAL atoi
atoi
 LEAS -4,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  #0
 STD  4,S
 STD  [,S++]
 LDD  6,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #43
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc3
 LDD  6,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #45
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
 LDD  6,S
 ADDD #1
 STD  6,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #45
 CMPD ,S++
 LBNE cc5
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  2,S
 COMA
 COMB
 ADDD #1
 STD  [,S++]
cc5
cc2
cc6
 LDD  6,S
 PSHS D
 LDB  [,S++]
 SEX
 CMPD #0
 LBEQ cc7
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  #10
 PSHS D
 LDD  6,S
 PULS X
 JSR ccmult
 PSHS D
 LDD  10,S
 ADDD #1
 STD  10,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 ADDD ,S++
 PSHS D
 LDD  #48
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  [,S++]
 JMP cc6
cc7
 LDD  0,S
 CMPD #0
 LBEQ cc8
 LDD  2,S
 COMA
 COMB
 ADDD #1
 LEAS 4,S
 RTS
cc8
 LDD  2,S
 LEAS 4,S
 RTS
 END
