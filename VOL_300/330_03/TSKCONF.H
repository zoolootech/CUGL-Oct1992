;/* --- Version 2.2 90-10-12 16:16 ---
COMMENT ^

   TSKCONF.H - CTask - Configuration definitions 

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   This file has been changed in version 2.1 to allow inclusion in 
   both the C and the Assembler parts. This avoids confusion and hard
   to find errors due to mismatched configurations. It's still possible
   to make errors, but since everything is close together, it's easier 
   to spot the problem. Please be extra careful to ensure that the
   C and Assembler definitions agree.
*/

#define  FALSE             0
#define  TRUE              1       /*^
         FALSE          =  0
         TRUE           =  0ffffh  ; Note: Assembler definition is different

COMMENT ^

   CODE_SHARING   If TRUE, the generated kernel supports code sharing.
                  This requires the entry points to load ds on entry,
                  and compilation with Large model (MSC) or Huge model (TC).
                  The default is FALSE.
                  
   NEAR_CODE      If TRUE, all CTask routines are 'near'.
                  Use only with small model.
                  You will have to change the make-files so the
                  compiler model is no longer Large, and the code segment
                  is not named, when turning on this flag.
                  The default is FALSE.

   LOCALS_FAR     If TRUE, internal CTask routines ar 'far'.
                  This might be necessary if your compiler/linker does
                  not allow placing all CTask kernel code in a common
                  segment.
                  Do not set this flag if NEAR_CODE is set.
                  The default is FALSE.

   CALL_PASCAL    Use Pascal calling sequence for CTask routines.
                  This may save some code, but may cause naming
                  conflicts if your compiler limits external Pascal
                  names to 8 characters.
                  The default is FALSE.

   TC_HUGE        (Assembler only)
                  Define TC_HUGE for use with the Turbo C Huge
                  model, and if it is desired to separate CTask's data
                  from the normal data segment.
                  This flag causes the CTASK_DATA segment to be defined,
                  (class is DATA) and DS to be loaded with this segment
                  on function entry. 
                  The C functions in the CTask kernel must be compiled
                  to use the same data segment.
                  The default is not defined.

   LOAD_DS        (Assembler only)
                  Define LOAD_DS to reload the data segment on entry
                  to global functions.
                  This flag must be defined for TC_HUGE. It can also be
                  defined if the data segment can not safely be assumed to be
                  loaded in DS on function entry, for example if the code
                  sharing feature of version 2.1 is used. The C routines
                  must be compiled with the necessary compiler switches
                  or the _loadds keyword in this case.
                  The default is not defined.

   ROM_CODE       (Assembler only)
                  Define ROM_CODE TRUE for embedded systems using ROM-
                  based code. This option disables storing variables in
                  the code segment.
                  The default is FALSE.

   FAR_STACK      (Assembler only)
                  If FAR_STACK is defined TRUE, the local stacks used
                  by CTask interrupt handlers are allocated outside the
                  default data segment. This may require changes in the
                  standard start-up module to make sure the segment 
                  ordering is correct. The Turbo C linker may be confused
                  and place the CTask-stack segment in the middle of the
                  DGROUP if no mention is made of the segment in C0.ASM.
                  The default is FALSE.

      CAUTION:    Changing requires recompile of all sources.
*/

#define  CODE_SHARING      FALSE
#define  NEAR_CODE         FALSE
#define  LOCALS_FAR        FALSE
#define  CALL_PASCAL       FALSE    /*^
                        
         CODE_SHARING   =  FALSE
         NEAR_CODE      =  FALSE
         LOCALS_FAR     =  FALSE
         CALL_PASCAL    =  FALSE

         ROM_CODE       =  FALSE    ; ASM only
         FAR_STACKS     =  FALSE    ; ASM only

; The following two defines may be defined on the command line (any value)
; or uncommented here. To disable, don't set to FALSE, comment them out.
; Setting TC_HUGE is sufficient to enable both options.

;        TC_HUGE        =  1     ; ASM only
;        LOAD_DS        =  1     ; ASM only
         IFDEF    TC_HUGE
LOAD_DS  =  1
         ENDIF

COMMENT ^

   TSK_DYNAMIC    if TRUE, creating dynamic control blocks is allowed.
                  This option should normally be disabled in embedded
                  systems. Disabling the option will also save some
                  code, and eliminate the (model dependent) references
                  to the C memory allocation routines. This could be
                  useful for TSR or spawning kernels, and if you don't
                  use the dynamic allocation feature anyway.
                  The tsksio module requires TSK_DYNAMIC to be set
                  if on-line definition of new ports is desired.
                  The default is TRUE.

      CAUTION:    Changing requires recompile of all sources.

   TSK_DYNLOAD    if FALSE, this instance of the kernel does not include
                  dynamic allocation routines.
                  Setting this flag to FALSE when TSK_DYNAMIC is TRUE
                  only makes sense with multiple linked kernels. A resident
                  kernel might not employ dynamic allocation, whereas a
                  secondary kernel needs it. In this situation, TSK_DYNAMIC
                  must be set in the primary so that the task kill code
                  is included and the kernel configurations match, but 
                  TSK_DYNLOAD may be false to prevent inclusion of the 
                  malloc/free run-time routines.
                  This option has no effect if TSK_DYNAMIC is FALSE.
                  The default is TRUE.

      CAUTION:    Changing requires recompile of tskinst.c and tskgrp.c.
