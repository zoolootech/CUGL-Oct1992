/*
   --- Version 2.2 90-10-12 10:34 ---

   TSK.H - CTask - Type definitions and global routine prototypes.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany
*/


#include "tskconf.h"
#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>

#if defined(__TURBOC__)
#define TSK_TURBO  1
#define TSK_MSC    0
#else
#define TSK_TURBO  0
#define TSK_MSC    1
#endif

/* 
   Selecting near/far and calling sequence attributes:

   For Turbo and Microsoft, you won't have to change the following
   defines. Just set the NEAR_CODE and LOCALS_FAR defines, and the
   CALL_PASCAL define, in tskconf.h.

   Other compilers may or may not support some aspects of the model
   mix used in standard CTask. For those compilers, you may have to
   edit the definitions to suit your memory model.

   Globalfunc  defines a global CTask routine.
               The attributes are normally "far cdecl", but may also
               be "near" and/or "pascal", depending on configuration.

   CGlobalfunc defines a global CTask routine that must be called
               using the C calling sequence.
               The attributes are normally "far cdecl", but may also
               be "near cdecl".

   Localfunc   defines a routine that is exclusively used inside CTask.
               Those routines are normally 'near' to save some code.
               If your compiler does not allow placing all CTask
               routines in a common code segment, you will have to
               define this as 'far'.
               It normally has the "near cdecl" attribute, but may also
               be "far" and/or "pascal".

   CLocalfunc  same as Localfunc, but must use C calling sequence.

   Staticfunc  defines a routine that is local to a module.
               Those routines are only used within a module, and can 
               normally be 'near'. Only if your compiler does not 
               support the 'near' keyword, or places all routines in
               separate segments, will you have to redefine this as 
               empty or 'far'.
               The attributes normally are "near pascal".

   CStaticfunc same as Staticfunc, but must use C calling sequence.

   Taskfunc    defines task and call-back functions.
               Must be 'far cdecl' unless your compiler does not
               support those keywords.

   Neardata    Is the attribute for local shared data in CTask. It is
               defined as 'near' for MSC, and empty for Turbo C.
               This causes the shared data to be placed in the
               default data segment (except for Turbo C Huge, where
               the assignment to the CTask data segment is done with
               compiler switches).
               If your compiler does not support the 'near' attribute
               for global data, you may define this as empty, but please
               make sure that the assembler modules can access the data.

*/

#if (TSK_TURBO)
#define Neardata           /* Turbo doesn't like near on data */
#else
#define Neardata near      /* MSC requires it when in Large model */
#endif

#if (NEAR_CODE)
#define Farc  near        /* near code means all routines are near */
#define Nearc near        /* including internal ones */
#else
#if (CODE_SHARING && TSK_MSC)
#define Farc  far _loadds /* for code sharing, _loadds is required */
#else
#define Farc  far         /* else it's just 'far' */
#endif
#if (LOCALS_FAR)
#define Nearc far         /* Near code is far, too */
#else
#define Nearc near        /* That's the normal case */
#endif
#endif
#if (CALL_PASCAL)
#define Cdeclc pascal
#else
#define Cdeclc cdecl
#endif

#define Globalfunc   Farc Cdeclc
#define Localfunc    Nearc Cdeclc
#define CGlobalfunc  Farc cdecl
#define CLocalfunc   Nearc cdecl
#define Staticfunc   near pascal
#define CStaticfunc  near cdecl
#define Taskfunc     far cdecl

/*
   The local attribute is used mainly for documentation purposes.
   It should be 'static', unless you want the routines to be globally
   visible for debugging (define empty).
*/

#define local  static

/*---------------------------------------------------------------------*/

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef void far *farptr;
typedef byte far *byteptr;
typedef word far *wordptr;
typedef void (Taskfunc *funcptr)();
typedef void (Taskfunc *funcptr_void)(void);
typedef void (Taskfunc *funcptr_int)(int);
typedef void (Taskfunc *funcptr_fp)(farptr);
typedef void (Taskfunc *funcptr_dword)(dword);
typedef void (interrupt far * intprocptr)(void);

#define TTIMEOUT ((farptr) -1L)
#define TWAKE    ((farptr) -2L)
#define TWATCH   ((farptr) -3L)

