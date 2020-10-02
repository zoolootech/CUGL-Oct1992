/* CTRLC.H - Header file for The Ctrl+C Library of Interrupt Routines  */
/*
** CTRLCLIB - The CTRL-C Library(Tm)
** Copyright 1990 Trendtech Corporation, Inc.
** All Rights Reserved Worldwide
*/

#ifndef __CTRLCH__
#define __CTRLCH__


#undef MSC
#ifdef MSC6
#undef MSC5 
#undef QC25
#undef TC20
#define MSC
#endif

#ifdef MSC5
#undef MSC6 
#undef QC25
#undef TC20
#define MSC
#endif

#ifdef QC25
#undef MSC5 
#undef MSC6
#undef TC20
#define MSC
#endif

#if defined( TC20 ) || defined( TCXX )
#ifndef __TURBOC__
#define __TURBOC__
#endif
#undef MSC5 
#undef MSC6
#undef QC25
#undef MSC
#define _interrupt   interrupt
#define _dos_getvect getvect
#define _dos_setvect setvect
#define _outp        outportb
#define _disable     disable
#endif



/* Function Prototypes:  */

extern int  CtrlcLibSetup(unsigned int);
extern void CtrlcLibCleanup(void);
extern void CtrlBreak(int);          
extern void CtrlC(int);              
extern void FullCtrlC(int);          
extern unsigned int QueryCtrlC(void);        
extern unsigned int QueryPrtScr(void);        
extern unsigned int QuerySysReq(void);
extern unsigned int QueryKeyPad5(void);
extern void KeyboardHandler(int);
extern void SysReq(int);              
extern void CtrlAltDel(int);         
extern void PrintScreen(int);      
extern void PauseKey(int);         
extern void CtrlS(int);         
extern void CtrlP(int);         
extern void SystemReset(int);
extern void ShowCtrlC(void);
extern int  CapsLock(int);   
extern int  NumLock(int);    
extern int  ScrollLock(int); 
extern int  InsertLock(int); 

#define L_OFF          0x01
#define L_ON           0x02
#define L_ALWAYSOFF    0x04
#define L_ALWAYSON     0x08
#define L_TOGGLE       0x10

#define KB_FLAG_ADDR          0x00000417  /*  Keyboard flags address */

#ifdef TC20
#define FARNULL               NULL
#else
#define FARNULL               (unsigned far *)NULL
#endif

#define DISABLE               1           /*  generic function action */
#define ENABLE                2           /*  this too */

#define INSTALL               DISABLE     /*  generic function action */
#define REMOVE                ENABLE      /*  this too */

#define COLDBOOT              0x0000
#define WARMBOOT              0x1234

#define GETBREAKFLAG          1           /*  control-C flag action */
#define SETBREAKFLAG          2           /*  this too */

#define CTRLCINT              0x23        /*  return value from QUERY */
#define CTRLBRKINT            0x1B        /*  return value from QUERY */

#define SCRLPRS               0x1010      /*  masks LOCK keys as pressed for always on*/
#define NUMLPRS               0x2020
#define CAPLPRS               0x4040

#define SCRLMASK              0x0010      /*  masks for toggling LOCK keys */
#define NUMLMASK              0x0020
#define CAPLMASK              0x0040
#define INSLMASK              0x0080


/*  Definitions for installed traps and interrupts */

/*  Ints needing KB handler: */
#define KEYBOARDINT        0x0001
#define FCTRLC             0x0002
#define CTRLALTDEL         0x0004
#define CTRLS              0x0010
#define CTRLP              0x0020
#define CTRLPRTSCR         0x0040
#define SYSREQ             0x0080

/*  Ints that don't need the KB handler: */
#define CTRLC              0x0100
#define CTRLBREAK          0x0200
#define PRINTSCREEN        0x0400
#define SYSTEMBOOT         0x0800
#define CTRL2              0x1000
#define ALT3               0x2000
#define PAUSEKEY           0x4000
#define spare4             0x8000


/* Global variables */
/* these must be DEFINED in source containing the main() function */
/* and DECLARED in all other source modules */


extern unsigned int KB_Make, KB_Break;

extern struct ctrlclibflags
   {
   unsigned int KBints;    /*  bit map of INTS requiring INT 09 handler */
                           /*  bit 0 = KEYBOARD  */
                           /*  bit 1 = Full CTRLC trapped (capture ^C) */
                           /*  bit 2 = CTRL+ALT+DEL trapped */
                           /*  bit 3 = "AlwaysFlag" 1=set; 0=not set */
                           /*  bit 4 = CTRL+S trapped (DOS screen pause) */
                           /*  bit 5 = CTRL+P trapped (Print Screen) */
                           /*  bit 6 = AlwaysFlag trapped */
                           /*  bit 7 = SysReq trapped (system request) */


   unsigned int Ints;      /*  bit map of INT handlers NOT requiring KB handler */
                           /*  bit 8 =  CTRLC trapped */
                           /*  bit 9 =  CTRL-BREAK trapped */
                           /*  bit 10 = PRINT SCREEN trapped */
                           /*  bit 11 = Warm Boot */
   unsigned int MakeBreak;
   unsigned int BreakFlag;
   unsigned int BootFlag;
   unsigned int PauseFlag;
   unsigned int PrtScrFlag;
   unsigned int SysReqFlag;
   unsigned int KeyPad5Flag;

   unsigned int AlwaysFlag;/*  bit field for LOCK'ing keys */
                           /*  bit 1 = Scroll Lock */
                           /*  bit 2 = Num Lock */
                           /*  bit 3 = Caps Lock */
                           /*  bit 4 = Insert Key */
   unsigned int scrALWAYSMASK;
   unsigned int numALWAYSMASK;
   unsigned int capALWAYSMASK;
   unsigned int insALWAYSMASK;

   } cl;      /* all variables initialized in CL_COMM.C */

#endif  /* __CTRLCH__ */

/* ---------------------------END OF HEADER------------------------------- */

