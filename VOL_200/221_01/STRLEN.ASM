;small-c V2.3
 NAME strlen
 GLOBAL strlen
strlen
 LEAS -2,S
 LDD  #0
 STD  0,S
cc2
 LDD  4,S
 ADDD #1
 STD  4,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 CMPD #0
 LBEQ cc3
 LDD  0,S
 ADDD #1
 STD  0,S
 JMP cc2
cc3
 LDD  0,S
 LEAS 2,S
 RTS
 END
