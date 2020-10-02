/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      db_var.c  v1.3  (c) 1990  Ken Harris                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*      This software is made available on an AS-IS basis. Unrestricted     */
/*      use is granted provided that the copyright notice remains intact.   */
/*      The author makes no warranties expressed or implied.                */
/*                                                                          */
/****************************************************************************/

#include "dblib.h"

#ifdef ANSI
void db_var_bld_csum(BUFFER);
void db_var_chk_csum(BUFFER);
#else
void db_var_bld_csum();
void db_var_chk_csum();
#endif

/*
 *      db_add_var  -  Add a record to a variable file
 */

void db_add_var(df, user_data, data_size)
 DATA_FILE df;
 char *user_data;
 int data_size;
{
        FILE_HDR  fh;
        BUFFER   buf;
        VAR_REC  vbuf;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        buf->buf_cur_blk  = fh->fh_vfile_size;
        buf->buf_rec_inx  = 0;
	buf->buf_cur_size = sizeof(struct db_var_rec) + data_size;

	if (data_size < 1 || buf->buf_cur_size > buf->buf_size)
	{	db_error = DB_VAR_SIZE_ERROR;
		return;
	}

        db_add_blk = buf->buf_cur_blk;
        db_add_rec = buf->buf_rec_inx;

        vbuf = (VAR_REC) buf->buf_data;
	vbuf->var_stat  = DB_INUSE;
	vbuf->var_dsize = data_size;

        memcpy(vbuf->var_data, user_data, data_size);
	db_var_bld_csum(buf);

        fh->fh_rec_cnt++;
	fh->fh_vfile_size += buf->buf_cur_size;

        db_put_blk(df,df->df_fhdr);
        db_put_blk(df,buf);
}

/*
 *      db_read_first_var  -  Read First Record in a Variable File
 */

void db_read_first_var(df, user_data, data_size)
 DATA_FILE df;
 char *user_data;
 int  *data_size;
{
        FILE_HDR fh;
        BUFFER  buf;
        VAR_REC vrec;
        ulong   psn;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
	vrec = (VAR_REC) buf->buf_data;

        for (psn=DB_FILE_HDR_SIZE; psn < fh->fh_vfile_size;)
        {       db_get_blk(df, psn, buf);
                if (db_error) return;

		db_var_chk_csum(buf);
		if (db_error) return;

		buf->buf_cur_size = sizeof(struct db_var_rec) 
				  + vrec->var_dsize;

		if (vrec->var_stat == DB_INUSE)
		{	*data_size = vrec->var_dsize;
			memcpy(user_data, vrec->var_data, *data_size);
			return;
		}

		psn += buf->buf_cur_size;
        }
        db_error= DB_END_OF_FILE;
}

/*
 *      db_read_next_var  -  Read Next Record in a Variable File
 */

void db_read_next_var(df, user_data, data_size)
 DATA_FILE df;
 char *user_data;
 int  *data_size;
{
        FILE_HDR fh;
        BUFFER  buf;
        VAR_REC vrec;
        ulong   psn;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        vrec = (VAR_REC) buf->buf_data;

	psn = df->df_prev_blk + df->df_prev_vsize;

        while (psn < fh->fh_vfile_size)
        {       db_get_blk(df, psn, buf);
                if (db_error) return;

		db_var_chk_csum(buf);
		if (db_error) return;

		buf->buf_cur_size = sizeof(struct db_var_rec) 
				  + vrec->var_dsize;

		if (vrec->var_stat == DB_INUSE)
		{	*data_size = vrec->var_dsize;
			memcpy(user_data, vrec->var_data, *data_size);
			return;
		}

		psn += buf->buf_cur_size;
        }
        db_error= DB_END_OF_FILE;
}

/*
 *      db_read_last_var  -  Read Last Record in a Variable File
 */

