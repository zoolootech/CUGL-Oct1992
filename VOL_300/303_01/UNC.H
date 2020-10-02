/*
 *	SCCS:	%W%	%G%	%U%
 *	Header file for uncompile program.
 *
 *EMACS_MODES:c
 */

#define	MAXCHARS	50
#define	HASHMOD		97

/*
 *	The following structure is used to keep track of symbols.
 */

struct	symstr	{
	struct	symstr	*s_next;		/*  Next in hash chain  */
	struct	symstr	*s_link;		/*  Next in duplicate labels */
#ifdef	COFF
	unsigned	s_type	:  5;		/*  Symbol type  */
#else	/*  !COFF  */
	unsigned	s_type	:  3;		/*  Symbol type  */
#endif	/*  !COFF  */
	unsigned	s_newsym:  1;		/*  A new symbol  */
	unsigned	s_invent:  1;		/*  Invented symbol  */
	unsigned	s_glob	:  1;		/*  Global symbol  */
	long		s_value;		/*  Value if defined  */
	short		s_defs;			/*  Defined count  */
	short		s_used;			/*  Used count  */
	unsigned short	s_lsymb;		/*  Local symbol  */
	char		s_name[1];		/*  Chars of name null term */
};

typedef	struct	symstr	*symbol;

symbol	symbhash[HASHMOD];

typedef	struct	{
	int	ef_t;			/*  Text file fd  */
	int	ef_d;			/*  Data file fd  */
	long	ef_entry;		/*  Entry point  */
	long	ef_tsize;		/*  Text size  */
	long	ef_dsize;		/*  Data size  */
	long	ef_bsize;		/*  Bss size  */
	long	ef_end;			/*  End of it all  */
	long	ef_tbase;		/*  Text base  */
	long	ef_dbase;		/*  Data base  */
	long	ef_bbase;		/*  Bss base  */
	int	ef_stcnt;		/*  Number of symbols  */
	int	ef_stmax;		/*  Max number of symbols  */
	symbol	*ef_stvec;		/*  Symbol vector  */
}  ef_fids;

typedef	ef_fids	*ef_fid;

/*
 *	Description of word in text file.  This entry is held in the place
 *	corresponding to the address in the text file.
 */

typedef	struct	{
	unsigned  short	t_contents;		/*  Actual contents  */
	unsigned  short t_iindex;		/*  Index in table  */
	unsigned	t_type	:  2;		/*  Type  */
	unsigned	t_vins  :  1;		/*  Valid instruction  */
	unsigned	t_bdest	:  1;		/*  Is branch dest  */
	unsigned	t_gbdest:  1;		/*  Is global dest  */
	unsigned	t_dref	:  1;		/*  Refered to in data  */
	unsigned	t_bchtyp:  2;		/*  Branch type  */
#ifdef	COFF
	unsigned	t_zilch	:  3;		/*  used to be t_lng */
#else	/*  !COFF  */
	unsigned	t_lng	:  3;		/*  Length in words  */
#endif	/*  !COFF  */
	unsigned	t_reloc :  2;		/*  Relocatable  */
	unsigned	t_rptr	:  2;		/*  Where relocated  */
	unsigned	t_rdisp :  1;		/*  Relocatable displacement */
	unsigned	t_isrel :  1;		/*  Relocated  */
	unsigned	t_amap	:  1;		/*  Worked out  */
#ifdef	COFF
	short		t_lng;			/*  Length in words */
#endif	/*  COFF  */
	symbol		t_relsymb;		/*  Relocation symbol  */
	long		t_reldisp;		/*  Offset + or - from symb */
	symbol		t_lab;			/*  Label  */
	unsigned  short	t_lsymb;		/*  Local symbol  */
	long		t_reflo;		/*  Lowest place referred  */
	long		t_refhi;		/*  Highest place referred  */
	long		t_data;			/*  Data >= here  */
	unsigned  short	t_match;		/*  Lib match lng  */
}  t_entry;

/*
 *	Types ......
 */

#define	T_UNKNOWN	0
#define	T_BEGIN		1
#define	T_CONT		2

#define	R_NONE		0		/*  No relocation  */
#define	R_BYTE		1		/*  Byte relocation  */
#define	R_WORD		2		/*  Word relocation  */
#define	R_LONG		3		/*  Long relocation  */

/*
 *	Branch types.
 */

#define	T_NOBR		0
#define	T_CONDBR	1
#define	T_UNBR		2
#define	T_JSR		3

typedef	struct	{
	unsigned  char	d_contents;		/*  Actual contents  */
	unsigned	d_type  :  4;		/*  Data type  */
	unsigned	d_reloc :  2;		/*  Relocatable  */
	unsigned	d_rptr	:  2;		/*  Where relocated  */
	short		d_lng;			/*  Length -ve for D_CONT */
	symbol		d_relsymb;		/*  Relocation symbol  */
	long		d_reldisp;		/*  Offset + or - from symb */
	symbol		d_lab;			/*  Label  */
}  d_entry;

/*
 *	Data types.
 */

#define	D_ASC		0		/*  Ascii chars  */
#define	D_ASCZ		1		/*  Null-term ascii  */
#define	D_BYTE		2		/*  Decimal bytes  */
#define	D_WORD		3		/*  Words  */
#define	D_LONG		4		/*  Longs  */
#define	D_ADDR		5		/*  Address pointer  */
#define	D_CONT		6		/*  Continuation of last  */

/*
 *	'Common' items.
 */

struct	commit	{
	symbol	*c_symb;		/*  List of symbols  */
	int	c_int;			/*  Current number  */
	int	c_max;			/*  Maximum  */
};

/*
 *	Library file description.
 */

struct	libit	{
#ifdef	COFF
	LDFILE *ldptr,*ldptr2;		/*  independent file pointer packages
					    for the same file */
#else	/*  !COFF  */
	int	lf_fd;			/*  File descriptor  */
	long	lf_offset;		/*  Offset of current file  */
	long	lf_next;		/*  Offset of next file  */
#endif	/*  !COFF  */
	char	lf_name[14];		/*  Name of item  */
};

#ifdef	COFF
/* magic number stuff like Sun */
#define OMAGIC 0407
#define NMAGIC 0410
#define ZMAGIC 0413
#define N_BADMAG(x) \
    (((x).magic)!=OMAGIC && ((x).magic)!=NMAGIC && ((x).magic)!=ZMAGIC)

/* definitions of type for Sun -- used for symstr.type, d_entry.d_rptr */
#define S_UNDF 0x0		/* undefined */
#define S_ABS  0x2		/* absolute */
#define S_TEXT 0x4		/* text */
#define S_DATA 0x6		/* data */
#define S_BSS  0x8		/* bss */
#define S_COMM 0x12		/* common (internal to ld) */
#define S_FN   0x1f		/* file name symbol */
#define S_EXT  01		/* external bit, or'ed in */
#define S_TYPE 0x1e		/* mask for all the type bits */
#endif	/*  COFF  */
