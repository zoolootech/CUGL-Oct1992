/*	DPATH:	System dependant path definitions for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

/*	possible names and paths of help files under different OSs	*/

char *pathname[] =

#if	AMIGA
{
	".spellrc",
	"dict.dct",
	"",
	"sys:c/",
	"sys:t/",
	"sys:s/",
	":c/",
	":t/",
	":s/"
};
#endif

#if	ST520
{
	"spell.rc",
	"dict.dct",
	"\\",
	"\\bin\\",
	"\\util\\",
	""
};
#endif

#if	FINDER
{
	"spell.rc",
	"dict.dct",
	"/bin",
	"/sys/public",
	""
};
#endif

#if	MSDOS
{
	"spell.rc",
	"dict.dct",
	"\\sys\\public\\",
	"\\usr\\bin\\",
	"\\bin\\",
	"\\",
	""
};
#endif

#if	V7 | BSD | USG
{
	".spellrc",
	"dict.dct",
	"/usr/local/",
	"/usr/lib/",
	""
};
#endif

#if	VMS
{
	"spell.rc",
	"dict.dct",
	"",
	"sys$sysdevice:[vmstools]"
};
#endif

#if     CMS
{
        "profile.spell",
        "dict.dct",
        "CMS:"
};
#endif

#define	NPNAMES	(sizeof(pathname)/sizeof(char *))
