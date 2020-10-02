;small-c V2.3
 NAME abs
 GLOBAL abs
abs
 LDD  2,S
 PSHS D
 LDD  #0
 CMPD ,S++
 LBLE cc2
 LDD  2,S
 COMA
 COMB
 ADDD #1
 RTS
cc2
 LDD  2,S
 RTS
cc3
 RTS
 END
