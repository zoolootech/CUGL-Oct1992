;small-c V2.3
 NAME strcat
 GLOBAL strcat
strcat
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
 JMP cc2
cc3
 LDD  4,S
 SUBD #1
 STD  4,S
cc4
 LDD  4,S
 ADDD #1
 STD  4,S
 SUBD #1
 PSHS D
 LDD  4,S
 ADDD #1
 STD  4,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 STB  [,S++]
 CMPD #0
 LBEQ cc5
 JMP cc4
cc5
 RTS
 END