#define TIMEOUT   (-1)
#define WAKE      (-2)
#define WATCH     (-3)

/* Task states */

#define  ST_KILLED   (byte)0
#define  ST_STOPPED  (byte)1
#define  ST_DELAYED  (byte)2
#define  ST_WAITING  (byte)3
#define  ST_ELIGIBLE (byte)4
#define  ST_RUNNING  (byte)5

/* Task flags */

#define  F_TEMP      (byte)0x80     /* Task is temporary, free on kill */
#define  F_STTEMP    (byte)0x40     /* Task stack is temporary, free on kill */
#define  F_PERM      (byte)0x20     /* Task is permanent, do not kill */

#define  F_USES_NDP  (byte)0x02     /* Task uses the NDP */
#define  F_CRIT      (byte)0x01     /* Task is critical, may not be preempted */

#define  FL_SYSM     (byte)0xf0     /* Mask for system flags */
#define  FL_USRM     (byte)0x0f     /* Mask for user flags */

/* Timer queue element action kinds (upper nibble of elkind) */

#define  TELEM_TIMER       (byte)0x10  /* Timeout element */
#define  TELEM_MEM         (byte)0x20  /* Memory watch element */
#define  TELEM_PORT        (byte)0x30  /* Port watch element */
#define  TELEM_HOTKEY      (byte)0x40  /* Hotkey element */

/* Timer watch element comparison kinds (lower nibble of elkind) */

#define  TCMP_EQ           (byte)1     /* Equal */
#define  TCMP_NE           (byte)2     /* Not Equal */
#define  TCMP_GE           (byte)3     /* Greater or Equal (unsigned) */
#define  TCMP_LE           (byte)4     /* Less or Equal (unsigned) */
#define  TCMP_GES          (byte)5     /* Greater or Equal (signed) */
#define  TCMP_LES          (byte)6     /* Less or Equal (signed) */
#define  TCMP_CHG          (byte)7     /* Change in value */

/* Timer queue element control structure pointer kinds */

#define  TKIND_TASK        (byte)1     /* tcbptr, Wakeup associated task */
#define  TKIND_WAKE        (byte)2     /* tcbptr, but not same task */
#define  TKIND_PROC        (byte)3     /* call function */
#define  TKIND_FLAG        (byte)4     /* flagptr, set flag */
#define  TKIND_COUNTER     (byte)5     /* counterptr, increment counter */
#define  TKIND_COUNTDEC    (byte)6     /* counterptr, decrement counter */

/* Timer queue element flags */

#define  TFLAG_BUSY        (byte)0x01  /* Timer task is busy processing element */
#define  TFLAG_ENQUEUE     (byte)0x02  /* Enqueue after processing */
#define  TFLAG_UNQUEUE     (byte)0x04  /* Don't enqueue after processing */
#define  TFLAG_REMOVE      (byte)0x08  /* Free element after processing */
#define  TFLAG_REPEAT      (byte)0x40  /* Bit set signals repeat processing */
#define  TFLAG_TEMP        (byte)0x80  /* Bit set means temporary element */

/* Name link and queue head structure types */

#define  Q_HEAD         (byte)0x80     /* Queue head flag */

#define  TYP_GROUP      (byte)0
#define  TYP_TCB        (byte)1
#define  TYP_FLAG       (byte)2
#define  TYP_RESOURCE   (byte)3
#define  TYP_COUNTER    (byte)4
#define  TYP_MAILBOX    (byte)5
#define  TYP_PIPE       (byte)6
#define  TYP_WPIPE      (byte)7
#define  TYP_BUFFER     (byte)8
#define  TYP_TIMER      (byte)9
#define  TYP_WATCH     (byte)10
#define  TYP_HOTKEY    (byte)11

#define  NAMELENGTH     9  /* For structure names: 8 bytes + zero */

/* Installation flags */

#define IFL_VIDEO      0x0001      /* Install INT 10 access resource */
#define IFL_DISK       0x0002      /* Install INT 13 access resource */
#define IFL_INT8_DIR   0x0004      /* Call original INT 8 directly */
#define IFL_PRINTER    0x0008      /* Install INT 17 handler */
#define IFL_INT15      0x0010      /* Install IBM-AT INT 15 handler */
#define IFL_NODOSVARS  0x0020      /* Don't swap DOS variables */
#define IFL_NOEXITCHECK 0x0040     /* Don't check for premature exit */