*/

#define  TSK_DYNAMIC       TRUE
#define  TSK_DYNLOAD       TRUE    /*^

         TSK_DYNAMIC    =  TRUE
         TSK_DYNLOAD    =  TRUE

COMMENT ^

   IBM            if TRUE, routines specific to the IBM family of PCs
                  (and clones) are installed.
                  The default is TRUE.

   DOS            if TRUE, routines specific to PC/MS-DOS are installed.
                  The default is TRUE.

   NOTE:          If you disable IBM, you have to provide timer and keyboard
                  hardware support yourself. Check the areas in "tskmain.c" 
                  that are affected by this define.
                  If you disable DOS, the resulting version can not be
                  used under DOS without significant restrictions.
                  The program can be tested, but DOS is not protected
                  from multiple entrance, so that concurrent DOS calls
                  will crash the system. You can use this configuration
                  (IBM true, DOS false) to debug embedded applications
                  which do not use DOS calls under DOS.

      CAUTION:    Changing requires recompile of all sources.
*/

#define  IBM               TRUE
#define  DOS               TRUE     /*^

         IBM            =  TRUE 
         DOS            =  TRUE 

COMMENT ^

   TSK_NAMEPAR    if TRUE, the create_xxx routines accept a name parameter.
                  The default is TRUE.

   TSK_NAMED      if TRUE, control block names are processed.
                  The default is TRUE.

                  TSK_NAMED may only be enabled (TRUE) if TSK_NAMEPAR is also 
                  enabled. The TSK_NAMEPAR option is provided to allow
                  compatible calling sequences. If TSK_NAMED is FALSE, and
                  TSK_NAMEPAR is TRUE, the name parameter will be ignored.
                  Using TSK_NAMED is highly reommended for DOS applications.

   GROUPS         if TRUE, multiple task groups are supported.
                  For this option to work, TSK_NAMEPAR and TSK_NAMED must
                  be enabled. Allowing task groups is strongly recommended
                  for DOS applications (the standard version requires it), 
                  embedded systems will usually not use task groups.
                  The default is TRUE.

      CAUTION:    Changing requires recompile of all sources.

   If DOS is TRUE, TSK_NAMEPAR, TSK_NAMED, and GROUPS must also be TRUE.
*/

#define  TSK_NAMED         TRUE
#define  TSK_NAMEPAR       TRUE
#define  GROUPS            TRUE       /*^

         TSK_NAMED      =  TRUE
         TSK_NAMEPAR    =  TRUE
         GROUPS         =  TRUE 

COMMENT ^


   CLOCK_MSEC     if TRUE, all timeouts are specified in milliseconds.
                  The default is FALSE.

      CAUTION:    Changing requires recompile of all sources.
*/

#define  CLOCK_MSEC        FALSE    /*^

         CLOCK_MSEC     =  FALSE

COMMENT ^

   PRI_TIMER      Priority of the timer task. Normally higher than any
                  other task in the system.
                  The default is 0xf000.

   PRI_STD        standard priority of user tasks (arbitrary), for use
                  in user programs.
                  The default is 100.

   PRI_INT8       priority of the int8 task. Normally lower than or equal to
                  user defined tasks, but may be adjusted to tune responses
                  of TSR's. If there are busy waiting tasks in the system,
                  the priority should be equal to or higher than that of the
                  task that does the busy waiting.
                  The default is PRI_STD.

      CAUTION:    Changing requires recompile of tskinst.c
*/

#define  PRI_TIMER         0xf000
#define  PRI_STD           100
#define  PRI_INT8          (PRI_STD)      /*^

         PRI_TIMER      =  0f000h
         PRI_STD        =  100
         PRI_INT8       =  PRI_STD

COMMENT ^

   AT_BIOS        if TRUE, the AT BIOS wait/post handler is installed.
                  This can normally be left on even for XT type machines.
                  However, some exotic clone BIOS versions have been
                  reported to have problems with INT 15h compatibility, so 
                  for complete portability, this option could be set FALSE.
                  The same effect (although with slightly increased code)
                  can be had by setting the IFL_INT15 flag on installation.
                  The default is TRUE.

      CAUTION:    Changing requires recompile of tskinst.c

   INT8_EARLY     if TRUE, the timer interrupt always uses early INT8
                  processing, regardless of the IFL_INT8_EARLY
                  installation flag.
                  Setting this flag saves some code and data,
                  but eliminates some flexibility. Under DOS, timer
                  ticks can be missed under certain circumstances when
                  set, so DOS based configurations always should use FALSE.
                  The installation flag has a slightly different effect,
                  and may be used under DOS with no adverse effects.
                  Embedded systems likely will always use TRUE.
                  The default is FALSE (use installation flag).

   INT8_LATE      if TRUE, the timer interrupt always uses late INT8
                  processing, regardless of the IFL_INT8_EARLY
                  installation flag.
                  Setting this flag saves some code, but eliminates 
                  some flexibility.
                  Embedded systems likely will always use FALSE.
                  The default is FALSE (use installation flag).

      CAUTION:    Changing requires recompile of 
                  tskmain.c, tskttsk.c, and tsktim.asm.
