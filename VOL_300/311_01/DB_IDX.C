/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      db_idx.c  v1.3  (c) 1987-1990  Ken Harris                           */
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
 *      db_add_idx  -  Add record to an index file
 */

void db_add_idx(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR    fh;
        INDEX_HDR ihdr;
        INDEX_REC irec;
        BUFFER     buf;
        char     *rbuf, *src, *dst;
        ushort     rec;
        int        cnt;

        db_error     = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        ihdr = (INDEX_HDR) buf->buf_data;
        rbuf = buf->buf_data + sizeof(struct db_index_hdr);

        if (!fh->fh_root_ptr)
        {       db_get_next_avail(df, buf);
                if (db_error) return;
        }
        else
        {       db_find_insert_idx(df, user_data, fh->fh_key_size);
                if (db_match_blk)
                        if (!(fh->fh_file_stat & DB_DUP_ALLOWED))
                        {       db_error = DB_DUP_NOT_ALLOWED;
                                return;
                        }
        }
                 
        ihdr->idx_rec_cnt++;
        rec = buf->buf_rec_inx;

        db_add_blk = buf->buf_cur_blk;
        db_add_rec = buf->buf_rec_inx;

        if (rec <= ihdr->idx_rec_cnt)
        {       src = rbuf + (rec - 1) * fh->fh_rec_size;
                dst = src + fh->fh_rec_size;
                cnt = (ihdr->idx_rec_cnt - rec + 1) * fh->fh_rec_size;
                memcpy(dst, src, cnt);
        }                             
        
        irec = (INDEX_REC) src;
        irec->idx_idx_ptr = 0;
        dst = (char *) irec + fh->fh_ctl_size;
        memcpy(dst, user_data, fh->fh_data_size);

        fh->fh_rec_cnt++;

        db_split_blk_idx(df);
        if (db_error) return;

        db_put_blk(df, df->df_fhdr);
}
      
/*
 *      db_find_insert_idx  -  Find Insert Point in an index File
 */

void db_find_insert_idx(df, key, key_size)
 DATA_FILE df;
 char *key;                         
 int   key_size;
{
        FILE_HDR    fh;
        INDEX_HDR ihdr;
        INDEX_REC irec;
        BUFFER     buf;
        char     *rbuf, *ikey;
        ulong      blk;
        ushort     rec;
        int          x;
                       

        db_error     = 0;
        db_match_blk = 0;
        db_match_rec = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        ihdr = (INDEX_HDR) buf->buf_data;
        blk  = fh->fh_root_ptr;

        if (!key_size) key_size = fh->fh_key_size;

        while (blk)
        {       db_get_blk(df, blk, buf);
                if (db_error) return;

                rbuf = buf->buf_data + sizeof(struct db_index_hdr);

                for (rec=1; rec <= ihdr->idx_rec_cnt; rec++)
                {       irec = (INDEX_REC) rbuf;
                        ikey = rbuf + fh->fh_ctl_size;

                        x = memcmp(key, ikey, key_size);
        
                        if (x == 0)
                        {       db_match_blk = blk;
                                db_match_rec = rec;
                                blk = irec->idx_idx_ptr;
                                break;
                        }

                        if (x < 0)
                        {       blk = irec->idx_idx_ptr;
                                break;
                        }

                        rbuf += fh->fh_rec_size;

                        if (rec == ihdr->idx_rec_cnt)
                        {       irec = (INDEX_REC) rbuf;
                                blk  = irec->idx_idx_ptr;
                        }
                }        
        }
        buf->buf_rec_inx = rec;
}

/*
 *      db_split_blk_idx  -  Check for a block split
 */

