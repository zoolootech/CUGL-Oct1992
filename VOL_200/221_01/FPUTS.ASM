;small-c V2.3
 EXT ccdiv,ccmult
 EXT fopen,fclose,getc,putc,putchar,getchar,rewind,gets
 EXT exit
 EXT CCSWITCH
 EXT CCPOLL
 EXT isalpha,isupper,islower,isdigit,isspace
 EXT toupper,tolower
 EXT strlen,reverse,left,strcmp,strcpy
 EXT abs,itoa,atoi,printf
 NAME fputs
 GLOBAL fputs
fputs
 LEAS -2,S
cc2
 LEAY 0,S
 TFR  Y,D
 PSHS D
 LDD  8,S
 ADDD #1
 STD  8,S
 SUBD #1
 PSHS D
 LDB  [,S++]
 SEX
 STD  [,S++]
 CMPD #0
 LBEQ cc3
 LDD  0,S
 PSHS D
 LDD  6,S
 PSHS D
 JSR putc
 LEAS 4,S
 JMP cc2
cc3
 LEAS 2,S
 RTS
 END
