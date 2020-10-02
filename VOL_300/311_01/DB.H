/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      db.h  v1.3  (c) 1987-1990  Ken Harris                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*      This software is made available on an AS-IS basis. Unrestricted     */
/*      use is granted provided that the copyright notice remains intact.   */
/*      The author makes no warranties expressed or implied.                */
/*                                                                          */
/****************************************************************************/

#ifdef ALL
#define SEQ 1
#define RAN 1
#define IDX 1
#define VAR 1
#endif

#define DB_VERSION    13	      /* Version 1.3			*/
#define FILE_NAME_MAX 50

#define YES -1
#define NO   0

#ifdef ULTRIX
#ifndef _TYPES_
 typedef unsigned short ushort;
 typedef unsigned int   uint;
#endif
 typedef unsigned char  uchar;
 typedef unsigned long  ulong;
#endif

#ifdef SYSV
#ifndef types_h
 typedef unsigned short ushort;
 typedef unsigned int   uint;
#endif
 typedef unsigned char  uchar;
 typedef unsigned long  ulong;
#endif

#ifdef MSC
 typedef unsigned short ushort;
 typedef unsigned int   uint;
 typedef unsigned char  uchar;
 typedef unsigned long  ulong;
#endif

#ifdef TURBO
 typedef unsigned short ushort;
 typedef unsigned int   uint;
 typedef unsigned char  uchar;
 typedef unsigned long  ulong;
#endif

/*
 *      db_file_hdr  -  File Header Record
 */

struct db_file_hdr
{       ushort fh_db_version;           /* Data Base Version #          */
        ushort fh_file_type;            /* File Type                    */
#define                                    DB_SEQ       1
#define                                    DB_RANDOM    2
#define                                    DB_INDEX     3
#define					   DB_VAR       4

        ushort fh_file_stat;            /* File Status                  */
#define                                    DB_CLOSED       0
#define                                    DB_OPEN         1
#define                                    DB_DUP_ALLOWED  2

        ulong  fh_last_block;           /* Last Allocated Block         */
        ushort fh_block_size;           /* Block Size in bytes          */
        ushort fh_rec_size;             /* Record Size in bytes         */
        ushort fh_ctl_size;             /* Control part of rec size     */
        ushort fh_data_size;            /* User data part of rec size   */
        ulong  fh_rec_cnt;              /* Live Record Cnt              */
        ushort fh_recs_per_blk;         /* Records Per Block            */
        ushort fh_root_ptr;             /* Index File Root Pointer      */
        ushort fh_base_size;            /* Random File Base Size        */
        ushort fh_key_size;             /* Size of record key           */
        ulong  fh_next_avail;           /* First Available Record       */
        ushort fh_atr_size;             /* Size of Attribute Block      */
	ulong  fh_vfile_size;		/* Size of VAR file data   V1.3 */
	ushort fh_olink_cnt;		/* Owner link count        V1.3 */
	ushort fh_mlink_cnt;		/* Member link count       V1.3 */
	ushort fh_klink_cnt;		/* Key link count	   V1.3 */
					/* Last part of the header rec  */
					/* is reserved for optional     */
					/* user attribute data.  The    */
					/* size is currently 256 bytes  */
};

#define DB_FILE_HDR_SIZE  512
#define DB_ATR_OFFSET     256

typedef  struct db_file_hdr *FILE_HDR;


/*
 *      db_data_buf  -  Data buffer
 */

struct db_data_buf
{       struct db_data_buf *buf_prev;   /* Previous Buffer in list      */
        struct db_data_buf *buf_next;   /* Next Buffer in list          */
        ushort buf_size;                /* Size of Buffer in Bytes      */
        ushort buf_cur_blk;             /* Current Logical Blk (0=none) */
        ushort buf_cur_size;            /* Current Block Size           */
        ushort buf_rec_inx;             /* Record index into block      */
        char  *buf_data;                /* Pointer to data              */
};

typedef  struct db_data_buf *BUFFER;


/*
 *      db_data_file  -  Data File Descriptor Block
 */

struct db_data_file
{       ushort df_stat;                 /* Data File Status             */
        BUFFER df_fhdr;                 /* File Header Buffer           */
        short  df_fd;                   /* File descriptor              */
        char   df_fname[FILE_NAME_MAX]; /* File name                    */
        ulong  df_prev_blk;             /* Prev Block Read              */
        ushort df_prev_rec;             /* Prev Record Read             */
	ushort df_prev_vsize;		/* Prev var rec size		*/
        struct db_data_buf *df_buf;     /* Data Buffer                  */
        struct db_data_buf *df_tmp;     /* Tmp Data Buffer              */
        struct db_data_buf *df_aux;     /* Another Tmp Data Buffer      */
};