void db_split_blk_idx(df)
 DATA_FILE df;
{
        FILE_HDR   fh;
        BUFFER     buf,  tmp,  aux;
        INDEX_HDR ihdr,  thdr, ahdr;
        INDEX_REC  idx;
        char     *hold, *rbuf, *src,  *dst;
        ushort     cnt,  cnt1,  cnt2;
        ushort     rec,  left, right;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;
        tmp = df->df_tmp;

        ihdr = (INDEX_HDR) buf->buf_data;

        if (ihdr->idx_rec_cnt <= fh->fh_recs_per_blk)
        {       db_put_blk(df, buf);
                return;
        }

        buf        = df->df_tmp;
        tmp        = df->df_buf;
        df->df_buf = buf;
        df->df_tmp = tmp;
        aux        = df->df_aux;

        db_get_next_avail(df, buf);
        if (db_error) return;

        ihdr = (INDEX_HDR) buf->buf_data;
        thdr = (INDEX_HDR) tmp->buf_data;
        ahdr = (INDEX_HDR) aux->buf_data;

        left   = tmp->buf_cur_blk;
        right  = buf->buf_cur_blk;

        cnt1 = thdr->idx_rec_cnt / 2;
        cnt2 = thdr->idx_rec_cnt - (cnt1 + 1);

        ihdr->idx_parent  = thdr->idx_parent;
        ihdr->idx_rec_cnt = cnt2;         

        src = tmp->buf_data + sizeof(struct db_index_hdr)
                                                + (cnt1 + 1) * fh->fh_rec_size;
        dst = buf->buf_data + sizeof(struct db_index_hdr);
        cnt = (cnt2 + 1) * fh->fh_rec_size;
        memcpy(dst, src, cnt);

        db_put_blk(df, buf);
        if (db_error) return;

        if (db_add_blk == tmp->buf_cur_blk)
                if (db_add_rec > cnt1 + 1)
                {       db_add_blk  =  buf->buf_cur_blk;
                        db_add_rec -= cnt1 + 1;         
                }

        rbuf = buf->buf_data + sizeof(struct db_index_hdr);
        if (!thdr->idx_parent)
        {       if (tmp->buf_cur_blk != fh->fh_root_ptr)
                {       db_error = DB_INVALID_INDEX;
                        return;
                }
                db_get_next_avail(df, buf);
                thdr->idx_parent = buf->buf_cur_blk;
                fh->fh_root_ptr  = buf->buf_cur_blk;
                idx              = (INDEX_REC) rbuf;
                rec              = 1;
        }
        else
        {       db_get_blk(df, (long)thdr->idx_parent, buf);
                if (db_error) return;

                for (rec=1; rec <= ihdr->idx_rec_cnt+1; rec++)
                {       idx = (INDEX_REC) rbuf;

                        if (idx->idx_idx_ptr == left)
                                break;

                        rbuf += fh->fh_rec_size;
                }

                if (idx->idx_idx_ptr != left)
                {       db_error = DB_INVALID_INDEX;
                        return;
                }
        }

        idx->idx_idx_ptr = right;

        if (db_add_blk == tmp->buf_cur_blk)
                if (db_add_rec == cnt1 + 1)
                {       db_add_blk =  buf->buf_cur_blk;
                        db_add_rec =  rec;
                }

        src = rbuf;
        dst = rbuf + fh->fh_rec_size;
        cnt = ((ihdr->idx_rec_cnt + 1) - rec + 1) * fh->fh_rec_size;
        memcpy(dst, src, cnt);

        src = tmp->buf_data + sizeof(struct db_index_hdr)
                                         + cnt1 * fh->fh_rec_size;
        dst = rbuf;
        cnt = fh->fh_rec_size;
        memcpy(dst, src, cnt);

        idx->idx_idx_ptr = left;
        ihdr->idx_rec_cnt++;

        thdr->idx_rec_cnt = cnt1;
        db_put_blk(df, tmp);
        if (db_error) return;

        db_get_blk(df, (long) right, tmp);
        if (db_error) return;

        if (thdr->idx_parent != buf->buf_cur_blk)
        {       thdr->idx_parent = buf->buf_cur_blk;
                db_put_blk(df, tmp);
                if (db_error) return;
        }

        rbuf = tmp->buf_data + sizeof(struct db_index_hdr);
        for (cnt=1; cnt <= thdr->idx_rec_cnt + 1; cnt++)
        {       idx = (INDEX_REC) rbuf;

                if (idx->idx_idx_ptr)
                {       db_get_blk(df, (long)idx->idx_idx_ptr, aux);
                        if (db_error) return;

                        ahdr->idx_parent = tmp->buf_cur_blk;
                        db_put_blk(df, aux);
                        if (db_error) return;
                }

                rbuf += fh->fh_rec_size;
        }

        db_split_blk_idx(df);
}

/*
 *      db_read_first_idx  -  Read First Record in an Index File
 */

