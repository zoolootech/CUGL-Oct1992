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
 NAME islower
 GLOBAL islower
islower
 LDB  3,S
 SEX
 PSHS D
 LDD  #97
 CMPD ,S++
 BLE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 PSHS D
 LDB  5,S
 SEX
 PSHS D
 LDD  #122
 CMPD ,S++
 BGE  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 ANDA ,S+
 ANDB ,S+
 RTS
 END