typedef  struct db_data_file *DATA_FILE;
typedef  struct db_data_file *DATA_SET;

#define DB_FH(df) ((FILE_HDR)(df->df_fhdr->duf_data))
                           
#define FH_DB_VERSION(df)   DB_FH(df)->fh_db_version
#define FH_FILE_TYPE(df)    DB_FH(df)->fh_file_type
#define FH_FILE_STAT(df)    DB_FH(df)->fh_file_stat
#define FH_LAST_BLOCK(df)   DB_FH(df)->fh_last_block
#define FH_BLOCK_SIZE(df)   DB_FH(df)->fh_block_size
#define FH_REC_SIZE(df)     DB_FH(df)->fh_rec_size
#define FH_CTL_SIZE(df)     DB_FH(df)->fh_ctl_size
#define FH_DATA_SIZE(df)    DB_FH(df)->fh_data_size
#define FH_REC_CNT(df)      DB_FH(df)->fh_rec_cnt
#define FH_RECS_PER_BLK(df) DB_FH(df)->fh_recs_per_blk
#define FH_ROOT_PTR(df)     DB_FH(df)->fh_root_ptr

/*
 *      Record types
 */

struct db_free_rec
{       char  fre_stat;                 /* Status                       */
#define                                    DB_FREE   '0'
#define                                    DB_INUSE  '1'
        ulong fre_next;                 /* Next Available               */
};

struct db_seq_rec
{       char  seq_stat;                 /* Status                       */
};

struct db_index_hdr
{       char   idx_stat;                /* Status                       */
        ushort idx_parent;              /* Pointer to Parent            */
        ushort idx_rec_cnt;             /* Keys in this block           */
};

struct db_index_rec
{       ushort idx_idx_ptr;             /* Pointer to index block       */
};

struct db_random_hdr
{       char   ran_stat;                /* Status                       */
        ushort ran_next;                /* Overflow pointer             */
        ushort ran_rec_cnt;             /* Live Recs in this block      */
};

/*
 *	This is a dummy structure that is used in sizeof operations
 *	to make sure that db_random_hdr is atleast as big as db_free_rec.
 */
struct db_random_uhdr			
{	union
	{	struct db_free_rec ran_free;
		struct db_random_hdr ran_hdr;
	} uhdr;
};

struct db_random_rec
{       char   ran_stat;                /* Status                       */
};
          
struct db_var_rec
{	char   var_stat;		/* Status			*/
	ushort var_dsize;		/* size of data	in bytes	*/
	char   var_data[3];		/* beginning of data area &	*/
					/* dummy for 2nd size field &	*/
					/* 1 byte checksum at end	*/
};

typedef struct db_free_rec   *FREE_REC;
typedef struct db_seq_rec    *SEQ_REC;
typedef struct db_index_hdr  *INDEX_HDR;
typedef struct db_index_rec  *INDEX_REC;
typedef struct db_random_hdr *RANDOM_HDR;
typedef struct db_random_rec *RANDOM_REC;
typedef struct db_var_rec    *VAR_REC;

/*
 *	Link Structures
 */

struct db_link
{	DATA_FILE owner_file;		/* Pointer to owner File desc	*/
	short     owner_index;		/* Index of owner link rec      */
	short     owner_offset;		/* Offset of owner link rec     */
	DATA_FILE member_file;		/* Pointer to member File desc  */
	short     member_index;		/* Index of member link rec	*/
	short     member_offset;	/* Offset of member link rec    */
};

typedef struct db_link *DB_LINK;


struct db_owner_link
{	long	olnk_first;		/* rec # of first member	*/
	long    olnk_last;		/* rec # of last member		*/
};

typedef struct db_owner_link *OLINK;


struct db_member_link
{	long	mlnk_prev;		/* rec # of prev member		*/
	long	mlnk_next;		/* rec # of next member		*/
	long	mlnk_owner;		/* rec # of owner		*/
};

typedef struct db_member_link *MLINK;


struct db_key_link
{	long  klnk_owner;		/* rec # of the master		*/
};

typedef struct db_key_link *KLINK;

/*
 *	dictionary (symbol table manager) header defs
 */

#define DICT_HASH_SIZE 127
#define DICT_MAGIC     0xF012

