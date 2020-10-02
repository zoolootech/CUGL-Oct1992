;/* --- Version 2.2 90-10-12 16:37 ---
   COMMENT  ^

   TSKDEB.H - CTask - Debug definitions 

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   This file is new with version 2.1.

   DEBUG          Enables debugging output.
                  Use the following flags:

                  DEB_SECOND     Display debug output on secondary monitor

                  DEB_TSKDIS     Display current,previous,eligible tasks
                                 when in scheduler

                  DEB_FLASHERS   Display counters for scheduler, ticker,
                                 keyboard ints

   Sample definition:

         #define DEBUG     (DEB_TSKDIS | DEB_FLASHERS)
         DEBUG          =  DEB_TSKDIS OR DEB_FLASHERS

*/

#define  DEB_TSKDIS        0x0001
#define  DEB_FLASHERS      0x0002
#define  DEB_SECOND        0x8000

#define  DEBUG             0 /*^

         DEB_TSKDIS     =  0001h
         DEB_FLASHERS   =  0002h
         DEB_SECOND     =  8000h

         DEBUG          =  0

COMMENT ^
*/

/* ----------------------------------------------------------------------- */

/*
Task display:
          1         2         3         4         5         6         7
01234567890123456789012345678901234567890123456789012345678901234567890123456789
 Current   Previous  Eligible  Next el.  Sched Idle  Ticki Tickc Keybi StOfl S
*<name..>  <name..>  <name..>  <name..>  <cnt> <cnt> <cnt> <cnt> <cnt> <cnt> n
*/

#define DEBLINTD " Current   Previous  Eligible  Next el."
#define DEBLINFL "Sched Idle  Ticki Tickc Keybi StOfl S"
#define DEBINIFL "00000 00000 00000 00000 00000 00000 0"

#define  DEBFLASH_NDIGS    5

#define  DEBP_CURRTSK      (1*2)
#define  DEBP_PREVTSK      (11*2)
#define  DEBP_ELIGTSK      (21*2)
#define  DEBP_NEXTELIG     (31*2)
#define  DEBP_CNTSCHED     (41*2)
#define  DEBP_CNTIDLE      (47*2)
#define  DEBP_CNTTICK      (53*2)
#define  DEBP_CNTTCHAIN    (59*2)
#define  DEBP_CNTKEYBD     (65*2)
#define  DEBP_CNTSTOFL     (71*2)
#define  DEBP_STACKNUM     (77*2)  /*^

         DEBFLASH_NDIGS =  5

         DEBP_CURRTSK   =  (1*2)
         DEBP_PREVTSK   =  (11*2)
         DEBP_ELIGTSK   =  (21*2)
         DEBP_NEXTELIG  =  (31*2)
         DEBP_CNTSCHED  =  (41*2)
         DEBP_CNTIDLE   =  (47*2)
         DEBP_CNTTICK   =  (53*2)
         DEBP_CNTTCHAIN =  (59*2)
         DEBP_CNTKEYBD  =  (65*2)
         DEBP_CNTSTOFL  =  (71*2)
         DEBP_STACKNUM  =  (77*2)

;*/