*/

#define  AT_BIOS           TRUE 
#define  INT8_EARLY        FALSE
#define  INT8_LATE         FALSE    /*^

         AT_BIOS        =  TRUE
         INT8_EARLY     =  FALSE
         INT8_LATE      =  FALSE

COMMENT ^

   SINGLE_DATA    if TRUE, only a single global data block is used.
                  This speeds up processing, but prohibits linkage
                  of multiple groups. Useful for dedicated systems,
                  and also if your program is known to never interact
                  with another copy of CTask. Should be left FALSE if you're
                  not so sure, must be FALSE if GROUPS is enabled.
                  The default is FALSE.

      CAUTION:    Changing requires recompile of all sources.
*/

#define  SINGLE_DATA       FALSE    /*^

         SINGLE_DATA    =  FALSE

COMMENT ^

   EMS            If TRUE, EMS support is included. For DOS applications
                  using spawn or going TSR, this should always be TRUE,
                  unless it is known that the program will never be
                  executed in an EMS environment. It must also be TRUE
                  for programs using EMS.
                  The default is TRUE.

   EMS_SAVE_SIZE  Size of the EMS page info save area.
                  For LIM 4.0 drivers, only four pages are saved, 
                  so 16 bytes should be sufficient. If the partial
                  page map function is not available in the driver,
                  more space may be required for holding the full
                  page map.
                  The default is 16.

      CAUTION:    Changing requires recompile of all sources.
*/

#define  EMS               TRUE
#define  EMS_SAVE_SIZE     16     /*^

         EMS            =  TRUE 
         EMS_SAVE_SIZE  =  16

COMMENT ^


   NDP            If TRUE, 80x87 support is included.
                  The default is FALSE.

      CAUTION:    Changing requires recompile of all sources.

*/

#define  NDP               FALSE    /*^

         NDP            =  FALSE

COMMENT ^


   HOTKEYS        If TRUE, keyboard Hotkey support is included.
                  The default is TRUE.

      CAUTION:    Changing requires recompile of all sources.
*/

#define  HOTKEYS           TRUE     /*^

         HOTKEYS        =  TRUE 

COMMENT ^


   CHECKING       Enables pointer and stack checks when TRUE.
                  The default is FALSE.

      CAUTION:    Changing requires recompile of all sources.
*/

#define  CHECKING          FALSE    /*^

         CHECKING       =  FALSE

COMMENT ^
*/

/* ----------------------------------------------------------------------- */

#if (CODE_SHARING && NEAR_CODE)
#error Bad conf: Cant have near code with code sharing
#endif
#if (NEAR_CODE && LOCALS_FAR)
#error Bad conf: Cant have near code and locals far
#endif
#if (TSK_NAMED && !TSK_NAMEPAR)
#error Bad conf: Cant have names without name parameters
#endif
#if (GROUPS && !TSK_NAMEPAR)
#error Bad conf: Cant have goups without names
#endif
#if (DOS && !GROUPS)
#error Bad conf: DOS requires names and groups
#endif
#if (INT8_EARLY && INT8_LATE)
#error Bad conf: INT8 cant be both early and late
#endif
#if (GROUPS && SINGLE_DATA)
#error Bad conf: Groups require multiple data
#endif

/*^
        IF CODE_SHARING
          IFNDEF LOAD_DS
        %out Bad conf: Must have Load_ds with code sharing
        .err
          ENDIF
        ENDIF
        IF (CODE_SHARING AND NEAR_CODE)
        %out Bad conf: Cant have near code with code sharing
        .err
        ENDIF
        IF (NEAR_CODE AND LOCALS_FAR)
        %out Bad conf: Cant have near code and locals far
        .err
        ENDIF
        IF (TSK_NAMED AND NOT TSK_NAMEPAR)
        %out Bad conf: Cant have names without name parameters
        .err
        ENDIF
        IF (GROUPS AND NOT TSK_NAMEPAR)
        %out Bad conf: Cant have goups without names
        .err
        ENDIF
        IF (DOS AND NOT GROUPS)
        %out Bad conf: DOS requires names and groups
        .err
        ENDIF
        IF (INT8_EARLY AND INT8_LATE)
        %out Bad conf: INT8 cant be both early and late
        .err
        ENDIF
        IF (GROUPS AND SINGLE_DATA)
        %out Bad conf: Groups require multiple data
        .err
        ENDIF

; */