/*
 *	db_dict_obj - dictionary object header
 */

struct db_dict_obj
{	short               o_type;	/* object type   	     */
	struct db_dict_obj *next;	/* next object in hash chain */
	short               d_size;	/* data size		     */
	short               n_size;	/* name size		     */
	char		    data;	/* first char of the data    */
				        /* name follows data         */
};

typedef struct db_dict_obj  *DOBJ;

struct db_dict_hdr
{	ushort  magic;			/* Magic Number		     */
	DOBJ	htbl[DICT_HASH_SIZE];	/* Hash Table		     */
};

typedef struct db_dict_hdr  *DICT;

#define DB_FILE_NOT_FOUND      1
#define DB_READ_ERROR          2
#define DB_END_OF_FILE         3
#define DB_WRITE_ERROR         4
#define DB_FILE_NOT_CREATED    5
#define DB_FILE_NOT_OPEN       6
#define DB_INVALID_BLOCK       7
#define DB_BUFFER_ERROR        8
#define DB_NO_CURRENT_REC      9
#define DB_DELETED_REC        10
#define DB_INVALID_FREE       11
#define DB_INVALID_BLK_SIZE   12
#define DB_INVALID_INDEX      13
#define DB_REC_NOT_FOUND      14
#define DB_DUP_NOT_ALLOWED    15
#define DB_INVALID_REQUEST    16
#define DB_INVALID_RANDOM     17
#define DB_INVALID_FHDR       18
#define DB_VERSION_ERROR      19
#define DB_INVALID_LINK       20
#define DB_LINK_ERROR         21
#define DB_FTYPE_ERROR        22
#define DB_INVALID_OPTION     23
#define DB_ALLOC_ERROR        24
#define DB_VAR_SIZE_ERROR     25
#define DB_VAR_CSUM_ERROR     26
#define DB_UNSUPP_FEATURE     27
#define DB_INVALID_DICT       28
#define DB_INVALID_NAME       29
#define DB_INVALID_ERROR      30
#define DB_LAST_ERROR         30

extern int    db_error;
extern ulong  db_match_blk;
extern ushort db_match_rec;
extern ulong  db_add_blk;
extern ushort db_add_rec;

#ifdef ANSI
/*
 *	db_main defs
 */

    struct db_data_file *db_create(char *,char *, char *);
    void db_fhdr_create(struct db_file_hdr *);
    struct db_data_file *db_open(char *,char *);
    void db_fhdr_open(struct db_file_hdr *);
    struct db_data_file *db_close(struct db_data_file *);
    void db_delete(struct db_data_file *);
    unsigned long db_get_rec_no(struct db_data_file *);
    void db_check_df(struct db_data_file *);
    void db_extend(struct db_data_file *,struct db_data_buf *);
    void db_get_next_avail(struct db_data_file *,struct db_data_buf *);
    void db_free_rec(struct db_data_file *,struct db_data_buf *);
    void db_get_blk(struct db_data_file *,long ,struct db_data_buf *);
    void db_put_blk(struct db_data_file *,struct db_data_buf *);
    struct db_data_buf *db_alloc_buf(int );
    struct db_data_buf *db_free_buf(struct db_data_buf *);
    void db_free_df(struct db_data_file *);
    char *db_error_msg(int );
    
    unsigned long db_add();
    void db_read_first();
    void db_read_next();
    void db_read_last();
    void db_read_prev();
    void db_find();
    void db_update();
    void db_read_direct();
    void db_read_atr();
    void db_update_atr();
/*
 *	db_seq defs
 */

    void db_add_seq(struct db_data_file *,char *);
    void db_read_first_seq(struct db_data_file *,char *);
    void db_read_next_seq(struct db_data_file *,char *);
    void db_read_last_seq(struct db_data_file *,char *);
    void db_read_prev_seq(struct db_data_file *,char *);
    void db_update_seq(struct db_data_file *,char *);

/*
 *	db_ran defs
 */

    void db_add_ran(struct db_data_file *,char *);
    void db_find_ran(struct db_data_file *,char *,char *);
    void db_read_first_ran(struct db_data_file *,char *);
    void db_read_next_ran(struct db_data_file *,char *);
    void db_read_last_ran(struct db_data_file *,char *);
    void db_read_prev_ran(struct db_data_file *,char *);
    void db_update_ran(struct db_data_file *,char *);
    void db_delete_ran(struct db_data_file *);
    void db_delete_blk_ran(struct db_data_file *,unsigned long );
    short db_hash_ran(struct db_data_file *,char *);
    short db_search_blk_ran(struct db_data_file *,char *,struct db_data_buf *);