void db_read_last_var(df, user_data, data_size)
 DATA_FILE df;
 char *user_data;
 int  *data_size;
{
        FILE_HDR fh;
        BUFFER  buf;
        VAR_REC vrec;
        ulong   psn;
	char   *c, *d;
	short   vsize;


        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;

        for (psn=fh->fh_vfile_size; psn >= DB_FILE_HDR_SIZE;)
        {       psn -= buf->buf_size;
		db_get_blk(df, psn, buf);
                if (db_error) return;
		psn = buf->buf_cur_blk;

		c = buf->buf_data + buf->buf_cur_size - 2;
		d = (char *)&vsize;

		*d++ = *c++;
		*d   = *c;

		buf->buf_rec_inx  = buf->buf_size 
				  - sizeof(struct db_var_rec) - vsize;
		buf->buf_cur_size = sizeof(struct db_var_rec) + vsize;

		db_var_chk_csum(buf);
		if (db_error) return;

		vrec = (VAR_REC)(buf->buf_data + buf->buf_rec_inx);

		if (vrec->var_stat == DB_INUSE)
		{	*data_size = vrec->var_dsize;
			memcpy(user_data, vrec->var_data, *data_size);
			return;
		}
        }
        db_error= DB_END_OF_FILE;
}

/*
 *      db_read_prev_var  -  Read Previous Record in a Variable File
 */

void db_read_prev_var(df, user_data, data_size)
 DATA_FILE df;
 char *user_data;
 int  *data_size;
{
        FILE_HDR fh;
        BUFFER  buf;
        VAR_REC vrec;
        ulong   psn;
	char   *c, *d;
	short   vsize;


        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;

        for (psn=df->df_prev_blk+df->df_prev_rec; psn >= DB_FILE_HDR_SIZE;)
        {       psn -= buf->buf_size;
		db_get_blk(df, psn, buf);
                if (db_error) return;
		psn = buf->buf_cur_blk;

		c = buf->buf_data + buf->buf_cur_size - 2;
		d = (char *)&vsize;

		*d++ = *c++;
		*d   = *c;

		buf->buf_rec_inx  = buf->buf_cur_size 
				  - sizeof(struct db_var_rec) - vsize;
		buf->buf_cur_size = sizeof(struct db_var_rec) + vsize;

		db_var_chk_csum(buf);
		if (db_error) return;

		vrec = (VAR_REC)(buf->buf_data + buf->buf_rec_inx);

		if (vrec->var_stat == DB_INUSE)
		{	*data_size = vrec->var_dsize;
			memcpy(user_data, vrec->var_data, *data_size);
			return;
		}
        }
        db_error= DB_END_OF_FILE;
}

/*
 *      db_update_var  -  Update Record in a Variable File
 */

void db_update_var(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR  fh;
        BUFFER   buf;
        VAR_REC  vrec;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        if (buf->buf_cur_blk == 0)
        {       db_error = DB_NO_CURRENT_REC;
                return;
        }

	vrec = (VAR_REC)(buf->buf_data + buf->buf_rec_inx);

        if (vrec->var_stat != DB_INUSE)
        {       db_error = DB_DELETED_REC;
                return;
        }

        memcpy(vrec->var_data, user_data, vrec->var_dsize);

	db_var_bld_csum(buf);

        db_put_blk(df, buf);
}                           

/*
 *      db_delete_var  -  Delete Record in a Variable File
 */

void db_delete_var(df)
 DATA_FILE df;
{
        FILE_HDR  fh;
        BUFFER   buf;
        VAR_REC  vrec;
	char    *c;
	int      i;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        if (buf->buf_cur_blk == 0)
        {       db_error = DB_NO_CURRENT_REC;
                return;
        }

	vrec = (VAR_REC)(buf->buf_data + buf->buf_rec_inx);

        if (vrec->var_stat != DB_INUSE)
        {       db_error = DB_DELETED_REC;
                return;
        }

	vrec->var_stat = DB_FREE;
	for (i=0, c=vrec->var_data; i < vrec->var_dsize; i++) *c++ = 0;

	fh->fh_rec_cnt--;

	db_var_bld_csum(buf);

        db_put_blk(df, buf);
	db_put_blk(df,buf);
}                           

