/*
** include doscall.h -- defines for DOS function calls
*/
 
 
/*
** DOS function call codes
*/
 
#define F_KBDECH       1
#define F_DSPOUT       2
#define F_PRNOUT       5
#define F_DIRECH       6
#define F_DIRNOE       7
#define F_KBDNOE       8
#define F_BUFKBD       10
#define F_OPEN         15
#define F_CLOSE        16
#define F_SEARCH       17
#define F_CREATE       22
#define F_SETDTA       26
#define F_READ         39
#define F_WRITE        40
#define F_GDATE        42
#define F_GTIME        44
 
 
/*
** DOS FCB structure
*/
 
#define FCBSIZE        59      /* includes 7-byte FCB extension
                               ** and 15-byte non-DOS postfix
                               */
/* DOS FCB extension begins here */
#define FCB_FLAG       0       /* 000H -- not allocated
                               ** 080H -- KBD
                               ** 040H -- SCRN
                               ** 0C0H -- CON
                               ** 020H -- PRN
                               ** 010H -- ASYNC
                               ** 002H -- READ
                               ** 001H -- WRITE
                               ** 0FFH -- disk FCB (extended)
                               ** others -- device FCB (simulated)
                               */
#define FCB_ATTR       6       /* file creation attributes
                               ** 02H -- hidden
                               ** 04H -- system
                               ** 06H -- both
                               */
/* standard DOS FCB begins here */
#define FCB_DRIVE      7       /* DOS drive number
                               ** 0 - default drive
                               ** 1 - drive A
                               ** 2 - drive B
                               */
#define FCB_FILE       8       /* file name
                               */
#define FCB_EXTEN      16      /* file name extension
                               */
#define FCB_BLOCK      19      /* block number (unused)
                               */
#define FCB_LRECL      21      /* logical record size (always 1)
                               */
#define FCB_FSIZE      23      /* file size in bytes (set by DOS)
                               */
#define FCB_FDATE      27      /* date of last update (set by DOS)
                               */
#define FCB_SRECNO     39      /* record number within block (unused)
                               */
#define FCB_RRECNO     40      /* record number relative to beginning of file
                               ** when LRECL is 1, this value is byte position
                               */
/* non-DOS extension begins here */
#define FCB_MODE       44      /* mode for which file was opened
                               ** 000H - read
                               ** 001H - write
                               ** 002H - read/write
                               ** status
                               ** 080H - end-of-file
                               ** 040H - last input character was CR
                               ** 020H - last input character was LF
                               */
/* UNIX-like sgttyb structure */
#define FCB_SGTTYB     45
#define FCB_ISPEED     45      /* input speed
                               */
#define FCB_OSPEED     46      /* output speed
                               */
#define FCB_ERASE      47      /* backspace character
                               */
#define FCB_KILL       48      /* cancel line character
                               */
#define FCB_FLGS       49      /* flags -- 2 bytes
                               ** 0FF00H - bits to control delays
                               ** 00080H - even parity allowed on input
                               ** 00040H - odd parity allowed on input
                               ** 00020H - "raw" mode - wake up on all chars
                               ** 00010H - map CR into LF, echo LF or CR as both
                               ** 00008H - echo
                               ** 00004H - map upper case to lower case (input)
                               ** 00002H - return each char as typed
                               ** 00001H - automatic flow control
                               */
#define FCB_IBUF       51      /* intermediate buffer
                               */
#define FCB_ISEG       53      /* segment of same (0 == DS)
                               */
#define FCB_IPTR       55      /* next byte in buffer
                               */
#define FCB_ICNT       57      /* # of bytes left
                               */
 