void db_read_first_idx(df, blk, user_data)                           
 DATA_FILE df;
 int blk;
 char *user_data;
{
        FILE_HDR    fh;
        BUFFER     buf;
        INDEX_HDR ihdr;
        INDEX_REC  idx;
        char      *src;

                    
        db_error = 0;

        if (!blk)
        {       db_error = DB_END_OF_FILE;
                return;
        }

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        ihdr = (INDEX_HDR)  buf->buf_data;
        idx  = (INDEX_REC) (buf->buf_data + sizeof(struct db_index_hdr));

        while (blk)
        {       db_get_blk(df, (long) blk, buf);
                if (db_error) return;
         

                if (!ihdr->idx_rec_cnt)
                {       db_error = DB_INVALID_INDEX;
                        return;
                }

                blk = idx->idx_idx_ptr;
        }                              

        src = buf->buf_data + sizeof(struct db_index_hdr) + fh->fh_ctl_size;
        buf->buf_rec_inx = 1;
        memcpy(user_data, src, fh->fh_data_size);
}

/*
 *      db_read_next_idx  -  Read Next Record in an Index File
 */

void db_read_next_idx(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR    fh;
        BUFFER     buf;
        INDEX_HDR ihdr;
        INDEX_REC  idx;
        char      *src;

                    
        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        ihdr = (INDEX_HDR)  buf->buf_data;
                                               
        db_get_blk(df, df->df_prev_blk, buf);
        if (db_error) return;

        buf->buf_rec_inx = df->df_prev_rec;

        if (buf->buf_rec_inx > ihdr->idx_rec_cnt)
        {       while (buf->buf_rec_inx > ihdr->idx_rec_cnt)
                {       db_get_parent_idx(df);
                        if (db_error) return;
                }

                src = buf->buf_data + sizeof(struct db_index_hdr)
                    + (buf->buf_rec_inx - 1) * fh->fh_rec_size
                    + fh->fh_ctl_size;

                memcpy(user_data, src, fh->fh_data_size);
                return;
        }

        buf->buf_rec_inx++;

        idx = (INDEX_REC) (buf->buf_data + sizeof(struct db_index_hdr)
            + (buf->buf_rec_inx - 1) * fh->fh_rec_size);

        if (idx->idx_idx_ptr)
        {       db_read_first_idx(df, idx->idx_idx_ptr, user_data);
                return;
        }

        if (buf->buf_rec_inx > ihdr->idx_rec_cnt)
        {       df->df_prev_rec = buf->buf_rec_inx;
                db_read_next_idx(df, user_data);
                return;
        }

        src = ((char *) idx) + fh->fh_ctl_size;
        memcpy(user_data, src, fh->fh_data_size);
}

/*
 *	db_read_last_idx  -  Read Last Record in an Index File
 */

void db_read_last_idx(df, blk, user_data)
 DATA_FILE df;
 int blk;
 char *user_data;
{
        FILE_HDR    fh;
        BUFFER     buf;
        INDEX_HDR ihdr;
        INDEX_REC  idx;
        char      *src;

                    
        db_error = 0;

        if (!blk)
        {       db_error = DB_END_OF_FILE;
                return;
        }

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        ihdr = (INDEX_HDR)  buf->buf_data;

        while (blk)
        {       db_get_blk(df, (long) blk, buf);
                if (db_error) return;
         

                if (!ihdr->idx_rec_cnt)
                {       db_error = DB_INVALID_INDEX;
                        return;
                }

		idx  = (INDEX_REC) (buf->buf_data + sizeof(struct db_index_hdr)
				   + ihdr->idx_rec_cnt * fh->fh_rec_size);

                blk = idx->idx_idx_ptr;
        }                              

	src = buf->buf_data + sizeof(struct db_index_hdr)
	    + (ihdr->idx_rec_cnt - 1) * fh->fh_rec_size + fh->fh_ctl_size;

	buf->buf_rec_inx = ihdr->idx_rec_cnt;
        memcpy(user_data, src, fh->fh_data_size);
}

/*
 *	db_read_prev_idx  -  Read Prev Record in an Index File
 */