#define IFL_STD   (IFL_DISK | IFL_PRINTER | IFL_INT15)   /* Standard flags */

/* 
   Size of the DOS variable swap area plus 8 bytes.
   40+8 is more than sufficient for all current versions of DOS up to
   DOS 4.01.
*/

#define DOSSWAPSIZE     0x30

/* --------------------------------------------------------------------- */

/*
   The following types define 80x87 data. They are used only when
   NDP is defined TRUE in tskconf.h.
   The type definitions and the basic algorithms for saving the
   coprocessor state were provided by Dan Heine.
*/

/* Temporary real (80-bit) type */

typedef struct {
               word  mant15_0;
               word  mant31_16;
               word  mant47_32;
               word  mant63_48;
               word  s_exponent;
               } t_real;

typedef t_real far *t_realptr;

/* 80x87 state save area */

typedef struct {
               word     control87;
               word     status87;
               word     tag87;
               word     iplo87;
               word     iphi87_opcode87;
               word     opaddrlo87;
               word     opaddrhi87_null;
               t_real   st0;
               t_real   st1;
               t_real   st2;
               t_real   st3;
               t_real   st4;
               t_real   st5;
               t_real   st6;
               t_real   st7;
               } ndpsave_t;

typedef ndpsave_t far *ndpsaveptr;

/* --------------------------------------------------------------------- */

typedef struct callchain_rec far *callchainptr;
typedef void (Taskfunc *funcptr_ccp)(callchainptr);

typedef struct callchain_rec {
                             callchainptr next;    /* Must be first */
                             funcptr_ccp  func;
                             farptr       user_ptr;
#if (TSK_DYNAMIC)
                             byte         flags;
#endif
                             } callchain;

/* --------------------------------------------------------------------- */

/*
   The 'queue' structure is a dual link for linking task control blocks 
   and timer blocks. The first three fields are used both for the queue
   head, and for elements to be inserted in a queue.

   CAUTION: Do not change the order of the first three fields in
            either queue or queue_head!
*/

typedef struct {
                word     prior;
                word     ini_prior;
               } qelem_pri;

typedef struct queue_rec far *queptr;

typedef struct queue_rec {
                          volatile queptr next;
                          volatile queptr prev;
                          byte            kind;
                          union  {
                                 qelem_pri   pri;
                                 dword       ticks;
                                 } el;
                         } queue;


typedef struct {
               volatile queptr first;
               volatile queptr last;
               byte            kind;
               } queue_head;

typedef queue_head far *queheadptr;

typedef struct name_rec far *nameptr;

struct name_rec {
                queue_head list;          /* Name list */
                farptr     strucp;        /* Top of structure pointer */
                char       name [NAMELENGTH];
                };

typedef struct name_rec namerec;

typedef struct tcb_rec far *tcbptr;
typedef tcbptr far *tqueptr;

typedef struct tlink_rec far *tlinkptr;

struct telem_memwatch {
                      wordptr address;
                      word    mask;
                      word    compare;
                      };

struct telem_portwatch {
                        word  port;
                        word  mask;
                        word  compare;
                        byte  in_word;
                       };

typedef struct {
               byte  mask;
               byte  compare;
               } kbflag;

struct telem_hotkey {
                        kbflag   flag1;
                        kbflag   flag2;
                        kbflag   flag3;
                        byte     scancode;
                       };

struct telem_timeout {
                     dword    reload;     /* Timeout counter reload value */
                     };

typedef struct tlink_rec tlink;

struct tlink_rec {
                  queue    link;
                  tlinkptr next;       /* for exclusive use by timer task */
                  farptr   strucp;     /* Pointer to control structure */
                  dword    user_parm;  /* User defined parameter */
#if (GROUPS)
                  queue_head chain;      /* Timer control block chain */
#endif
                  union {
                        struct telem_memwatch   mem;
                        struct telem_portwatch  port;
                        struct telem_timeout    time;
                        struct telem_hotkey     key;
                        }  elem;
                  byte     elkind;     /* Kind of watch element */
                  byte     struckind;  /* Kind of structure pointed to */
                  byte     flags;      /* Flags */
                 };


