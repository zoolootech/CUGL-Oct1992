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
 NAME toupper
 GLOBAL toupper
toupper
 LDB  3,S
 SEX
 PSHS D
 JSR islower
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
 PSHS D
 LDD  #32
 PULS X
 PSHS D
 TFR  X,D
 SUBD ,S++
 RTS
cc2
 LDB  3,S
 SEX
 RTS
cc3
 RTS
 END