void db_read_prev_idx(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR    fh;
        BUFFER     buf;
        INDEX_HDR ihdr;
        INDEX_REC  idx;
        char      *src;

                    
        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        ihdr = (INDEX_HDR)  buf->buf_data;
                                               
        db_get_blk(df, df->df_prev_blk, buf);
        if (db_error) return;

        buf->buf_rec_inx = df->df_prev_rec;

        idx = (INDEX_REC) (buf->buf_data + sizeof(struct db_index_hdr)
            + (buf->buf_rec_inx - 1) * fh->fh_rec_size);

	if (buf->buf_rec_inx == 1 && idx->idx_idx_ptr == 0)
	{	while (buf->buf_rec_inx == 1)
                {       db_get_parent_idx(df);
                        if (db_error) return;
                }
					    
		buf->buf_rec_inx--;

                src = buf->buf_data + sizeof(struct db_index_hdr)
                    + (buf->buf_rec_inx - 1) * fh->fh_rec_size
                    + fh->fh_ctl_size;

                memcpy(user_data, src, fh->fh_data_size);
                return;
        }

        if (idx->idx_idx_ptr)
	{	db_read_last_idx(df, idx->idx_idx_ptr, user_data);
                return;
        }

	buf->buf_rec_inx--;

        idx = (INDEX_REC) (buf->buf_data + sizeof(struct db_index_hdr)
            + (buf->buf_rec_inx - 1) * fh->fh_rec_size);

        src = ((char *) idx) + fh->fh_ctl_size;
        memcpy(user_data, src, fh->fh_data_size);
}


/*
 *      db_get_parent_idx  -  Get Parent Block
 */

void db_get_parent_idx(df)
 DATA_FILE df;        
{
        FILE_HDR     fh;
        BUFFER      buf;
        INDEX_HDR  ihdr;
        INDEX_REC   idx;
        char      *rbuf;
        ushort hold_blk;
        int         rec;

                    
        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        rbuf = buf->buf_data + sizeof(struct db_index_hdr);
        ihdr = (INDEX_HDR)  buf->buf_data;                 

        hold_blk = buf->buf_cur_blk;
        if (!ihdr->idx_parent)
        {       db_error = DB_END_OF_FILE;
                return;
        }

        db_get_blk(df, (long)ihdr->idx_parent, buf);
        if (db_error) return;

        for (rec=1; rec <= ihdr->idx_rec_cnt + 1; rec++)
        {       idx = (INDEX_REC) rbuf;

                if (idx->idx_idx_ptr == hold_blk)
                        break;

                rbuf += fh->fh_rec_size;
        }

        if (idx->idx_idx_ptr != hold_blk)
        {       db_error = DB_INVALID_INDEX;
                return;
        }

        buf->buf_rec_inx = rec;
}

/*
 *      db_find_first_idx  -  Find First Record
 */

void db_find_first_idx(df, user_data, key, key_size)
 DATA_FILE df;
 char *user_data;
 char *key;
 int   key_size;
{
        FILE_HDR  fh;
        BUFFER   buf;
        char   *rbuf;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;

        db_find_insert_idx(df, key, key_size);
        if (db_error) return;

        if (!db_match_blk)
        {       db_error = DB_REC_NOT_FOUND;
                return;
        }

        db_get_blk(df, db_match_blk, buf);
        if (db_error) return;

        buf->buf_rec_inx = db_match_rec;

        rbuf = buf->buf_data + sizeof(struct db_index_hdr)
             + (buf->buf_rec_inx - 1) * fh->fh_rec_size + fh->fh_ctl_size;

        memcpy(user_data, rbuf, fh->fh_data_size);
}

/*
 *      db_delete_idx  -  Delete an Index Record
 */

void db_delete_idx(df)
 DATA_FILE df;
{
        FILE_HDR     fh;
        BUFFER      buf;
        INDEX_HDR  ihdr;
        char       *src, *dst;
        int         cnt, move_flag;

        db_error = 0;

        move_flag = db_move_to_leaf_idx(df);
        if (db_error) return;       

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        ihdr = (INDEX_HDR) buf->buf_data;

        dst = buf->buf_data + sizeof(struct db_index_hdr)
                            + (buf->buf_rec_inx - 1) * fh->fh_rec_size;
        src = dst + fh->fh_rec_size;
        cnt = (ihdr->idx_rec_cnt - buf->buf_rec_inx + 1) * fh->fh_rec_size;

        if (cnt) memcpy(dst, src, cnt);

        df->df_prev_blk = buf->buf_cur_blk;
        df->df_prev_rec = buf->buf_rec_inx - 1;

        ihdr->idx_rec_cnt--;
        if (fh->fh_rec_cnt) fh->fh_rec_cnt--;

        if (buf->buf_cur_blk == fh->fh_root_ptr  &&  ihdr->idx_rec_cnt == 0)
        {       fh->fh_root_ptr = 0;
                db_free_rec(df, buf);
                if (db_error) return;
        }
        else
        {       db_balance_idx(df);
                if (db_error) return;
        }

        db_put_blk(df, df->df_fhdr);

        if (move_flag)
                db_read_next(df, df->df_tmp->buf_data);
}

