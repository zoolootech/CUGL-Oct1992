/*  HEADER:     CUGaaa.bb;
    FILENAME:   local.h;
    TITLE:      Local Header;
    COMPILERS:  C86, Lattice C, unix/3 C, unix/5 C;
    SYSTEM:     ms-dos, unix/3, unix/5;
    AUTHORS:    Bill Rogers;
    VERSION:    3.01;
    DATE:       05/09/1985;
    KEYWORDS:   stdio.h, ms-dos, unix, header, uniform;
    SEE-ALSO:   cmodem;
    DESCRIPTION:
       "This file plus 'stdio.h' should result in the same interface
        independent of the computer, operating system or C compiler!
        Note the type names that can be redefined for different
        environments (courtesy Computer Innovations)."
    REFERENCES: C Language Bulletin;
 */
/*--------------------------------------------------------------------*/
/*  INCLUDE */
#include <stdio.h>
/*--------------------------------------------------------------------*/

/*  Environment */

#define L8086                   /*  3B2, PLEXUS, XT or L8086 */ 
#define MS_DOS                  /*  MS_DOS, PC_DOS or UNIX */ 
#define C86                     /*  C86 or UNIX_C */ 
#define HZ1500                  /*  VT100, TV950, HZ1500 or ANSI */

/*  Switch character on command line. */

#ifdef  PC_DOS
#define switchar        '\/'
#ifdef  MS_DOS
#define switchar        '\/'  
#endif
#ifdef  UNIX
#define switchar        '-'     
#endif

/*  Lower Case Versions of Upper Case Definitions Given in stdio.h */

#ifdef  C86
#ifdef  _C86_BIG
#define null             (0l)
#else
#define null             0
#endif
#define file             FILE
#endif

#ifdef  UNIX_C
#define null             0
#define file             struct _iobuf
#endif

#define eof              (-1)
#define eos              '\0'
#define error            0                

#ifdef  C86
#define aread            0
#define awrite           1
#define aupdate          2
#define bread            4
#define bwrite           5
#define bupdate          6
#endif

/*  Types */

#define bool            int       /*  boolean */
#define enum            int       /*  enumeration */
#define metachar        short int /*  return char or flag */
#define string          char      /*  character string */
#define void            int       /*  return void (procedure) */

#define false           0                
#define true            1

#ifdef  C86
#define SCHAR           char
#define UCHAR           unsigned char
#define CHAR            char
#define SSHORT          short int
#define USHORT          unsigned short int
#define SINT            int
#define UINT            unsigned int
#define SLONG           long int
#define ULONG           unsigned long int
#define FLOAT           float
#define DOUBLE          double
#endif

#ifdef  UNIX_C
#define SCHAR           char
#define UCHAR           unsigned char
#define SSHORT          short int
#define USHORT          unsigned short int
#define SLONG           long int
#define ULONG           unsigned long int
#define SINT            int
#define UINT            unsigned int
#endif

/* Open Stream File Modes */

#define openread         "r"
#define openupd          "r+"
#define creawrit         "w"
#define creaupd          "w+"
#define apndwrit         "a"
#define apndupd          "a+"

#ifdef  C86
#define opnreadb         "rb"
#define opnupdb          "rb+"
#define crewritb         "wb"
#define creupdb          "wb+"
#define apnwritb         "ab"
#define apnupdb          "ab+"
#define console          "CON:"
#define printer          "PRN:"
#endif

#ifdef  UNIX_C
#define opnreadb         "r"
#define opnupdb          "r+"
#define crewritb         "w"
#define creupdb          "w+"
#define apnwritb         "a"
#define apnupdb          "a+"
#endif

/*  Preprocessor 'Functions' */

#define abs(x)           ((x) >= (0) ? (x) : (-x))
#define max(x,y)         ((x) >= (y) ? (x) : ( y))
#define min(x,y)         ((x) <= (y) ? (x) : ( y))

/*  External Definitions */

#ifdef C86
extern  file             *fopen();
extern  file             *freopen();
extern  long int         ftell();
extern  char             *fgets();
extern  char             *gets();
#endif
/*--------------------------------------------------------------------*/
/*  end local.h */
/*--------------------------------------------------------------------*/
