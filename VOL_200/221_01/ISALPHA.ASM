;small-c V2.3
 NAME isalpha
 GLOBAL isalpha
isalpha
 LDB  3,S
 SEX
 PSHS D
 LDD  #97
 CMPD ,S++
 BLE  *+8
 LDD  #0
 JMP cc2 _
 LDD  #1
 LDB  3,S
 SEX
 PSHS D
 LDD  #122
 CMPD ,S++
 BGE  *+8
 LDD  #0
 JMP cc2 _
 LDD  #1
 LDD  #1
cc2
 CMPD #1
 LBEQ cc3
 LDB  3,S
 SEX
 PSHS D
 LDD  #65
 CMPD ,S++
 BLE  *+8
 LDD  #0
 JMP cc4 _
 LDD  #1
 LDB  3,S
 SEX
 PSHS D
 LDD  #90
 CMPD ,S++
 BGE  *+8
 LDD  #0
 JMP cc4 _
 LDD  #1
 LDD  #1
cc4
 CMPD #1
 LBEQ cc3
 LDD  #0
 JMP cc5
cc3
 LDD  #1
cc5
 RTS
 END
