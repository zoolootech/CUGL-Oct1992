/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      db_ran.c  v1.3  (c) 1987-1990  Ken Harris                           */
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

/*
 *      db_add_ran  -  Add a record to a random file
 */                                                 

void db_add_ran(df, user_data)
 DATA_FILE df;
 char    *user_data;
{
        FILE_HDR   fh;
        RANDOM_HDR rhdr, thdr;
        RANDOM_REC rrec;
        BUFFER     buf, tmp;    
        short      hval;
        char      *rbuf;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        tmp  = df->df_tmp;
        rhdr = (RANDOM_HDR) buf->buf_data;
        thdr = (RANDOM_HDR) tmp->buf_data;

        hval = db_hash_ran(df, user_data);

        db_get_blk(df, (long) hval, buf);
        if (db_error) return;

        if (rhdr->ran_stat != DB_INUSE)
        {       db_error = DB_INVALID_RANDOM;
                return;
        }

        if (db_search_blk_ran(df, user_data, buf))
        {       db_error = DB_DUP_NOT_ALLOWED;
                return;
        }

        while (rhdr->ran_next)
        {       db_get_blk(df, (long) rhdr->ran_next, buf);
                if (db_error) return;

                if (db_search_blk_ran(df, user_data, buf))
                {       db_error = DB_DUP_NOT_ALLOWED;
                        return;
                }
        }

        tmp->buf_cur_blk = 0;
        if (rhdr->ran_rec_cnt >= fh->fh_recs_per_blk)
        {       buf        = df->df_tmp;
                tmp        = df->df_buf;
                df->df_buf = buf;
                df->df_tmp = tmp;
                rhdr       = (RANDOM_HDR) buf->buf_data;
                thdr       = (RANDOM_HDR) tmp->buf_data;

                db_get_next_avail(df, buf);
                if (db_error) return;

                rhdr->ran_next = 0;
                thdr->ran_next = buf->buf_cur_blk;

        }

        rbuf = buf->buf_data + sizeof(struct db_random_uhdr)
             + rhdr->ran_rec_cnt * fh->fh_rec_size;
        rrec = (RANDOM_REC) rbuf;

        if (rrec->ran_stat != DB_FREE)
        {       db_error = DB_INVALID_FREE;
		printf("ran - free stat = %d\n", rrec->ran_stat);
                return;
        }
        memcpy(rbuf+fh->fh_ctl_size, user_data, fh->fh_data_size);
        rrec->ran_stat = DB_INUSE;

        rhdr->ran_rec_cnt += 1;
        db_add_blk = buf->buf_cur_blk;
        db_add_rec = rhdr->ran_rec_cnt;

        fh->fh_rec_cnt += 1;

        db_put_blk(df, buf);
        if (db_error) return;

        if (tmp->buf_cur_blk) 
        {       db_put_blk(df, tmp);
                if (db_error) return;
        }

        db_put_blk(df, df->df_fhdr);
}

/*
 *      db_find_ran  -  Find a Random record
 */

void db_find_ran(df, user_data, key)
 DATA_FILE  df;
 char     *user_data, *key;
{
        RANDOM_HDR rhdr;
        RANDOM_REC rrec;
        FILE_HDR   fh;
        BUFFER     buf;
        short      hval, rec;
        char      *rbuf;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        rhdr = (RANDOM_HDR) buf->buf_data;

        hval = db_hash_ran(df, key);

        while (hval)
        {       db_get_blk(df, (long)hval, buf);
                if (db_error) return;

                if (rec = db_search_blk_ran(df, key, buf))
                {       buf->buf_rec_inx = rec;
                        rbuf = buf->buf_data + sizeof(struct db_random_uhdr)
                             + (rec - 1) * fh->fh_rec_size;

                        rrec = (RANDOM_REC) rbuf;
                        if (rrec->ran_stat != DB_INUSE)
                        {       db_error = DB_INVALID_RANDOM;
                                return;
                        }
                        memcpy(user_data, rbuf+fh->fh_ctl_size, fh->fh_data_size);
                        return;
                }
                hval = rhdr->ran_next;
        }
        db_error = DB_REC_NOT_FOUND;
        return;
}

/*
 *      db_read_first_ran - Read first record from a random file
 */