typedef struct group_rec gcb;
typedef gcb far *gcbptr;

struct tcb_rec {
               queue          cqueue;     /* Current queue link */
               queheadptr     qhead;      /* Queue head pointer */
               byteptr        stkbot;     /* Task stack bottom */
               volatile byte  state;      /* Task state */
               byte           flags;      /* Task flags */

               byteptr        stack;      /* Task register save area */
               word           t_ax;
               word           t_cx;
               word           t_dx;
               word           t_si;
               word           t_di;
               word           t_bp;
               word           t_es;
               word           t_ds;

               tlink           timerq;     /* Timer queue link */
               volatile farptr retptr;     /* Event return pointer */
               volatile int    retsize;    /* Return buffer size for pipes */

               funcptr     save_func;
               funcptr     rest_func;
               farptr      user_ptr;   /* User defined parameter */
#if (GROUPS)
               gcbptr      group;      /* Current Group control block pointer */
               gcbptr      homegroup;  /* Creating Group control block pointer */
#endif
#if (DOS)
               funcptr     sched_ent_func; /* Used by DOS handler */
               volatile byte indos;      /* Flag for use by DOS handler */
               volatile byte t_new;    /* New task flag for DOS save */
               word        base_psp;   /* Base PSP segment */
               dword       psp_sssp;   /* Saved PSP SS/SP value */
               byte        swap_area [DOSSWAPSIZE]; /* DOS vars save area */
#endif
#if (TSK_NAMED)
               namerec     name;
#endif
#if (EMS)
               byte        ems_map [EMS_SAVE_SIZE];
#endif
#if (NDP)
               ndpsave_t   ndpsave;    /* 80x87 save area */
#endif
               };

typedef struct tcb_rec tcb;

struct group_rec {
                 gcbptr    home;       /* Home group */
                 gcbptr    level;      /* Next group on level */
                 gcbptr    branch;     /* Next group on higher level */
                 tcbptr    creator;    /* Task that created this group */
                 dword     exit_addr;  /* Exit address */
                 word      create_psp; /* Base PSP addr of this group */
                 word      save_psp;   /* PSP addr of this group's creator */
                 dword     save_sssp;  /* SS/SP from base PSP of creator */
                 namerec   namelist;   /* List of structures in this group */
                 tcbptr    main_ptr;   /* Main task for this group */
                 callchainptr remove;   /* Int handler remove chain */
                 queue_head  telem_list;  /* Timer element chain */
                 queue_head  ticker_list; /* Timer element chain */
#if (TSK_DYNAMIC)
                 farptr (Globalfunc *palloc)(word); /* alloc function ptr */
                 farptr (Globalfunc *pfree)(farptr);  /* free function ptr */
#endif
                 };


typedef struct {
               queue_head   wait_set;
               queue_head   wait_clear;
               volatile int state;
#if (TSK_DYNAMIC)
               byte         flags;
#endif
#if (TSK_NAMED)
               namerec      name;
#endif
               } flag;

typedef flag far *flagptr;

typedef struct {
               queue_head     wait_set;
               queue_head     wait_clear;
               volatile dword state;
#if (TSK_DYNAMIC)
               byte           flags;
#endif
#if (TSK_NAMED)
               namerec        name;
#endif
               } counter;

typedef counter far *counterptr;

typedef struct {
               queue_head        waiting;
               volatile tcbptr   owner;
               volatile word     count;
#if (TSK_DYNAMIC)
               byte              flags;
#endif
#if (TSK_NAMED)
               namerec           name;
#endif
               } resource;

typedef resource far *resourceptr;

typedef struct msg_header far *msgptr;

struct msg_header {
                  volatile msgptr next;
                  };

typedef struct {
               queue_head  waiting;
               msgptr      mail_first;
               msgptr      mail_last;
#if (TSK_DYNAMIC)
               byte        flags;
#endif
#if (TSK_NAMED)
               namerec     name;
#endif
               } mailbox;

typedef mailbox far *mailboxptr;

typedef struct {
               queue_head  wait_read;
               queue_head  wait_write;
               queue_head  wait_clear;
               word        bufsize;
               volatile word filled;
               volatile word inptr;
               volatile word outptr;
               byteptr     contents;
#if (TSK_DYNAMIC)
               byte        flags;
#endif
#if (TSK_NAMED)
               namerec     name;
#endif
               } pipe;

