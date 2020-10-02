/****************************************************************************/
/*                                                                          */
/*                 S O R T  -  Callable Sort Facility                       */
/*                             (c) 1987-1990 Ken Harris                     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*      This software is made available on an AS-IS basis. Unrestricted     */
/*      use is granted provided that the copyright notice remains intact.   */
/*      The author makes no warranties expressed or implied.                */
/*                                                                          */
/****************************************************************************/

char *copywrite = "sort v1.3 (c) 1987-1990  Ken Harris";

#include "dblib.h"

#define SORT_CLOSED          0
#define SORT_OPEN	     1
#define SORT_DONE	     2
#define SORT_EOF             3
#define MIN_BUFFER_SIZE   1000
#define MAX_BUFFER_SIZE  32000

struct sort_buffer
{	int   sb_file;			/* file descriptor		*/
	long  sb_fcnt;			/* file block count		*/
	long  sb_fpos;			/* file position		*/
	char *sb_badr;			/* buffer address		*/
	long  sb_bsiz;			/* buffer size in bytes 	*/
	long  sb_rsiz;			/* buffer size in records	*/
	long  sb_rcnt;			/* current record count 	*/
	char *sb_radr;			/* current record address	*/
};

struct key_field
{	struct key_field *kf_next;	/* next key field		*/
	char   kf_type; 		/* field type  'A' = Ascii	*/
					/*	       'I' = Integer	*/
					/*	       'U' = Unsigned	*/
	char   kf_seq;			/* sort order  'A' = Ascending	*/
					/*	       'D' = Descending */
	int    kf_pos;			/* starting position		*/
	int    kf_size; 		/* field size			*/
};

static int  sfile1;			/* temp file #1 		*/
static int  sfile2;			/* temp file #2 		*/
static int  sfile3;			/* temp file #3 		*/

static int  sort_state = 0;		/* current state of the sort	*/  
					/* 0 = Sort not initialized	*/
					/* 1 = Sort is	initialized	*/
					/* 2 = Sort has been done	*/

static int  sort_rec_size;		/* Size of user data record	*/
static long sort_rec_cnt;		/* Record Count 		*/
static int  sort_key_size;		/* Size of user key		*/
static int  sort_krec_size;		/* Size of key + control info	*/   
static long sort_merge_cnt;		/* count records in merge pass	*/
static long sort_next_fpos;		/* position of next file block	*/
static struct key_field *key_spec=NULL; /* sort key specs		*/
static char *sort_key_rec;

static struct sort_buffer sbuf1;	/* Sort buffer #1		*/
static struct sort_buffer sbuf2;	/* Sort buffer #2		*/
static struct sort_buffer sbuf3;	/* Sort buffer #3		*/

#ifdef ANSI
	static one_merge_pass();
	static char *sort_read(struct sort_buffer *);
	static merge_write(struct sort_buffer *);
	static sort_close();
	static sort_write(int, char *, int);
	static sort_error(char *);
#else
	static one_merge_pass();
	static char *sort_read();
	static merge_write();
	static sort_close();
	static sort_write();
	static sort_error();
#endif

long lseek();

/*
 *	sort_init  -  Initialize the sort data
 */