void db_read_first_ran(df, user_data)
 DATA_FILE  df;
 char     *user_data;
{
        RANDOM_HDR  rhdr;
        RANDOM_REC  rrec;
        FILE_HDR    fh;
        BUFFER      buf;
        ulong       blk;
        char       *rbuf;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        rhdr = (RANDOM_HDR) buf->buf_data;

        for (blk=1; blk <= fh->fh_last_block; blk++)
        {       db_get_blk(df, blk, buf);
                if (db_error) return;

                if (rhdr->ran_stat == DB_INUSE && rhdr->ran_rec_cnt > 0)
                {       buf->buf_rec_inx = 1;
                        rbuf = buf->buf_data + sizeof(struct db_random_uhdr);

                        rrec = (RANDOM_REC) rbuf;
                        if (rrec->ran_stat != DB_INUSE)
                        {       db_error = DB_INVALID_RANDOM;
                                return;
                        }
                        memcpy(user_data, rbuf+fh->fh_ctl_size, fh->fh_data_size);
                        return;
                }
        }
        db_error = DB_END_OF_FILE;
        return;
}

/*
 *      db_read_next_ran - Read next record from a random file
 */

void db_read_next_ran(df, user_data)
 DATA_FILE  df;
 char     *user_data;
{
        RANDOM_HDR  rhdr;
        RANDOM_REC  rrec;
        FILE_HDR    fh;
        BUFFER      buf;
        ulong       blk;
        ushort      rec;
        char       *rbuf;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        rhdr = (RANDOM_HDR) buf->buf_data;

        if (df->df_prev_rec < fh->fh_recs_per_blk)
        {       blk = df->df_prev_blk;
                rec = df->df_prev_rec;
        }
        else
        {       blk = df->df_prev_blk + 1;
                rec = 0;
        }

        for (; blk <= fh->fh_last_block; blk++, rec=0)
        {       db_get_blk(df, blk, buf);
                if (db_error) return;
                                                   
                if (rhdr->ran_stat != DB_INUSE) continue;
                if (rhdr->ran_rec_cnt <= rec) continue;


                buf->buf_rec_inx = rec + 1;
                rbuf = buf->buf_data + sizeof(struct db_random_uhdr)
                     + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

                rrec = (RANDOM_REC) rbuf;
                if (rrec->ran_stat != DB_INUSE)
                {       db_error = DB_INVALID_RANDOM;
                        return;
                }
                memcpy(user_data, rbuf+fh->fh_ctl_size, fh->fh_data_size);
                return;
        }
        db_error = DB_END_OF_FILE;
        return;
}

/*
 *	db_read_last_ran - Read last record from a random file
 */

void db_read_last_ran(df, user_data)
 DATA_FILE  df;
 char     *user_data;
{
        RANDOM_HDR  rhdr;
        RANDOM_REC  rrec;
        FILE_HDR    fh;
        BUFFER      buf;
        ulong       blk;
        char       *rbuf;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        rhdr = (RANDOM_HDR) buf->buf_data;

	for (blk=fh->fh_last_block; blk >= 1; blk--)
        {       db_get_blk(df, blk, buf);
                if (db_error) return;

                if (rhdr->ran_stat == DB_INUSE && rhdr->ran_rec_cnt > 0)
		{	buf->buf_rec_inx = rhdr->ran_rec_cnt;
			rbuf = buf->buf_data + sizeof(struct db_random_uhdr)
			     + (rhdr->ran_rec_cnt - 1) * fh->fh_rec_size;

                        rrec = (RANDOM_REC) rbuf;
                        if (rrec->ran_stat != DB_INUSE)
                        {       db_error = DB_INVALID_RANDOM;
                                return;
                        }
                        memcpy(user_data, rbuf+fh->fh_ctl_size, fh->fh_data_size);
                        return;
                }
        }
        db_error = DB_END_OF_FILE;
        return;
}

/*
 *	db_read_prev_ran - Read next record from a random file
 */