typedef pipe far *pipeptr;

typedef struct {
               queue_head  wait_read;
               queue_head  wait_write;
               queue_head  wait_clear;
               word        bufsize;
               volatile word filled;
               volatile word inptr;
               volatile word outptr;
               wordptr     wcontents;
#if (TSK_DYNAMIC)
               byte        flags;
#endif
#if (TSK_NAMED)
               namerec     name;
#endif
               } wpipe;

typedef wpipe far *wpipeptr;

typedef struct {
               resource    buf_write;
               resource    buf_read;
               wpipe       pip;
               volatile word msgcnt;
#if (TSK_DYNAMIC)
               byte    flags;
#endif
#if (TSK_NAMED)
               namerec     name;
#endif
               } buffer;

typedef buffer far *bufferptr;


typedef struct ticker_rec far *tick_ptr;

typedef struct ticker_rec {
                          volatile tick_ptr next;
                          volatile dword ticks;
#if (GROUPS)
                          queue_head chain;
#endif
#if (TSK_DYNAMIC)
                          byte     flags;
#endif
                          } ticker;

/* --------------------------------------------------------------------- */

#if TSK_NAMEPAR
#define TN(name_def) ,name_def
#else
#define TN(name_def) 
#endif

#define CRITICAL  int crit_intsav
#define C_ENTER   crit_intsav = tsk_dis_int()
#define C_LEAVE   tsk_ena_int (crit_intsav)


/* module tskmain */

extern int ctask_active;

extern int Globalfunc install_tasker (byte varpri, int speedup, word flags
                                      TN(byteptr name));
extern void Globalfunc remove_tasker (void);


/* module tskutil */

extern void Globalfunc preempt_off (void);
extern void Globalfunc preempt_on (void);
extern void Globalfunc tsk_ena_preempt (void);
extern void Globalfunc tsk_dis_preempt (void);
extern int Globalfunc t_delay (dword ticks);
extern callchainptr Globalfunc chain_removefunc (funcptr_ccp func, callchainptr chain, farptr user_ptr);
extern void Globalfunc unchain_removefunc (callchainptr chain);
extern word Globalfunc ticks_per_sec (void);


/* module tskasm */

extern void Globalfunc schedule (void);
extern void Globalfunc c_schedule (void);
extern void Globalfunc yield (void);

extern int Globalfunc  tsk_dis_int (void);
extern void Globalfunc tsk_ena_int (int);
extern void Globalfunc tsk_nop (void);

#if (TSK_TURBO)
/* The following are missing from the TC++ 1.0 DOS.H file */
unsigned char _Cdecl __inportb__(int __portid);
void	_Cdecl	__outportb__	(int __portid, unsigned char __value);
#define inportb __inportb__
#define outportb __outportb__

#define tsk_cli()       disable()
#define tsk_sti()       enable()
#define tsk_outp(p,b)   outportb(p,b)
#define tsk_inp(p)      ((byte) inportb(p))
extern word Globalfunc tsk_inpw (word port);
#elif TSK_MSC
#pragma intrinsic(_disable,_enable,outp,inp,inpw)
#define tsk_cli()       _disable()
#define tsk_sti()       _enable()
#define tsk_outp(p,b)   outp(p,b)
#define tsk_inp(p)      ((byte) inp(p))
#define tsk_inpw(p)     ((byte) inpw(p))

#define MK_FP(seg,ofs)  ((farptr)(((dword)(seg) << 16) | (word)(ofs)))
#else
extern int Globalfunc  tsk_dis_int (void);
extern void Globalfunc tsk_ena_int (int);
extern void Globalfunc tsk_cli (void);
extern void Globalfunc tsk_sti (void);
#endif

#define TMK_FP(seg,ofs) ((farptr)(((dword)(seg) << 16) | (word)(ofs)))
#define TFP_SEG(ptr)    ((word)((dword)ptr >> 16))
#define TFP_OFF(ptr)    ((word)((dword)ptr))
#define TSK_OFFSETOF(struc,elem)  ((word)(&((struc near *)0)->elem))
#define TSK_STRUCTOP(struc,ptr,elem)  ((struc far *)((byteptr)ptr - TSK_OFFSETOF(struc, elem)))
#define LNULL           ((void far *)0L)