/*
 *	db_idx defs
 */

    void db_add_idx(struct db_data_file *,char *);
    void db_find_insert_idx(struct db_data_file *,char *,int );
    void db_split_blk_idx(struct db_data_file *);
    void db_read_first_idx(struct db_data_file *,int ,char *);
    void db_read_next_idx(struct db_data_file *,char *);
    void db_read_last_idx(struct db_data_file *,int ,char *);
    void db_read_prev_idx(struct db_data_file *,char *);
    void db_get_parent_idx(struct db_data_file *);
    void db_find_first_idx(struct db_data_file *,char *,char *,int );
    void db_delete_idx(struct db_data_file *);
    short db_move_to_leaf_idx(struct db_data_file *);
    void db_balance_idx(struct db_data_file *);
    void db_update_idx(struct db_data_file *,char *);

/*
 *	db_link defs
 */

    void db_add_member_first(DB_LINK);
    void db_add_member_last(DB_LINK);
    void db_delete_member(DB_LINK);
    long db_find_first_member(DB_LINK);
    long db_find_last_member(DB_LINK);
    long db_find_prev_member(DB_LINK);
    long db_find_next_member(DB_LINK);
    long db_find_owner(DB_LINK);
    void db_check_link(DB_LINK);
    int  db_olink_offset(DATA_FILE, int);
    int  db_mlink_offset(DATA_FILE, int);

/*
 * dict defs
 */

    int  db_dict_hash(char *);
    DICT db_dict_init();
    void *db_dict_add(DICT, char *, int, int, void *);
    void db_dict_delete(DICT, char *, int);
    void db_dict_delete_all(DICT, int);
    void *db_dict_find(DICT, char *, int);
    int  db_dict_find_all(DICT, int, char ***, void ***);
    void db_dict_load(DICT, char *, char *);
    void db_dict_dump(DICT, char *, char *);
    void db_dict_close(DICT);

#else
/*
 *	db_main defs
 */

    DATA_FILE db_create();
    void db_fhdr_create();
    DATA_FILE db_open();
    void db_fhdr_open();
    DATA_FILE db_close();
    unsigned long db_add();
    void db_read_first();
    void db_read_next();
    void db_read_last();
    void db_read_prev();
    void db_find();
    void db_update();
    void db_delete();
    unsigned long db_get_rec_no();
    void db_read_direct();
    void db_read_atr();
    void db_update_atr();
    void db_check_df();
    void db_extend();
    void db_get_next_avail();
    void db_free_rec();
    void db_get_blk();
    void db_put_blk();
    struct db_data_buf *db_alloc_buf();
    struct db_data_buf *db_free_buf();
    void db_free_df();
    char *db_error_msg();

/*
 *	db_seq defs
 */

    void db_add_seq();
    void db_read_first_seq();
    void db_read_next_seq();
    void db_read_last_seq();
    void db_read_prev_seq();
    void db_update_seq();

/*
 *	db_ran defs
 */

    void db_add_ran();
    void db_find_ran();
    void db_read_first_ran();
    void db_read_next_ran();
    void db_read_last_ran();
    void db_read_prev_ran();
    void db_update_ran();
    void db_delete_ran();
    void db_delete_blk_ran();
    short db_hash_ran();
    short db_search_blk_ran();

/*
 *	db_idx defs
 */

    void db_add_idx();
    void db_find_insert_idx();
    void db_split_blk_idx();
    void db_read_first_idx();
    void db_read_next_idx();
    void db_read_last_idx();
    void db_read_prev_idx();
    void db_get_parent_idx();
    void db_find_first_idx();
    void db_delete_idx();
    short db_move_to_leaf_idx();
    void db_balance_idx();
    void db_update_idx();

/*
 *	db_link defs
 */

    void db_add_member_first();
    void db_add_member_last();
    void db_delete_member();
    long db_find_first_member();
    long db_find_last_member();
    long db_find_prev_member();
    long db_find_next_member();
    long db_find_owner();
    void db_check_link();
    int  db_olink_offset();
    int  db_mlink_offset();

/*
 * dict defs
 */

    db_dict_hash();
    DICT db_dict_init();
    void *db_dict_add();
    void db_dict_delete();
    void db_dict_delete_all();
    void *db_dict_find();
    db_dict_find_all();
    void db_dict_load();
    void db_dict_dump();
    void db_dict_close();
#endif