void db_read_prev_ran(df, user_data)
 DATA_FILE  df;
 char     *user_data;
{
        RANDOM_HDR  rhdr;
        RANDOM_REC  rrec;
        FILE_HDR    fh;
        BUFFER      buf;
        ulong       blk;
        ushort      rec;
        char       *rbuf;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        rhdr = (RANDOM_HDR) buf->buf_data;

	if (df->df_prev_rec > 1)
        {       blk = df->df_prev_blk;
		rec = df->df_prev_rec - 1;
        }
        else
	{	blk = df->df_prev_blk - 1;
                rec = 0;
        }

	for (; blk >= 1; blk--, rec=0)
        {       db_get_blk(df, blk, buf);
                if (db_error) return;
                                                   
                if (rhdr->ran_stat != DB_INUSE) continue;
		if (!rhdr->ran_rec_cnt) continue;

		if (rec)
			buf->buf_rec_inx = rec;
		else
			buf->buf_rec_inx = rhdr->ran_rec_cnt;

                rbuf = buf->buf_data + sizeof(struct db_random_uhdr)
                     + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

                rrec = (RANDOM_REC) rbuf;
                if (rrec->ran_stat != DB_INUSE)
                {       db_error = DB_INVALID_RANDOM;
                        return;
                }
                memcpy(user_data, rbuf+fh->fh_ctl_size, fh->fh_data_size);
                return;
        }
        db_error = DB_END_OF_FILE;
        return;
}


/*
 *      db_update_ran - Update a random record
 */

void db_update_ran(df, user_data)
 DATA_FILE  df;
 char     *user_data;
{
        RANDOM_HDR  rhdr;
        RANDOM_REC  rrec;
        FILE_HDR    fh;
        BUFFER      buf;
        char       *rbuf;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        rhdr = (RANDOM_HDR) buf->buf_data;

        if (rhdr->ran_rec_cnt < buf->buf_rec_inx)
        {       db_error = DB_DELETED_REC;
                return;
        }

        rbuf = buf->buf_data + sizeof(struct db_random_uhdr)
             + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

        rrec = (RANDOM_REC) rbuf;
        if (rrec->ran_stat != DB_INUSE)
        {       db_error = DB_INVALID_RANDOM;
                return;
        }
        memcpy(rbuf+fh->fh_ctl_size, user_data, fh->fh_data_size);
        db_put_blk(df, buf);
        return;
}

/*
 *      db_delete_ran - Delete a random record
 */

void db_delete_ran(df)
 DATA_FILE df;
{
        FILE_HDR    fh;
        RANDOM_HDR  rhdr,  thdr;
        RANDOM_REC  rrec,  trec;
        BUFFER      buf,   tmp;
        char       *rbuf, *tbuf;
        ulong       hval,  next;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        tmp  = df->df_tmp;
        rhdr = (RANDOM_HDR) buf->buf_data;
        thdr = (RANDOM_HDR) tmp->buf_data;

        if (buf->buf_rec_inx > rhdr->ran_rec_cnt)
        {       db_error = DB_DELETED_REC;
                return;
        }

        rbuf = buf->buf_data + sizeof(struct db_random_uhdr) + fh->fh_ctl_size;
        hval = db_hash_ran(df, rbuf);

        df->df_prev_blk = buf->buf_cur_blk;
        df->df_prev_rec = buf->buf_rec_inx - 1;

        if (!rhdr->ran_next)
        {       if (buf->buf_rec_inx < rhdr->ran_rec_cnt)
                {       rbuf = buf->buf_data + sizeof(struct db_random_uhdr)
                             + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

                        tbuf = buf->buf_data + sizeof(struct db_random_uhdr)
                             + (rhdr->ran_rec_cnt - 1) * fh->fh_rec_size;

                        rrec = (RANDOM_REC) rbuf;
                        if (rrec->ran_stat != DB_INUSE)
                        {       db_error = DB_INVALID_RANDOM;
                                return;
                        }
                        memcpy(rbuf, tbuf, fh->fh_rec_size);
                        memset(tbuf, 0, fh->fh_rec_size);

                        trec = (RANDOM_REC) tbuf;
                        trec->ran_stat = DB_FREE;
                }
                else
                if (buf->buf_rec_inx == rhdr->ran_rec_cnt)
                {       rbuf = buf->buf_data + sizeof(struct db_random_uhdr)
                             + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

                        rrec = (RANDOM_REC) rbuf;
                        if (rrec->ran_stat != DB_INUSE)
                        {       db_error = DB_INVALID_RANDOM;
                                return;
                        }

                        memset(rbuf, 0, fh->fh_rec_size);

                        rrec = (RANDOM_REC) rbuf;
                        rrec->ran_stat = DB_FREE;
                }

                rhdr->ran_rec_cnt--;
                db_put_blk(df, buf);
                if (db_error) return;

                if (!rhdr->ran_rec_cnt)
                {       db_delete_blk_ran(df, hval);
                        return;
                }
                return;
        }

        if (rhdr->ran_next)
        {       next = rhdr->ran_next;
                while (next)
                {       db_get_blk(df, next, tmp);
                        if (db_error) return;

                        next = thdr->ran_next;
                }
                                      
                if (!thdr->ran_rec_cnt)
                {       db_error = DB_INVALID_RANDOM;
                        return;
                }

                rbuf = buf->buf_data + sizeof(struct db_random_uhdr)
                     + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

                tbuf = tmp->buf_data + sizeof(struct db_random_uhdr)
                     + (thdr->ran_rec_cnt - 1) * fh->fh_rec_size;

                rrec = (RANDOM_REC) rbuf;
                if (rrec->ran_stat != DB_INUSE)
                {       db_error = DB_INVALID_RANDOM;
                        return;
                }
                memcpy(rbuf, tbuf, fh->fh_rec_size);
                memset(tbuf, 0, fh->fh_rec_size);

                trec = (RANDOM_REC) tbuf;
                trec->ran_stat = DB_FREE;

                thdr->ran_rec_cnt--;
                db_put_blk(df, buf);
                if (db_error) return;

                db_put_blk(df, tmp);
                if (db_error) return;

                if (!thdr->ran_rec_cnt)
                {       db_delete_blk_ran(df, hval);
                        return;
                }
                return;
        }
}