/* module tsktask */

extern int tsk_use_ndp;

extern tcbptr Globalfunc create_task (tcbptr task, funcptr func, byteptr stack,
                                      word stksz, word prior, farptr arg
                                      TN(byteptr name));
extern void Globalfunc kill_task (tcbptr task);
extern int Globalfunc start_task (tcbptr task);


/* module tsktutl */

extern int Globalfunc wake_task (tcbptr task);
extern int Globalfunc stop_task (tcbptr task);
extern word Globalfunc get_priority (tcbptr task);
extern void Globalfunc set_priority (tcbptr task, word prior);
extern void Globalfunc set_task_flags (tcbptr task, byte flags);
extern void Globalfunc set_funcs (tcbptr task, funcptr save, funcptr rest);
extern farptr Globalfunc set_user_ptr (tcbptr task, farptr uptr);
extern farptr Globalfunc get_user_ptr (tcbptr task);
extern tcbptr Globalfunc curr_task (void);


/* module tsktops */

extern tlinkptr CGlobalfunc create_timer_elem (tlinkptr elem, dword tout, 
                                               farptr strucp, byte kind, 
                                               int rept, ...);

extern tlinkptr CGlobalfunc create_timer (tlinkptr elem, dword tout, farptr strucp,
                                          byte kind, int rept, ...);
extern void Globalfunc enable_timer (tlinkptr elem);
extern void Globalfunc disable_timer (tlinkptr elem);
extern void Globalfunc delete_timer (tlinkptr elem);
extern void CGlobalfunc change_timer (tlinkptr elem, dword tout, int rept, ...);


/* module tsktsub */

extern void Globalfunc enable_watch (tlinkptr elem);

#if (CHECKING)
extern void Globalfunc disable_hwatch (tlinkptr elem, byte typ);
extern void Globalfunc delete_hwatch (tlinkptr elem, byte typ);

#define disable_watch(elem)   disable_hwatch(elem,TYP_WATCH);
#define delete_watch(elem)    delete_hwatch(elem,TYP_WATCH);

#define disable_hotkey(elem)  disable_hwatch(elem,TYP_HOTKEY);
#define delete_hotkey(elem)   delete_hwatch(elem,TYP_HOTKEY);

#else
extern void Globalfunc disable_watch (tlinkptr elem);
extern void Globalfunc delete_watch (tlinkptr elem);

#define disable_hotkey(elem)  disable_watch(elem);
#define delete_hotkey(elem)   delete_watch(elem);
#endif

/* module tskmemw */

extern tlinkptr CGlobalfunc create_memory_watch_elem (tlinkptr elem, farptr address, 
                                                 word mask, word compare, 
                                                 byte cmpkind, farptr strucp, 
                                                 byte kind, int rept, ...);
extern tlinkptr CGlobalfunc create_memory_watch (tlinkptr elem, farptr address, 
                                                 word mask, word compare, 
                                                 byte cmpkind, farptr strucp, 
                                                 byte kind, int rept, ...);
extern int Globalfunc wait_memory (farptr address, 
                                   word mask, word compare, byte cmpkind);


/* module tskporw */

extern tlinkptr CGlobalfunc create_port_watch_elem (tlinkptr elem, 
                                               word port, byte in_word,
                                               word mask, word compare, 
                                               byte cmpkind, farptr strucp, 
                                               byte kind, int rept, ...);

extern tlinkptr CGlobalfunc create_port_watch (tlinkptr elem, 
                                               word port, byte in_word,
                                               word mask, word compare, 
                                               byte cmpkind, farptr strucp, 
                                               byte kind, int rept, ...);

extern int Globalfunc wait_port (word port, byte in_word,
                                 word mask, word compare, byte cmpkind);


/* module tskhot */

#if (HOTKEYS)
extern tlinkptr CGlobalfunc create_hotkey_elem (tlinkptr elem, byte scancode,
                                                byte mask1, byte flag1,
                                                byte mask2, byte flag2,
                                                byte mask3, byte flag3,
                                                farptr strucp, byte kind, 
                                                int rept, ...);