/*
 *      db_reorg_var  -  Reorganize a Variable File 
 */

void db_reorg_var(new_df)
 DATA_FILE new_df;
{
	DATA_FILE old_df;
	FILE_HDR  old_fh, new_fh;
        char     *vrec, *calloc();
	int       vsize;


        db_error = 0;

        old_df = (DATA_FILE) calloc(1, sizeof(struct db_data_file));
	if (!old_df)
	{	db_error = DB_ALLOC_ERROR;
		return;
	}

        old_df->df_fhdr = db_alloc_buf(DB_FILE_HDR_SIZE);
        old_fh          = (FILE_HDR) old_df->df_fhdr->buf_data;
	if (db_error) { db_free_df(old_df); return; }

        new_fh = (FILE_HDR) new_df->df_fhdr->buf_data;
	memcpy(old_fh, new_fh, sizeof(struct db_file_hdr));

	new_fh->fh_rec_cnt    = 0;
	new_fh->fh_vfile_size = DB_FILE_HDR_SIZE;

        old_df->df_stat = DB_OPEN;
	old_df->df_fd   = new_df->df_fd;

        old_df->df_buf = db_alloc_buf(old_fh->fh_block_size);
	if (db_error) { db_free_df(old_df); return; }

	vrec = calloc(1, old_fh->fh_data_size);
	if (!vrec)
	{	db_error = DB_ALLOC_ERROR;
		return;
	}

	db_read_first(old_df, vrec, &vsize);
	while (!db_error)
	{	db_add(new_df, vrec, vsize);
		if (db_error) break;

		db_read_next(old_df, vrec, &vsize);
	}

	if (db_error == DB_END_OF_FILE) db_error = 0;

	free(vrec);
	db_free_df(old_df);
}

/*
 *      db_var_bld_csum - Compute var record check sum
 */

void db_var_bld_csum(buf)
 BUFFER   buf;
{
	VAR_REC   var;
        int cnt;          
	char *c, *d, csum;
                                

	if (buf->buf_cur_blk == 0) return;
	
	var = (VAR_REC) (buf->buf_data + buf->buf_rec_inx);
	for (cnt=0, csum=0, c = var->var_data; cnt < var->var_dsize; cnt++)
		csum ^= *c++;

	*c = csum;
	c  = ((char *)var) + sizeof(struct db_var_rec) + var->var_dsize - 2;
	d  = (char *)&var->var_dsize;
	*c++ = *d++;
	*c   = *d;
}

/*
 *      db_var_chk_csum - Check var record check sum
 */

void db_var_chk_csum(buf)
 BUFFER   buf;
{
	VAR_REC var;
        short   cnt;          
	char *c, *d, csum;
                                
	db_error = 0;

	if (buf->buf_cur_blk == 0) return;
	
	var = (VAR_REC) (buf->buf_data + buf->buf_rec_inx);
	if (var->var_dsize <= 0 || 
	    var->var_dsize > buf->buf_cur_size - sizeof(struct db_var_rec))
	{       db_error = DB_VAR_CSUM_ERROR;
		return;
	}

	for (cnt=0, csum=0, c = var->var_data; cnt < var->var_dsize; cnt++)
		csum ^= *c++;

	if (*c != csum)
	{       db_error = DB_VAR_CSUM_ERROR;
		return;
	}

	c  = ((char *)var) + sizeof(struct db_var_rec) + var->var_dsize - 2;
	d  = (char *)&cnt;
	*d++ = *c++;
	*d   = *c;

	if (cnt != var->var_dsize)
	{       db_error = DB_VAR_SIZE_ERROR;
		return;
	}
}
