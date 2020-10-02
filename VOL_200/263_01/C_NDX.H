/*
*	Copr.  Marietta Systems, Inc   1988, 1989
*	All rights reserved
*/

typedef long mony; /* 'mony'-type fields have two implied decimal places */

#define DBFTREEDEPTH 16
#if defined (__SMALL__) || defined(__MEDIUM__)
#define MAX_ELEM 32
#else
#define MAX_ELEM 64
#endif

struct DBFKEYED {
	int fldno;    /* field number of key field (-1 if null) */
	int logic;    /* flag / function code for conversion of field */
	int sublogic; /* sublogic function identifier */
	byte lit1, lit2; /* literal values */
	};

struct DBFINDEX {
	long total;   /* total blocks */
	long tree[1 + DBFTREEDEPTH]; /* blocks in current B-Tree (0 is top) */
	long k1;
	int (*makekey)();/* pointer to index field calc function */
	struct DBFKEYED field[8];  /* fields in key */
	int fh;       /* file number of data file, 0 if unused */
	int idepth;   /* number of index levels down the B-tree */
	int keylen;   /* length of key */
	int maxrec;   /* max records per block */
	int reclen;   /* length of key record */
	int nkey;     /* Bool, true if numeric/date key */
	int uniq;     /* Bool, true if unique key */
	int i1;
	};

struct DBFRELATE {
	int fh;       /* file being related */
	int fhx;      /* index file  (==0 for hashed access) */
	int pair[10][2]; /* field pairs in relationship (last == -1)
	                  * pair is orginating field# then target field# */
	};

struct DBFMAP {
	struct DBFRELATE *dbfr; /* pointer to relationship tables */
	int ix[7];    /* index files used by this data file, if ix[0] is zero
				   *  then no index files! */
	int rfh;      /* number of related files */
	};

struct CACHEMAP {
	long recno;   /* 1st record number of block in cache memory */
	void *block;  /* pointer to data area */
	int lastused; /* usage increment number last used */
	int write;    /* write flag, set 1 if block in memory changed */
	};

struct FNCACHE {
	long recused;  /* record in use */
	long lastrec;  /* last physical record used in file */
	struct CACHEMAP cache[MAX_ELEM]; /* pointer to cache table */
	unsigned last; /* last actual disk read modes */
	unsigned records; /* block size in records (1,2,4,8,etc) */
	int c_elem;    /* number of elements in cache table */
	int lastused;  /* last used counter */
	int free;      /* first free element in cache table, -1 if none free */
	int hardwrite; /* set 1 if all writes forced to disk */
	};

/*
* * * * * * * * * * * *
*	Definitions and prototypes for 'mony' type access
*
*/
extern int   acptmony (mony*, enum _JUST, enum ATTRIB, unsigned);
extern mony  atom (byte*);
extern int   dispmony (mony, int, int, enum ATTRIB, enum _JUST, unsigned);
extern byte* ftoascii(byte*, double, unsigned);
extern mony  ftom (double);
#ifdef LATTICE
#define itoascii(a, i) stci_d(a, i)
#define ltoascii(a, l) stcl_d(a, l)
#else
#define itoascii(a, i) itoa(i, a, 10)
#define ltoascii(a, l) ltoa(l, a, 10)
#endif
extern mony  monydisc (mony, mony);
extern mony  monydiv (mony, mony);
extern mony  monymarkup (mony, mony);
extern mony  monymult (mony, mony);
extern mony  monypercent (mony, mony);
extern mony  monyratio (mony, double);
extern byte* mtoascii (byte*, mony);
extern double mtof (mony);

/* */
extern int   _makekey(int, int, struct DBFINDEX*, char*);
extern int   acptdbf (int, unsigned, enum ATTRIB);
extern int   cacheoff (int);
extern int   dispdbf (int, unsigned, int, int, enum ATTRIB);
extern void  dbfdatein(char*, char*);
extern void  dbfdateout(char*, char*);
extern int   dbffield(int, char*);
extern int   dbffile(int);
extern double* dbfgetf(int, unsigned, double*);
extern long* dbfgetl(int, unsigned, long*);
extern mony* dbfgetm(int, unsigned, mony*);
extern char* dbfgets(int, unsigned, char*);
extern int   dbflist(int, char*, int, unsigned);
extern int   dbfputf(int, unsigned, double);
extern int   dbfputl(int, unsigned, long);
extern int   dbfputm(int, unsigned, mony);
extern int   dbfputs(int, unsigned, char*);
extern int   dbfread(int, enum F_READ);
extern int   dbfrelate(int, int, int, char*);
extern int   dbf_fld(int);
extern int   filelist (char*, char*, unsigned, int, char*);
extern int   filemenu (char*);
extern int   filesave (int);
extern int   hashmake(char*, unsigned, unsigned, long, unsigned, unsigned);
extern long  hashread(int, byte*, int);
extern int   iscache (int);
extern int   isuniqndx (int);
extern int   ndxappend(int);
extern int   ndxcustom(int, char*, int(*)(), int);
extern int   ndxdelete(int);
extern int   ndxexprn(int, char*, struct DBFKEYED*);
extern int   ndxfield (int, unsigned);
extern int   ndxopen(int, char*);
extern int   ndxrdex(int, char*, char*, int(*)(), int);
extern int   ndxread(int, enum F_READ);
extern int   ndxwrite(int);

/*
* * * * * * * * * *
*	Prototypes for text display
*/
extern int   compress (byte*);
extern int   dispwrap (byte*, int, int, enum ATTRIB, unsigned, unsigned);
extern int   ln_ex (byte*, byte*, unsigned);

/* */
extern void  dispdbfi(int, struct DBFINDEX*);
extern void  mapcache(int);
extern void  mapblock(int, struct DBFINDEX*);
extern int   cmrt_fw2(int, long*, unsigned, byte*);
/* end */