extern tlinkptr CGlobalfunc create_hotkey_entry (tlinkptr elem, byte scancode,
                                                 byte mask1, byte flag1,
                                                 byte mask2, byte flag2,
                                                 byte mask3, byte flag3,
                                                 farptr strucp, byte kind, 
                                                 int rept, ...);

extern int Globalfunc wait_hotkey (byte scancode,
                                   byte mask1, byte flag1,
                                   byte mask2, byte flag2,
                                   byte mask3, byte flag3);

extern void Globalfunc enable_hotkey (tlinkptr elem);

#endif


/* module tskrsc */

extern resourceptr Globalfunc create_resource (resourceptr rsc TN(byteptr name));
extern void Globalfunc delete_resource (resourceptr rsc);
extern int Globalfunc request_resource (resourceptr rsc, dword timeout);
extern int Globalfunc c_request_resource (resourceptr rsc);
extern int Globalfunc request_cresource (resourceptr rsc, dword timeout);
extern int Globalfunc c_request_cresource (resourceptr rsc);
extern void Globalfunc release_resource (resourceptr rsc);
extern int Globalfunc check_resource (resourceptr rsc);


/* module tskflg */

extern flagptr Globalfunc create_flag (flagptr flg TN(byteptr name));
extern void Globalfunc delete_flag (flagptr flg);
extern int Globalfunc wait_flag_set (flagptr flg, dword timeout);
extern int Globalfunc wait_flag_clear (flagptr flg, dword timeout);
extern void Globalfunc set_flag (flagptr flg);
extern void Globalfunc clear_flag (flagptr flg);
extern int Globalfunc clear_flag_wait_set (flagptr flg, dword timeout);
extern int Globalfunc check_flag (flagptr flg);


/* module tskcnt */

extern counterptr Globalfunc create_counter (counterptr cnt TN(byteptr name));
extern void Globalfunc delete_counter (counterptr cnt);
extern void Globalfunc clear_counter (counterptr cnt);
extern int Globalfunc wait_counter_set (counterptr cnt, dword timeout);
extern int Globalfunc wait_counter_clear (counterptr cnt, dword timeout);
extern dword Globalfunc inc_counter (counterptr cnt);
extern dword Globalfunc dec_counter (counterptr cnt);
extern dword Globalfunc check_counter (counterptr cnt);
extern void Globalfunc set_counter (counterptr cnt, dword val);


/* module tskmsg */

extern mailboxptr Globalfunc create_mailbox (mailboxptr box TN(byteptr name));
extern void Globalfunc delete_mailbox (mailboxptr box);
extern farptr Globalfunc wait_mail (mailboxptr box, dword timeout);
extern farptr Globalfunc c_wait_mail (mailboxptr box);
extern void Globalfunc send_mail (mailboxptr box, farptr msg);
extern int Globalfunc check_mailbox (mailboxptr box);


/* module tskpip */

extern pipeptr Globalfunc create_pipe (pipeptr pip, farptr buf, word bufsize
                                         TN(byteptr name));
extern void Globalfunc delete_pipe (pipeptr pip);
extern int Globalfunc read_pipe (pipeptr pip, dword timeout);
extern int Globalfunc c_read_pipe (pipeptr pip);
extern int Globalfunc write_pipe (pipeptr pip, byte ch, dword timeout);
extern int Globalfunc c_write_pipe (pipeptr pip, byte ch);
extern int Globalfunc wait_pipe_empty (pipeptr pip, dword timeout);
extern int Globalfunc check_pipe (pipeptr pip);
extern word Globalfunc pipe_free (pipeptr pip);
extern void Globalfunc flush_pipe (pipeptr pip);


/* module tskwpip */

extern wpipeptr Globalfunc create_wpipe (wpipeptr pip, farptr buf, word bufsize
                                           TN(byteptr name));
extern void Globalfunc delete_wpipe (wpipeptr pip);
extern word Globalfunc read_wpipe (wpipeptr pip, dword timeout);
extern word Globalfunc c_read_wpipe (wpipeptr pip);
extern int Globalfunc write_wpipe (wpipeptr pip, word ch, dword timeout);
extern int Globalfunc c_write_wpipe (wpipeptr pip, word ch);
extern int Globalfunc wait_wpipe_empty (wpipeptr pip, dword timeout);
extern word Globalfunc check_wpipe (wpipeptr pip);
extern word Globalfunc wpipe_free (wpipeptr pip);
extern void Globalfunc flush_wpipe (wpipeptr pip);


