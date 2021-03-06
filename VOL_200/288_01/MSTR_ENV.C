/*========================================================================
    Module:   Mstr_Env (Master Environment Access)
    Version:  1.00     11-Mar-1989

    Written by Scott Robert Ladd.

    This software has been released into the public domain;
    no warranties are expressed or implied.

    This source code will work with most MS-DOS C compilers. It has been
    tested with Microsoft C 5.1 and QuickC 2.0, Borland Turbo C 2.0, and
    Zortech C v1.07.
=========================================================================*/

#include "dos.h"      /* _psp definition */
#include "stddef.h"   /* NULL and other standard definitions */
#include "string.h"   /* string functions */
#include "mstr_env.h" /* to get prototypes */

/* define a macro for MSC and QC */
#if defined(_QC) || defined (_MSC)
     #define MK_FP(seg,off) ((void far *)(((long)(seg) << 16L) | (long)(off)))
#endif

/*
internal MS-DOS structures
--------------------------
*/

typedef struct
    {
    char reserved1[22];
    unsigned par_seg;   /* segment of the parent of the current program */
    char reserved2[20];
    unsigned env_seg;   /* environment segment for current program */
    }
    PSP;

  /*-----------------
     IMPORTANT NOTE!
    -----------------------------------------------------------------------
     The MCB structure must be compiled with byte, not word, alignment. In
     Microsoft C, this is accomplished through the use of the pack pragma.
     By default, Turbo C uses byte alignment. When compiling this module
     with Zortech C, the -a switch must be used to get byte alignment.
    ----------------------------------------------------------------------- */

#if defined(_QC) || defined(_MSC)
    #pragma pack(1)
#endif

typedef struct
    {
    char status;         /* indicates whether this block is in chain */
    unsigned owner_PSP;  /* the PSP segment of this block's owner */
    unsigned len;        /* the size (in paragraphs) of this memory block */
    }
    MCB;

#if defined(_QC) || defined(_MSC)
    #pragma pack()
#endif

/*
global declarations
-------------------
*/

char far * env_ptr; /* pointer to first byte of global env. */
static unsigned   env_len; /* length of global environment */

static int initialized = 0; /* indicates whether the module is initialized */

static char s[256];         /* internal buffer used for strings */

static void m_findenv(void); /* internal function used to find the master
                                environment */

#define NUL '\x00' /* the nul (zero) character used by C as a string
                      terminator */

/*
function definitions
--------------------
*/

/*
  function:     m_getenv

  purpose:      retrieves an environment variable's value

  parameters:   name -- the name of the variable whose value is sought

  returns:      A pointer to the value of the requested variable, or NULL if the
                variable was not found.

                NOTE: the return value is a pointer to a static char array in
                this module. Thus the previous value is destroyed each time this
                function is called.
*/

char * m_getenv(char * name)
    {
    char far * e;
    char *     n;
    int        i;

    /* check for initialization */
    if (!initialized)
         m_findenv();

    e = env_ptr;

    /* search for name */
    while (*e)
        {
        n = name;

        while ((*e == *n) && (*e != '=') && (*e != NUL) && (*n != NUL))
            {
            ++e;
            ++n;
            }

        if ((*n == NUL) && (*e == '=')) /* name found */
             {
             /* copy variable value to s */
             ++e;

             /* strcpy() cannot be used because pointer sizes may differ */
             for (i = 0; (i < 256) && (*e != NUL); ++i)
                  {
                  s[i] = *e;
                  ++e;
                  }

             if (i < 256)
                 s[i] = NUL;

             return s; /* contains value of name */
             }

        /* skip to the next environment variable */
        while (*e != NUL)
            ++e;

        ++e;
        }

    /* name wasn't found */
    return NULL;
    }

/*
    function:   m_putenv

    purpose:    stores an environment variable

    parameters: name -- name of the variable to be stored
                text -- the variable's value

    returns:    0 if the variable was stored
                1 if not
*/

