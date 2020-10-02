;small-c V2.3
 NAME tolower
 GLOBAL tolower
tolower
 LDB  3,S
 SEX
 PSHS D
 JSR isupper
 LEAS 2,S
 PSHS D
 LDB  5,S
 SEX
 PSHS D
 JSR isalpha
 LEAS 2,S
 ANDA ,S+
 ANDB ,S+
 CMPD #0
 LBEQ cc2
 LDB  3,S
 SEX
 ADDD #32
 RTS
cc2
 LDB  3,S
 SEX
 RTS
cc3
 RTS
 END