/* module tskbuf */

extern bufferptr Globalfunc create_buffer (bufferptr buf, farptr pbuf, 
                                           word bufsize TN(byteptr name));
extern void Globalfunc delete_buffer (bufferptr buf);
extern int Globalfunc read_buffer (bufferptr buf, farptr msg, int size, dword timeout);
extern int Globalfunc c_read_buffer (bufferptr buf, farptr msg, int size);
extern int Globalfunc write_buffer (bufferptr buf, farptr msg, int size, dword timeout);
extern int Globalfunc c_write_buffer (bufferptr buf, farptr msg, int size);
extern word Globalfunc check_buffer (bufferptr buf);


/* module tsksec */

extern word Globalfunc t_read_key (void);
extern word Globalfunc t_wait_key (dword timeout);
extern word Globalfunc t_keyhit (void);


/* module tskalloc (model dependent) */

#if (TSK_DYNAMIC && TSK_DYNLOAD)
extern farptr Globalfunc tsk_alloc (word size);
extern farptr Globalfunc tsk_calloc (word item, word size);
extern farptr Globalfunc tsk_free (farptr item);
extern farptr Globalfunc tsk_realloc (farptr item, word size);

extern resource Neardata alloc_resource;
#endif


/* module tskname */

#if (TSK_NAMED)
extern farptr Globalfunc find_group_name (gcbptr group, byteptr name, int kind);
extern farptr Globalfunc find_name (byteptr name, int kind);
extern void Globalfunc add_event_name (nameptr elem);
#else
#define add_event_name(x)
#endif


/* module tsktick */

extern tick_ptr Globalfunc create_ticker (tick_ptr elem, dword val);
extern void Globalfunc delete_ticker (tick_ptr elem);
extern void Globalfunc set_ticker (tick_ptr elem, dword val);
extern dword Globalfunc get_ticker (tick_ptr elem);


/* module tskres */

extern int Globalfunc ctask_resident (void);
extern int Globalfunc link_ctask (void);

/* --------------------------------------------------------------------- */

/* 
   Macros to define and initialize static control structures.
   General usage is:
         DEF_xxx (variable, name [other params]);
   Example:
         DEF_FLAG (my_flag, "MyFlag");
*/

#define TQH(var,typ)  {(queptr)&var, (queptr)&var, Q_HEAD | typ}
#if (TSK_NAMED)
#define TNR(var,nam,typ) ,{{LNULL, LNULL, typ}, &var, nam}
#else
#define TNR(var,nam,typ)
#endif
#if (TSK_DYNAMIC)
#define TFLG ,0
#else
#define TFLG
#endif

#define DEF_FLAG(var,nam) flag var = { TQH(var.wait_set,TYP_FLAG), TQH(var.wait_clear,TYP_FLAG), 0 TFLG TNR(var,nam,TYP_FLAG) }
#define DEF_COUNTER(var,nam) counter var = { TQH(var.wait_set,TYP_COUNTER), TQH(var.wait_clear,TYP_COUNTER), 0L TFLG TNR(var,nam,TYP_COUNTER) }
#define DEF_RESOURCE(var,nam) resource var = { TQH(var.waiting,TYP_RESOURCE), LNULL, 0 TFLG TNR(var,nam,TYP_RESOURCE) }
#define DEF_MAILBOX(var,nam) mailbox var = { TQH(var.waiting,TYP_RESOURCE), LNULL, LNULL TFLG TNR(var,nam,TYP_MAILBOX) }
#define DEF_PIPE(var,nam,buf,len) pipe var = { TQH(var.wait_read,TYP_PIPE), TQH(var.wait_write,TYP_PIPE), TQH(var.wait_clear,TYP_PIPE), \
                                               len, 0, 0, 0, (byteptr)buf TFLG TNR(var,nam,TYP_PIPE) }
#define DEF_WPIPE(var,nam,buf,len) wpipe var = { TQH(var.wait_read,TYP_WPIPE), TQH(var.wait_write,TYP_WPIPE), TQH(var.wait_clear,TYP_WPIPE), \
                                                 len / 2, 0, 0, 0, (wordptr)buf TFLG TNR(var,nam,TYP_WPIPE) }

