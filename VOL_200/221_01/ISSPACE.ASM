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
 NAME isspace
 GLOBAL isspace
isspace
 LDB  3,S
 SEX
 PSHS D
 LDD  #32
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc2
 LDB  3,S
 SEX
 PSHS D
 LDD  #9
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc2
 LDD  #0
 JMP cc3
cc2
 LDD  #1
cc3
 CMPD #1
 LBEQ cc4
 LDB  3,S
 SEX
 PSHS D
 LDD  #13
 CMPD ,S++
 BEQ  *+7
 LDD  #0
 BRA  *+5
 LDD  #1
 CMPD #1
 LBEQ cc4
 LDD  #0
 JMP cc5
cc4
 LDD  #1
cc5
 RTS
 END
