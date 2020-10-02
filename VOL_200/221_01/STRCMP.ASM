;small-c V2.3
 NAME strcmp
 GLOBAL strcmp
strcmp
cc4
 LDD  4,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  4,S
 PSHS D
 LDB  [,S++]
 SEX
 CMPD ,S++
 LBNE cc3
 JMP cc5
cc2
 LDD  4,S
 ADDD #1
 STD  4,S
 LDD  2,S
 ADDD #1
 STD  2,S
 JMP cc4
cc5
 LDD  4,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  #0
 CMPD ,S++
 LBNE cc6
 LDD  #0
 RTS
cc6
 JMP cc2
cc3
 LDD  4,S
 PSHS D
 LDB  [,S++]
 SEX
 PSHS D
 LDD  4,S
 PSHS D
 LDB  [,S++]
 SEX
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 RTS
 END
