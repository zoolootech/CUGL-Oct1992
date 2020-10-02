;small-c V2.3
 NAME itoa
 GLOBAL itoa
itoa
 LEAS -4,S
 LEAY 2,S
 TFR  Y,D
 PSHS D
 LDD  10,S
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBLE cc2
 LEAY 8,S
 TFR  Y,D
 PSHS D
 LDD  10,S
 COMA
 COMB
 ADDD #1
 STD  [,S++]
cc2
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 STD  [,S++]
cc5
 LDD  6,S
 ADDD #1
 STD  6,S
 SUBD #1
 PSHS D
 LDD  10,S
 PSHS D
 LDD  #10
 PULS X
 JSR ccdiv
 EXG  D,X
 ADDD #48
 STB  [,S++]
cc3
 LEAY 8,S
 TFR  Y,D
 PSHS D
 LDD  10,S
 PSHS D
 LDD  #10
 PULS X
 JSR ccdiv
 STD  [,S++]
 PSHS D
 LDD  #0
 CMPD ,S++
 LBGE cc4
 JMP cc5
cc4
 LDD  2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBLE cc6
 LDD  6,S
 ADDD #1
 STD  6,S
 SUBD #1
 PSHS D
 LDD  #45
 STB  [,S++]
cc6
 LDD  6,S
 PSHS D
 LDD  #0
 STB  [,S++]
 LDD  0,S
 PSHS D
 JSR reverse
 LEAS 2,S
 LEAS 4,S
 RTS
 END