sort_init(rec_size, spec_str)
 int   rec_size;
 char *spec_str;
{
#ifdef MSC
        unsigned _memavl();
#endif
#ifdef TURBO
	unsigned coreleft();
#endif
        long  buf_size;
        char *calloc();

	if (sort_state != SORT_CLOSED)
		sort_error("Sort Already Initialized");

	sort_rec_size	= rec_size;
	bld_key_spec(spec_str);

        sort_krec_size  = sort_key_size + sizeof(long);
	sort_krec_size += sort_krec_size % 2;
	sort_rec_cnt	= 0;

        sort_key_rec = (char *) calloc(1,sort_key_size);

#ifdef ULTRIX
        sfile1 = open("SORT1.TMP", O_CREAT|O_RDWR, S_IREAD|S_IWRITE);
        sfile2 = open("SORT2.TMP", O_CREAT|O_RDWR, S_IREAD|S_IWRITE);
        sfile3 = open("SORT3.TMP", O_CREAT|O_RDWR, S_IREAD|S_IWRITE);
#endif
#ifdef SYSV
        sfile1 = open("SORT1.TMP", O_CREAT|O_RDWR, S_IREAD|S_IWRITE);
        sfile2 = open("SORT2.TMP", O_CREAT|O_RDWR, S_IREAD|S_IWRITE);
        sfile3 = open("SORT3.TMP", O_CREAT|O_RDWR, S_IREAD|S_IWRITE);
#endif
#ifdef MSC
        sfile1 = open("SORT1.TMP", O_CREAT|O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
        sfile2 = open("SORT2.TMP", O_CREAT|O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
        sfile3 = open("SORT3.TMP", O_CREAT|O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
#endif
#ifdef TURBO
        sfile1 = open("SORT1.TMP", O_CREAT|O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
        sfile2 = open("SORT2.TMP", O_CREAT|O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
        sfile3 = open("SORT3.TMP", O_CREAT|O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
#endif

	if (sfile1 < 0 || sfile2 < 0 || sfile3 < 0)
		sort_error("Can't Open Temp Files");
 
#ifdef ULTRIX
	buf_size  = MAX_BUFFER_SIZE;
#endif
#ifdef SYSV
	buf_size  = MAX_BUFFER_SIZE;
#endif
#ifdef MSC
        buf_size  = _memavl();
#endif
#ifdef TURBO
	buf_size  = coreleft();
#endif
	if (buf_size > MAX_BUFFER_SIZE) buf_size = MAX_BUFFER_SIZE;
	buf_size  = (buf_size * 3)/4;
	buf_size -= buf_size % sort_krec_size;

	if (buf_size < MIN_BUFFER_SIZE)
		sort_error("Insufficient Memory");

        sbuf1.sb_badr = (char *) calloc(1,buf_size);
	sbuf1.sb_bsiz = buf_size;
	sbuf1.sb_rsiz = buf_size/sort_krec_size;
	sbuf1.sb_radr = sbuf1.sb_badr;
	sbuf1.sb_rcnt = 0;

	sort_state = SORT_OPEN;
}				      

/*
 *	bld_key_spec  -  Build Sort Key Spec
 */
bld_key_spec(s)
 char *s;
{
        struct  key_field *f;
        char   *calloc(), ch;

	sort_key_size = 0;
	f = key_spec  = NULL;
	while (*s)
	{
		if (!key_spec)
                        key_spec = f = (struct key_field *) calloc(1,sizeof(struct key_field));
		else
                {       f->kf_next = (struct key_field *) calloc(1,sizeof(struct key_field));
			f = f->kf_next;
		}

		f->kf_next = NULL;
		ch = *s++;
		if (islower(ch)) ch = toupper(ch);
		if (ch=='A' || ch=='D')
			f->kf_seq = ch;
		else
			sort_error("Invalid Key Spec - Unknown Sequence");

		ch = *s++;
		if (islower(ch)) ch = toupper(ch);
		if (ch=='A' || ch=='I' || ch=='U' || ch=='R')
			f->kf_type = ch;
		else
			sort_error("Invalid Key Spec - Unknown Field Type");

		f->kf_pos=0;
		while (isdigit(*s))
			f->kf_pos = 10 * f->kf_pos + (*s++ - '0');

		if (f->kf_pos < 1)
			sort_error("Invalid Sort Spec - Invalid Starting Position");

		if (*s++ != '.')
			sort_error("Invalid Sort Spec - Missing Field Size");

		f->kf_size=0;
		while (isdigit(*s))
			f->kf_size = 10 * f->kf_size + (*s++ - '0');
		     
		sort_key_size += f->kf_size;

		if (f->kf_pos + f->kf_size - 1 > sort_rec_size)
			sort_error("Invalid Sort Spec - Field Exceeds Record");

		if (*s==',') s++;
	}
}

/*
 *	sort_release  -  Release a record to the sort
 */

sort_release(data)
 char *data;
{
	int  sort_cmp();
	long *k_cnt;
	char *k_dat;


	if (sort_state != SORT_OPEN)
		sort_error("Improper Record Release");

	bld_sort_key(data);

	sort_rec_cnt++;
	sort_write(sfile1, data, sort_rec_size);

	k_cnt  = (long *) sbuf1.sb_radr;
	k_dat  = sbuf1.sb_radr + sizeof(long);
	*k_cnt = sort_rec_cnt;
	memcpy(k_dat, sort_key_rec, sort_key_size);

	sbuf1.sb_rcnt++;
        if (sbuf1.sb_rcnt < sbuf1.sb_rsiz)
		sbuf1.sb_radr += sort_krec_size;
	else
	{	qsort(sbuf1.sb_badr, (int)sbuf1.sb_rcnt, sort_krec_size, sort_cmp);
		sort_write(sfile2, (char *)&sbuf1.sb_rcnt, sizeof(long));
		sort_write(sfile2, sbuf1.sb_badr, sbuf1.sb_rcnt*sort_krec_size);
		sbuf1.sb_rcnt = 0;
		sbuf1.sb_radr = sbuf1.sb_badr;
	}
}




/*
 *	sort_cmp  -  sort compare function
 */
 sort_cmp(s1,s2)
  char *s1, *s2;
{
	return(memcmp(s1+sizeof(long),s2+sizeof(long),sort_key_size));
}

/*
 *	bld_sort_key  -  Build the sort key record
 */

bld_sort_key(data)
 char *data;
{
	struct key_field *f;
	char *s,*d;
	int  i, tmp, cmpl;

	d = sort_key_rec;
	f = key_spec;	 

	while (f)
	{
		s = data + f->kf_pos - 1;

		switch(f->kf_type)
		{	case 'A':      
				for (i=0; i < f->kf_size; i++)
					if (f->kf_seq == 'A')
						*d++ = *s++;
					else
						*d++ = ~*s++;
				break;

                        case 'U':
				s += f->kf_size - 1;

                                for (i=0; i < f->kf_size; i++)
					if (f->kf_seq == 'A')
						*d++ = *s--;
					else
						*d++ = ~*s--;
				break;

			case 'I':
				s += f->kf_size - 1;

                                if (f->kf_seq == 'A')
					*d++ = (*s--)^0200;
				else
					*d++ = ~((*s--)^0200);

                                for (i=1; i < f->kf_size; i++)
					if (f->kf_seq == 'A')
						*d++ = *s--;
					else
						*d++ = ~*s--;

                                break;

                        case 'R':
				s += f->kf_size - 1;

				if (*s & 0200)
				{	cmpl = 1;
					tmp  = ~(*s--);
				}		      
				else
				{	cmpl = 0;
					tmp  = (*s--) ^ 0200;
				}

				if (f->kf_seq == 'D') 
				{	cmpl ^= 1;
					*d++  = ~tmp;
				}
				else
					*d++ = tmp;

				for (i=1; i < f->kf_size; i++)
				{	if (cmpl)
						*d++ = ~*s--;
					else
						*d++ = *s--;
				}
                                break;

                        default: break;
		}

		f = f->kf_next;
	}
}

/*
 *	sort_merge  -  Do the merge
 */

sort_merge()
{
	if (sort_state != SORT_OPEN)
		sort_error("Merge Called Out of Sequence");

        if (!sort_rec_cnt)
        {       sort_state = SORT_EOF;
                return;
        }

	merge_init();

        while (sort_state != SORT_DONE)
	{	lseek(sfile2, 0L, 0);
		lseek(sfile3, 0L, 0);

		sort_merge_cnt = 0;
		sort_next_fpos = 0;

		sbuf3.sb_radr = sbuf3.sb_badr;
		sbuf3.sb_rsiz = sbuf3.sb_bsiz / sort_krec_size;
		sbuf3.sb_rcnt = 0;

                one_merge_pass();

		if (sort_merge_cnt != sort_rec_cnt)
			sort_error("Merge Count Error");

		if (sort_state != SORT_DONE)
		{	sbuf1.sb_file = sbuf3.sb_file;
			sbuf3.sb_file = sbuf2.sb_file;
			sbuf2.sb_file = sbuf1.sb_file;
		}
	}

	sort_merge_cnt = 0;
	sort_next_fpos = 0;
        sbuf1.sb_bsiz += sbuf2.sb_bsiz + sbuf3.sb_bsiz;
        sbuf1.sb_rsiz =  sbuf1.sb_rcnt = 0;
        sbuf1.sb_radr = NULL;
	lseek(sbuf1.sb_file, 0L, 0);
        get_merge_block(&sbuf1);
}

static one_merge_pass()
{
        char *r1, *r2, *sort_read();
	long block_size;

	while (sort_merge_cnt < sort_rec_cnt)
	{	get_merge_block(&sbuf1);
		get_merge_block(&sbuf2);

		if (sbuf1.sb_fcnt >= sort_rec_cnt)
		{	sort_state = SORT_DONE;
			return;
		}

		block_size = sbuf1.sb_fcnt + sbuf2.sb_fcnt;
		sort_write(sbuf3.sb_file, (char *)&block_size, sizeof(long));

		r1 = sort_read(&sbuf1);
                r2 = sort_read(&sbuf2);

		while (r1 && r2)
		{	switch (sort_cmp(r1,r2))
			{	case -1: merge_write(&sbuf1);
					 r1 = sort_read(&sbuf1);
					 break;

				case  0: merge_write(&sbuf1);
					 merge_write(&sbuf2);
					 r1 = sort_read(&sbuf1);
					 r2 = sort_read(&sbuf2);
					 break;

				case  1: merge_write(&sbuf2);
					 r2 = sort_read(&sbuf2);
					 break;
			}
		}
		while (r1)
		{	merge_write(&sbuf1);
			r1 = sort_read(&sbuf1);
		}

		while (r2)
		{	merge_write(&sbuf2);
			r2 = sort_read(&sbuf2);
		}

		if (sbuf3.sb_rcnt)
		{	sort_write(sbuf3.sb_file, sbuf3.sb_badr, sbuf3.sb_rcnt*sort_krec_size);
			sbuf3.sb_rcnt = 0;
			sbuf3.sb_radr = sbuf3.sb_badr;
		}
        }
}

/*
 *	merge_init  -  Merge Initialization
 */

merge_init()
{
	unsigned int buf_size;


	if (sbuf1.sb_rcnt)
	{	qsort(sbuf1.sb_badr, (int)sbuf1.sb_rcnt, sort_krec_size, sort_cmp);
		sort_write(sfile2, (char *)&sbuf1.sb_rcnt, sizeof(long));
		sort_write(sfile2, sbuf1.sb_badr, sbuf1.sb_rcnt*sort_krec_size);
		sbuf1.sb_rcnt = 0;
		sbuf1.sb_radr = sbuf1.sb_badr;
	}

	buf_size  = sbuf1.sb_bsiz / 3;
	buf_size -= buf_size % sort_krec_size;		 

	sbuf1.sb_bsiz = buf_size;
	sbuf2.sb_bsiz = buf_size;
	sbuf3.sb_bsiz = buf_size;

	sbuf2.sb_badr = sbuf1.sb_badr + buf_size;
	sbuf3.sb_badr = sbuf2.sb_badr + buf_size;

        sbuf1.sb_rsiz = sbuf1.sb_rcnt = 0;
        sbuf2.sb_rsiz = sbuf2.sb_rcnt = 0;
        sbuf3.sb_rsiz = sbuf3.sb_rcnt = 0;

        sbuf1.sb_radr = NULL;
        sbuf2.sb_radr = NULL;
        sbuf3.sb_radr = NULL;

        sbuf1.sb_file = sfile2;
	sbuf1.sb_fcnt = 0;
	sbuf1.sb_fpos = 0L;

	sbuf2.sb_file = sfile2;
	sbuf2.sb_fcnt = 0;
	sbuf2.sb_fpos = 0L;

	sbuf3.sb_file = sfile3;
	sbuf3.sb_fcnt = 0;
	sbuf3.sb_fpos = 0L;
}

/*
 *	sort_return  -	return a record from the sort
 */

char *sort_return(buf)
 char *buf;
{      
	long *key;
        char *sort_read();

        if (sort_state == SORT_EOF)
	{	sort_close();
		return(NULL);
	}

	if (sort_state != SORT_DONE)
		sort_error("Sort Return Sequence Error");

        key = (long *) sort_read(&sbuf1);

	if (!key)
	{	sort_close();
		return(NULL);
	}

        lseek(sfile1, (long)((*key-1)*sort_rec_size), 0);
        if (read(sfile1, buf, sort_rec_size) != sort_rec_size)
		sort_error("Return Read Error");

        return(buf);
}

/*
 *	sort_read  -  Read record from merge buffer
 */

static char *sort_read(buf)
 struct sort_buffer *buf;
{			   
	int cnt;

	if (buf->sb_rcnt < buf->sb_rsiz)
	{	buf->sb_rcnt++;
		buf->sb_radr += sort_krec_size;
		return(buf->sb_radr);
	}

	if (!buf->sb_fcnt) return(NULL);

	lseek(buf->sb_file, buf->sb_fpos, 0);

	buf->sb_rsiz = buf->sb_bsiz / sort_krec_size;
	if (buf->sb_rsiz > buf->sb_fcnt)
		buf->sb_rsiz = buf->sb_fcnt;

	if (read(buf->sb_file, buf->sb_badr, buf->sb_rsiz * sort_krec_size) <= 0)
		sort_error("Temp File Read Error");
	
	buf->sb_fcnt   -= buf->sb_rsiz;
	buf->sb_fpos   += buf->sb_rsiz * sort_krec_size;
	buf->sb_rcnt	= 1;
	buf->sb_radr	= buf->sb_badr;

	return(buf->sb_radr);
}

/*
 *	get_merge_block  -  Read up the next merge block
 */
get_merge_block(buf)
 struct sort_buffer *buf;
{
	buf->sb_rsiz = 0;
	buf->sb_rcnt = 0;
	buf->sb_radr = 0;

        if (sort_merge_cnt >= sort_rec_cnt)
	{	buf->sb_fcnt = 0;
		return;
	}

	lseek(buf->sb_file, sort_next_fpos, 0);
	if (read(buf->sb_file, &buf->sb_fcnt, sizeof(long)) <= 0)
		sort_error("Merge Read Error");

	buf->sb_fpos	= sort_next_fpos + sizeof(long);
	sort_merge_cnt += buf->sb_fcnt;
	sort_next_fpos += sizeof(long) + buf->sb_fcnt * sort_krec_size;
}
                                         
/*
 *	merge_write  -	merge output
 */

static merge_write(buf)
 struct sort_buffer *buf;
{
	memcpy(sbuf3.sb_radr, buf->sb_radr, sort_krec_size);

	sbuf3.sb_rcnt++;
	if (sbuf3.sb_rcnt < sbuf3.sb_rsiz)
		sbuf3.sb_radr += sort_krec_size;
	else
	{	sort_write(sbuf3.sb_file, sbuf3.sb_badr, sbuf3.sb_rcnt*sort_krec_size);
		sbuf3.sb_rcnt = 0;
		sbuf3.sb_radr = sbuf3.sb_badr;
	}
}

/*
 *	sort_close  -  Cleanup after a sort
 */

static sort_close()
{
	struct key_field *f, *t;

        if (sort_state == SORT_CLOSED) return;

	close(sfile1);
	close(sfile2);
	close(sfile3);

	unlink("SORT1.TMP");
	unlink("SORT2.TMP");
	unlink("SORT3.TMP");

        f = key_spec;
	while (f)
	{	t = f->kf_next;
		free(f);
		f = t;
	}

        free(sort_key_rec);
        free(sbuf1.sb_badr);

	sort_state = SORT_CLOSED;
}

/*
 *	sort_write  -  Write a reord to temp file
 */

static sort_write(fd, buf, cnt)
 int   fd, cnt;
 char *buf;    
{
	if (write(fd, buf, cnt) < cnt)
		sort_error("Write Error, Check Disk Space");
}

/*
 *	sort_error  -	Sort Error Exit
 */

static sort_error(text)
 char *text;
{
	fprintf(stderr,"\n\nSort Error: %s\7\n\n",text);
	exit(1);
}
