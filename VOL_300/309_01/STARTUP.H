/* header.h  used to generate startup code for C6809 */

#ifdef AS9
#asm
   ORG  $fffe
   FDB  $c000
   ORG  $c000
   lds  #$7000
   jmp  main
#endasm
#else
#asm
   .globl  main
   .globl  ccdiv
   .globl  ccmult
   .globl  ccswitch

   .area   RESET    (ABS,OVR)
   .org    0hfffe       ; reset vector
   .dw     0hc000

   .area   RAMDATA  (REL,CON)

   .area   ROMCODE  (REL,CON)
   lds     #0h7000
   jmp     main
#endasm
#endif