/*
 *      db_move_to_leaf_idx  -  Move index record to a leaf node 
 *                              before deleting it
 */

short db_move_to_leaf_idx(df)
 DATA_FILE df;
{
        FILE_HDR     fh;
        BUFFER      buf, tmp;
        INDEX_HDR  ihdr, thdr;
        INDEX_REC   idx, tdx;
        ulong       blk;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        tdx = (INDEX_REC)(buf->buf_data + sizeof(struct db_index_hdr)
                                + (buf->buf_rec_inx - 1) * fh->fh_rec_size);

        if (!tdx->idx_idx_ptr) return(0);

        tmp        = df->df_buf;
        buf        = df->df_tmp;
        df->df_buf = buf;
        df->df_tmp = tmp;
                   
        ihdr = (INDEX_HDR) buf->buf_data;
        thdr = (INDEX_HDR) tmp->buf_data;

        blk = tdx->idx_idx_ptr;

        while (blk)
        {       db_get_blk(df, blk, buf);
                if (db_error) return(0);

                idx = (INDEX_REC)(buf->buf_data + sizeof(struct db_index_hdr)
                                       + ihdr->idx_rec_cnt * fh->fh_rec_size);

                blk = idx->idx_idx_ptr;
        }

        idx = (INDEX_REC)((char *) idx - fh->fh_rec_size);

        idx->idx_idx_ptr = tdx->idx_idx_ptr;
        memcpy(tdx, idx, fh->fh_rec_size);
        idx->idx_idx_ptr = 0;

        db_put_blk(df, tmp);
        if (db_error) return(0);

        buf->buf_rec_inx = ihdr->idx_rec_cnt;

        return(1);
}

/*
 *      db_balance_idx  -  Rebalance tree after a delete
 */