/*
 *      db_delete_blk_ran - Delete empty overflow buckets
 *                          There should be at most one at the end of the chain.
 */

void db_delete_blk_ran(df, base_blk)
 DATA_FILE  df;
 ulong     base_blk;
{
        FILE_HDR    fh;
        RANDOM_HDR  rhdr, thdr;
        BUFFER      buf,  tmp;
        ulong       next;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        tmp  = df->df_tmp;
        rhdr = (RANDOM_HDR) buf->buf_data;
        thdr = (RANDOM_HDR) tmp->buf_data;

        buf->buf_cur_blk = 0;
        tmp->buf_cur_blk = 0;

        next = base_blk;
        while (next)
        {       db_get_blk(df, next, tmp);
                if (db_error) return;

                next = thdr->ran_next;

                if (!thdr->ran_rec_cnt)
                {       if (buf->buf_cur_blk)
                        {       rhdr->ran_next = next;
                                db_put_blk(df, buf);
                                if (db_error) return;
                        }
                        if (tmp->buf_cur_blk > fh->fh_base_size)
                        {       db_free_rec(df, tmp);
                                continue;
                        }
                }

                buf        = df->df_tmp;
                tmp        = df->df_buf;
                df->df_buf = buf;
                df->df_tmp = tmp;
                rhdr = (RANDOM_HDR) buf->buf_data;
                thdr = (RANDOM_HDR) tmp->buf_data;
        }
 }

/*
 *      db_hash_ran  -  Hash a random key
 */

short db_hash_ran(df, key)
 DATA_FILE  df;
 char     *key;
{
        FILE_HDR  fh;
        ulong     hval = 0l;                               
        short     i;

        fh = (FILE_HDR) df->df_fhdr->buf_data;

        for (i=0; i < fh->fh_key_size; i++) hval += key[i];

        hval = (hval % fh->fh_base_size) + 1;

        return((short) hval);
}

/*
 *      db_search_blk_ran  - Search a random block for matching record
 */

short db_search_blk_ran(df, key, buf)
 DATA_FILE  df;
 char     *key;
 BUFFER    buf;
{                            
        RANDOM_HDR rhdr;
        RANDOM_REC rrec;
        FILE_HDR   fh;
        char      *rec;
        short      r_cnt;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        rhdr = (RANDOM_HDR) buf->buf_data;
        rec  = buf->buf_data + sizeof(struct db_random_uhdr);

        for (r_cnt = 1; r_cnt <= rhdr->ran_rec_cnt; r_cnt++)
        {       rrec = (RANDOM_REC) rec;
                if (rrec->ran_stat != DB_INUSE)
                {       db_error = DB_INVALID_RANDOM;
                        return(0);
                }

                if (memcmp(key, rec+fh->fh_ctl_size, fh->fh_key_size)==0) 
                        return(r_cnt);

                rec += fh->fh_rec_size;
        }

        return(0);
}
