/*-------------------------------------------------------------------*/
/*	LIBRARY FILE:     ERRLIST.C
	------------
	WRITTEN:          07/09/87
	-------
	PURPOSE:          Array of pointers to error messages.   Should
	-------           be used with PRINTERR
	USAGE:
	-----
	DEPENDENCIES:     De Smet C V 2.44+
	------------
	Copyright 1987 - Cogar Computer Services Pty. Ltd                */
/*-------------------------------------------------------------------*/
char undefmsg[] = "Undefined error.";

char *errlist[] =
{
	"",                                /* [0]                        */
	"Invalid function number",         /* [1]                        */
	"File not found",                  /* [2]                        */
	"Invalid path",                    /* [3]                        */
	"Too many files open",             /* [4]                        */
	"Access denied",                   /* [5]                        */
	"Invalid file handle",             /* [6]                        */
	"Memory control blocks destroyed", /* [7]                        */
	"Memory exhausted",                /* [8]                        */
	"Invalid memory block address",    /* [9]                        */
	"Invalid environment",             /* [10]                       */
	"Invalid format",                  /* [11]                       */
	"Invalid access code",             /* [12]                       */
	"Invalid data",                    /* [13]                       */
	undefmsg,                          /* [14]                       */
	"Invalid drive was specified",     /* [15]                       */
	"Attempt to remove current directory",  /* [16]                  */
	"Not same device",                 /* [17]                       */
	"No more files",                   /* [18]                       */
	"Disk write-protected",            /* [19]                       */
	"Unknown unit",                    /* [20]                       */
	"Drive not ready",                 /* [21]                       */
	"Unknown command",                 /* [22]                       */
	"Data error (CRC)",                /* [23]                       */
	"Structure length error",          /* [24]                       */
	"Seek error",                      /* [25]                       */
	"Unknown media type",              /* [26]                       */
	"Sector not found",                /* [27]                       */
	"Printer out of paper",            /* [28]                       */
	"Write error",                     /* [29]                       */
	"Read error",                      /* [30]                       */
	"General failure",                 /* [31]                       */
	"Sharing violation",               /* [32]                       */
	"Lock violation",                  /* [33]                       */
	"Invalid disk change",             /* [34]                       */
	"FCB not available",               /* [35]                       */
	undefmsg,                          /* [36] to [79]               */
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,                          /* [40]                       */
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,                          /* [50]                       */
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,                          /* [60]                       */
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,                          /* [70]                       */
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,
	undefmsg,                          /* [79]                       */
	"File exists",                     /* [80]                       */
	undefmsg,                          /* [81]                       */
	"Cannot make",                     /* [82]                       */
	"INT 24 failure",                  /* [83]                       */
	"Unable to copy file to itself",   /* [84]                       */
	"Invalid option",                  /* [85]                       */
	"Subdirectory already exists",     /* [86]                       */
	"Not enough arguments",            /* [87]                       */
	"File is R/O",                     /* [88]                       */
	"Invalid option prefix",           /* [89]                       */
	"Bad drive...path...or filename",  /* [90]                       */
	"Insufficient memory installed",   /* [91]                       */
	"Write error...disk may be full",  /* [92]                       */
	"Insufficient disk space",         /* [93]                       */
	"Unknown error",                   /* [94]                       */
	"Path name too long",              /* [95]                       */
	"Invalid date",                    /* [96]                       */
	"Invalid time",                    /* [97]                       */
	"Unable to open for reading",      /* [98]                       */
	"Unable to open for writing",      /* [99]                       */
	"Unable to open for up-date",      /* [100]                      */
	"",                                /* [101] Sound bell           */
	"Serial port not available",       /* [102]                      */
	"Uninitialised data structure",    /* [103]                      */
	"Interrupts not supported"         /* [104]                      */
};
unsigned errtablesize = sizeof(errlist)/sizeof(char**);
/*-------------------------------------------------------------------*/
