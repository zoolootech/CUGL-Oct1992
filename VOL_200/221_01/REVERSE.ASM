;small-c V2.3
 NAME reverse
 GLOBAL reverse
reverse
 LEAS -6,S
 LDD  #0
 STD  2,S
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  10,S
 PSHS D
 JSR strlen
 LEAS 2,S
 PSHS D
 LDD  #1
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 STD  [,S++]
cc4
 LDD  2,S
 PSHS D
 LDD  2,S
 CMPD ,S++
 LBLE cc3
 JMP cc5
cc2
 LDD  2,S
 ADDD #1
 STD  2,S
 LDD  0,S
 SUBD #1
 STD  0,S
 JMP cc4
cc5
 LEAY 4,S
 TFR  Y,D
 PSHS D
 LDD  10,S
 PSHS D
 LDD  6,S
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 STD  [,S++]
 LDD  8,S
 PSHS D
 LDD  4,S
 ADDD ,S++
 PSHS D
 LDD  10,S
 PSHS D
 LDD  4,S
 ADDD ,S++
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 LDD  8,S
 PSHS D
 LDD  2,S
 ADDD ,S++
 PSHS D
 LDD  6,S
 STB  [,S++]
 JMP cc2
cc3
 LEAS 6,S
 RTS
 END
