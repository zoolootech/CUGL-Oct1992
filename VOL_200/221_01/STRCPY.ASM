;small-c V2.3
 NAME strcpy
 GLOBAL strcpy
strcpy
cc2
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
 LBEQ cc3
 JMP cc2
cc3
 RTS
 END
