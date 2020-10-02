/*
	this is the structure that controls the show. such a structure
	must be declared for each file in main. also, very important,
	be sure to include an initialization of _allocp = NULL; in
	main() before doing anything else.
	it is also necessary to set _blksiz of '_file' in main if
	you will be using the bseek(), btell() functions. they allow
	easy random access when using records of equal size.
*/

#define RFILE struct _file

struct _file {
	int _rfd;			/* file descripter */
	int _secs;			/* # of sectors in buffer */
	unsigned _frstsec;	/* first sector in buffer */
	unsigned _cursec;	/* cp/m current random sec */
	char _curbyt;		/* current random char */
	char *_nxtbyt;		/* next byte to be processed */
	char *_bufbase;		/* location of base */
	char *_pastbuf;		/* first byte beyond end of buffer */
	char _mode;			/* read, write, append, or direct */
	int _update;		/* buffer modified flag */
	unsigned _curblk;	/* currently addressed block */
	int _blksiz;		/* size of a logical block */
};

struct _file_ptr {
	unsigned _sector;
	char _byt;
};

#define BSFILE struct _bs

struct _bs {
	RFILE *_dfp;						/* fp to 'data_file' */
	char _pfname[15];					/* name  of ptr file */
	int (*_key_comp)();					/* pointer to appropriate comp funct */
	int _data_num;						/* number of data items in file */
	int _data_size;						/* size in bytes of a data item */
	int _max_data;						/* maximum data elements in ptr file */
	int _data_ptr;						/* set by last call to search() */
	int _array_index;					/* element in array foundby bssearch */
	int *_ptr_array;					/* base of pointer array */
};
nt random sec */