void db_balance_idx(df)
 DATA_FILE df;
{
        FILE_HDR    fh;
        BUFFER     buf,  tmp,  aux;
        INDEX_HDR ihdr, thdr, ahdr;
        INDEX_REC  idx,  tdx,  adx;
        int    min_cnt, hold,  cnt,  op;
        char      *src, *dst;  

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        ihdr = (INDEX_HDR) buf->buf_data;

        if (buf->buf_cur_blk == fh->fh_root_ptr)
        {       db_put_blk(df, buf);
                return;
        }

        min_cnt = (fh->fh_recs_per_blk + 1) / 2;
        if (ihdr->idx_rec_cnt >= min_cnt)
        {       db_put_blk(df, buf);
                return;
        }

        tmp        = df->df_buf;
        buf        = df->df_tmp;
        df->df_buf = buf;
        df->df_tmp = tmp;
        aux        = df->df_aux;

        thdr = (INDEX_HDR) tmp->buf_data;
        ahdr = (INDEX_HDR) aux->buf_data;
        ihdr = (INDEX_HDR) buf->buf_data;

        buf->buf_cur_blk = tmp->buf_cur_blk;
        buf->buf_rec_inx = tmp->buf_rec_inx;
        ihdr->idx_parent = thdr->idx_parent;

        db_get_parent_idx(df);
        if (db_error) return;

        idx  = (INDEX_REC)(buf->buf_data + sizeof(struct db_index_hdr)
                                + (buf->buf_rec_inx - 1) * fh->fh_rec_size);

        if (buf->buf_rec_inx == 1)
        {       adx = (INDEX_REC) ((char *) idx + fh->fh_rec_size);
                op  = 1;
        }
        else
        {       idx = adx = (INDEX_REC) ((char *) idx - fh->fh_rec_size);
                buf->buf_rec_inx--;
                op  = 2;          
        }                              

        db_get_blk(df, (long)adx->idx_idx_ptr, aux);
        if (db_error) return;

        if (ahdr->idx_rec_cnt <= min_cnt) op += 2;

        switch (op)
        {   case 1: /* Rotate Left */
                if (df->df_prev_blk == aux->buf_cur_blk &&
                    df->df_prev_rec == 0)
                {       df->df_prev_blk = buf->buf_cur_blk;
                        df->df_prev_rec = buf->buf_rec_inx;
                }

                if (df->df_prev_blk == buf->buf_cur_blk &&
                    df->df_prev_rec == buf->buf_rec_inx)
                {       df->df_prev_blk = tmp->buf_cur_blk;
                        df->df_prev_rec = thdr->idx_rec_cnt + 1;
                }                       
                else
                if (df->df_prev_blk == aux->buf_cur_blk &&
                    df->df_prev_rec == 1)
                {       df->df_prev_blk = buf->buf_cur_blk;
                        df->df_prev_rec = buf->buf_rec_inx;
                }
                else
                if (df->df_prev_blk == aux->buf_cur_blk)
                        df->df_prev_rec--;

                tdx = (INDEX_REC)(tmp->buf_data + sizeof(struct db_index_hdr)
                            + thdr->idx_rec_cnt * fh->fh_rec_size);
                adx = (INDEX_REC)(aux->buf_data + sizeof(struct db_index_hdr));

                hold = tdx->idx_idx_ptr;
                memcpy(tdx, idx, fh->fh_rec_size);
                tdx->idx_idx_ptr = hold;
                tdx = (INDEX_REC) ((char *) tdx + fh->fh_rec_size);
                tdx->idx_idx_ptr = adx->idx_idx_ptr;
                thdr->idx_rec_cnt++;
                db_put_blk(df,tmp);
                if (db_error) return;

                if (tdx->idx_idx_ptr)
                {       hold = tmp->buf_cur_blk;
                        db_get_blk(df, (long)tdx->idx_idx_ptr, tmp);
                        if (db_error) return;

                        thdr->idx_parent = hold;
                        db_put_blk(df, tmp);
                        if (db_error) return;
                }

                hold = idx->idx_idx_ptr;
                memcpy(idx, adx, fh->fh_rec_size);
                idx->idx_idx_ptr = hold;
                db_put_blk(df, buf);
                if (db_error) return;

                src = (char*) adx + fh->fh_rec_size;
                cnt = ahdr->idx_rec_cnt * fh->fh_rec_size;
                memcpy(adx, src, cnt);
                ahdr->idx_rec_cnt--;
                db_put_blk(df, aux);
                if (db_error) return;
                return;

            case 2: /* Rotate Right */ 
                if (df->df_prev_blk == buf->buf_cur_blk &&
                    df->df_prev_rec == buf->buf_rec_inx)
                {       df->df_prev_blk = tmp->buf_cur_blk;
                        df->df_prev_rec = 1;
                }                       
                else
                if (df->df_prev_blk == aux->buf_cur_blk &&
                    df->df_prev_rec == ahdr->idx_rec_cnt)
                {       df->df_prev_blk = buf->buf_cur_blk;
                        df->df_prev_rec = buf->buf_rec_inx;
                }
                else
                if (df->df_prev_blk == tmp->buf_cur_blk)
                        df->df_prev_rec++;

                tdx = (INDEX_REC)(tmp->buf_data + sizeof(struct db_index_hdr));
                adx = (INDEX_REC)(aux->buf_data + sizeof(struct db_index_hdr)
                                       + ahdr->idx_rec_cnt * fh->fh_rec_size);

                dst = (char *) tdx + fh->fh_rec_size;
                cnt = (thdr->idx_rec_cnt + 1) * fh->fh_rec_size;
                memcpy(dst, tdx, cnt);
                memcpy(tdx, idx, fh->fh_rec_size);
                tdx->idx_idx_ptr = adx->idx_idx_ptr;
                thdr->idx_rec_cnt++;
                db_put_blk(df, tmp);
                if (db_error) return;

                if (tdx->idx_idx_ptr)
                {       hold = tmp->buf_cur_blk;
                        db_get_blk(df, (long)tdx->idx_idx_ptr, tmp);
                        if (db_error) return;

                        thdr->idx_parent = hold;
                        db_put_blk(df, tmp);
                        if (db_error) return;
                }

                hold = idx->idx_idx_ptr;
                adx  = (INDEX_REC) ((char *) adx - fh->fh_rec_size);
                memcpy(idx, adx, fh->fh_rec_size);
                idx->idx_idx_ptr = hold;
                db_put_blk(df, buf);
                if (db_error) return;

                ahdr->idx_rec_cnt--;
                db_put_blk(df, aux);
                if (db_error) return;
                return;

            case 4: /* Merge Right */
                tmp        = df->df_aux;
                aux        = df->df_tmp;
                df->df_aux = aux;
                df->df_tmp = tmp;
                thdr       = (INDEX_HDR) tmp->buf_data;
                ahdr       = (INDEX_HDR) aux->buf_data;

            case 3: /* Merge Left */      
                if (df->df_prev_blk == buf->buf_cur_blk &&
                    df->df_prev_rec == buf->buf_rec_inx)
                {       df->df_prev_blk = tmp->buf_cur_blk;
                        df->df_prev_rec = thdr->idx_rec_cnt + 1;
                }                       
                else
                if (df->df_prev_blk == aux->buf_cur_blk)
                {       df->df_prev_blk  = tmp->buf_cur_blk;
                        df->df_prev_rec += thdr->idx_rec_cnt + 1;
                }                       

                tdx = (INDEX_REC)(tmp->buf_data + sizeof(struct db_index_hdr)
                            + thdr->idx_rec_cnt * fh->fh_rec_size);
                adx = (INDEX_REC)(aux->buf_data + sizeof(struct db_index_hdr));

                hold = tdx->idx_idx_ptr;
                memcpy(tdx, idx, fh->fh_rec_size);
                tdx->idx_idx_ptr = hold;
                tdx = (INDEX_REC) ((char *) tdx + fh->fh_rec_size);
                thdr->idx_rec_cnt++;

                cnt = ahdr->idx_rec_cnt * fh->fh_rec_size + fh->fh_ctl_size;
                memcpy(tdx, adx, cnt);
                thdr->idx_rec_cnt += ahdr->idx_rec_cnt;

                src = (char *) idx + fh->fh_rec_size;
                cnt = (ihdr->idx_rec_cnt - buf->buf_rec_inx)
                                       * fh->fh_rec_size + fh->fh_ctl_size;
                memcpy(idx, src, cnt);
                idx->idx_idx_ptr = tmp->buf_cur_blk;
                ihdr->idx_rec_cnt--;

                src = buf->buf_data + sizeof(struct db_index_hdr)
                    + ihdr->idx_rec_cnt * fh->fh_rec_size + fh->fh_ctl_size;
                cnt = fh->fh_rec_size;
                memset(src, 0, cnt);

		if (buf->buf_cur_blk == fh->fh_root_ptr)
                	if (!ihdr->idx_rec_cnt)
                 	       thdr->idx_parent = 0;

                db_put_blk(df, tmp);
                if (db_error) return;

                hold = ahdr->idx_rec_cnt + 1;
                db_free_rec(df, aux);
                if (db_error) return;

                for (cnt=1; cnt <= hold; cnt++)
                {       if (tdx->idx_idx_ptr)
                        {       db_get_blk(df, (long)tdx->idx_idx_ptr, aux);
                                if (db_error) return;

                                ahdr->idx_parent = tmp->buf_cur_blk;
                                db_put_blk(df, aux);
                                if (db_error) return;
                        }
                        tdx = (INDEX_REC)((char *) tdx + fh->fh_rec_size);
                }

                if (buf->buf_cur_blk == fh->fh_root_ptr)
                {       if (!ihdr->idx_rec_cnt)
                        {       fh->fh_root_ptr = tmp->buf_cur_blk;
                                db_free_rec(df, buf);
                                if (db_error) return;
                        }
                        else
                        {       db_put_blk(df, buf);
                                if (db_error) return;
                        }
                }
                else
                {       db_balance_idx(df);
                        if (db_error) return;
                }

                return;
        }
}

/*
 *      db_update_idx - Update an Index record
 */

void db_update_idx(df, user_data)
 DATA_FILE  df;
 char     *user_data;
{
        INDEX_HDR   ihdr;
        FILE_HDR    fh;
        BUFFER      buf;
        char       *rbuf;

        db_error = 0;

        fh   = (FILE_HDR) df->df_fhdr->buf_data;
        buf  = df->df_buf;
        ihdr = (INDEX_HDR) buf->buf_data;

        if (ihdr->idx_rec_cnt < buf->buf_rec_inx)
        {       db_error = DB_DELETED_REC;
                return;
        }

        rbuf = buf->buf_data + sizeof(struct db_index_hdr)
             + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

        memcpy(rbuf+fh->fh_ctl_size, user_data, fh->fh_data_size);
        db_put_blk(df, buf);
        return;
}