int m_putenv(char *name, char *text)
    {
    char far * e;
    unsigned   l = 0;
    char *     sptr;

    /* check for initialization */
    if (!initialized)
         m_findenv();

    e = env_ptr;

    /* check to see that the storage required is < 256 bytes */
    if ((strlen(name) + 2 + strlen(text)) > 256)
        return 1;

    /* make a complete environment string from the components given */
    strcpy(s,name);
    strupr(s);
    strcat(s,"=");
    strcat(s,text);

    /* delete any existing variables of the same name */
    m_delenv(name);

    /* find the end of the current variables (mark by two nulls) */
    e = env_ptr;
    l = 0;

    /* loop until two NULs are found, signifying the end of the environment */
    while (!((*e == NUL) && (*(e+1) == NUL)))
        {
        ++e;
        ++l;
        }

    /* get the amount of remaining space */
    l = env_len - l - 1;

    /* if the new variable won't fit, return an error */
    if (l < strlen(s))
         return 1;

    sptr = s;

    /* otherwise, copy s onto the end of the current environment */
    ++e;

    while (*sptr != NUL)
        *e++ = *sptr++;

    /* end the environment with two NUL bytes */
    *e = NUL;
    *(e + 1) = NUL;

    /* it worked! */
    return 0;
    }

/*
    function:   m_delgenv

    purpose:    deletes an environment variable

    parameters: name -- the name of the variable to be deleted

    returns:    1 if the variable could not be deleted or does not exit
                0 upon success
*/
int m_delenv(char *name)
     {
    char far * e1; /* used in search & marks beginning of next variable */
    char far * e2; /* marks beginning of the variable */
    char * n;      /* name pointer used in search */
    int searching = 1; /* flag to indicate search end */

    /* has the module been initialized? If not, do it! */
    if (!initialized)
        m_findenv();

    e1 = env_ptr;

    /* find the beginning of the variable to be deleted */
    while ((*e1 != NUL) && (searching))
        {
        n  = name;
        e2 = e1;

        while ((*e1 == *n) && (*e1 != '=') && (*e1 != NUL) && (*n != NUL))
            {
            ++e1;
            ++n;
            }

        if ((!*n) && (*e1 == '='))
            searching = 0; /* the variable we want was found! */

        while (*e1 != NUL)
            ++e1;

        ++e1;
        }

    /* if name wasn't found, return with an error */
    if ((*e1 == NUL) && (searching))
         return 1;

    /* otherwise, copy the remainder of the environment over name */
    while (!((*e1 == NUL) && (*(e1+1) == NUL)))
        {
        *e2 = *e1;
        e2++;
        e1++;
        }

    /* end the environment with double NUL bytes */
    *e2 = NUL;
    *(e2 + 1) = NUL;

    /* it worked */
    return 0;
    }

/*
    function:   m_findenv

    purpose:    locates the global environment and sets a pointer to it

    parameters: none

    returns:    nothing
*/

static void m_findenv()
    {
    union  REGS  regs;
    struct SREGS sregs;
    int far * SEGptr;    /* set to address of segment of first MCB */
    MCB far * CONFIGmcb; /* set to address of CONFIG's MCB */
    MCB far * SHELLmcb;  /* set to address of COMMAND.COM's MCB */
    MCB far * ENVmcb;    /* set to address of environment's MCB */
    PSP far * SHELLpsp;  /* set to address of COMMAND.COM's PSP */

    regs.h.ah = 0x52;
    intdosx(&regs,&regs,&sregs);

    SEGptr = MK_FP(sregs.es, regs.x.bx - 2);

    CONFIGmcb = MK_FP(*SEGptr,0);

    SHELLpsp = MK_FP(FP_SEG(CONFIGmcb) + CONFIGmcb->len + 2, 0);

    if (SHELLpsp->env_seg == 0)
        {
        /* the environment is in the block AFTER the parent program */
        SHELLmcb = MK_FP(FP_SEG(CONFIGmcb) + CONFIGmcb->len + 1, 0);
        env_ptr  = MK_FP(FP_SEG(SHELLmcb)  + SHELLmcb->len  + 2, 0);
        }
    else
        {
        /* otherwise, we have a direct pointer to the environment block */
        env_ptr = MK_FP(SHELLpsp->env_seg, 0);
        }

    /* set a ppointer to the MCB of the environment block */
    ENVmcb = MK_FP(FP_SEG(env_ptr) - 1, 0);

    /* calculate the length of the environment */
    env_len = ENVmcb->len * 16;

    /* set the initialization flag */
    initialized = 1;
    }
