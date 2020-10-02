/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      db_seq.c  v1.3  (c) 1987-1990  Ken Harris                           */
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
 *      db_add_seq  -  Add a record to a sequential file
 */

void db_add_seq(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR  fh;
        BUFFER   buf;
        char   *rbuf;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        db_get_next_avail(df, buf);
        if (db_error) return;

        db_add_blk = buf->buf_cur_blk;
        db_add_rec = buf->buf_rec_inx;

        rbuf = buf->buf_data + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

        memcpy(rbuf+fh->fh_ctl_size, user_data, fh->fh_data_size);

        fh->fh_rec_cnt++;

        db_put_blk(df,df->df_fhdr);
        db_put_blk(df,buf);
}

/*
 *      db_read_first_seq  -  Read First Record in a Sequential File
 */

void db_read_first_seq(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR fh;
        BUFFER  buf;
        SEQ_REC seq;
        ulong   blk;
        ushort  rec;
        char  *rbuf;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        for (blk=1; blk <= fh->fh_last_block; blk++)
        {       db_get_blk(df, blk, buf);
                if (db_error) return;

                rbuf = buf->buf_data;

                for (rec=1; rec <= fh->fh_recs_per_blk; rec++)
                {       seq = (SEQ_REC) rbuf;

                        if (seq->seq_stat == DB_INUSE)
                        {       buf->buf_rec_inx = rec;
                                memcpy(user_data, rbuf+fh->fh_ctl_size,
                                                              fh->fh_data_size);
                                return;
                        }
                        rbuf += fh->fh_rec_size;
                }
        }
        db_error= DB_END_OF_FILE;
}

/*
 *      db_read_next_seq  -  Read Next Record in a Sequential File
 */

void db_read_next_seq(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR fh;
        BUFFER  buf;
        SEQ_REC seq;
        ulong   blk;
        ushort  rec;
        char  *rbuf;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        db_get_blk(df, df->df_prev_blk, buf);
        if (db_error) return;

        buf->buf_rec_inx = df->df_prev_rec + 1;

        rbuf = buf->buf_data + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

        for (rec=buf->buf_rec_inx; rec <= fh->fh_recs_per_blk; rec++)
        {       seq = (SEQ_REC) rbuf;

                if (seq->seq_stat == DB_INUSE)
                {       buf->buf_rec_inx = rec;
                        memcpy(user_data, rbuf+fh->fh_ctl_size, fh->fh_data_size);
                        return;
                }
                rbuf += fh->fh_rec_size;
        }

        for (blk=buf->buf_cur_blk+1; blk <= fh->fh_last_block; blk++)
        {       db_get_blk(df, blk, buf);
                if (db_error) return;

                rbuf = buf->buf_data;

                for (rec=1; rec <= fh->fh_recs_per_blk; rec++)
                {       seq = (SEQ_REC) rbuf;

                        if (seq->seq_stat == DB_INUSE)
                        {       buf->buf_rec_inx = rec;
                                memcpy(user_data, rbuf+fh->fh_ctl_size,
                                                              fh->fh_data_size);
                                return;
                        }
                        rbuf += fh->fh_rec_size;
                }
        }
        db_error= DB_END_OF_FILE;
}

/*
 *	db_read_last_seq  -  Read Last Record in a Sequential File
 */

void db_read_last_seq(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR fh;
        BUFFER  buf;
        SEQ_REC seq;
        ulong   blk;
        ushort  rec;
        char  *rbuf;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

	for (blk=fh->fh_last_block; blk >= 1; blk--)
        {       db_get_blk(df, blk, buf);
                if (db_error) return;

		rbuf = buf->buf_data + (fh->fh_recs_per_blk - 1) * fh->fh_rec_size;

		for (rec=fh->fh_recs_per_blk; rec >= 1; rec--)
                {       seq = (SEQ_REC) rbuf;

                        if (seq->seq_stat == DB_INUSE)
                        {       buf->buf_rec_inx = rec;
                                memcpy(user_data, rbuf+fh->fh_ctl_size,
                                                              fh->fh_data_size);
                                return;
                        }
			rbuf -= fh->fh_rec_size;
                }
        }
        db_error= DB_END_OF_FILE;
}

/*
 *	db_read_prev_seq  -  Read Prev Record in a Sequential File
 */

void db_read_prev_seq(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR fh;
        BUFFER  buf;
        SEQ_REC seq;
        ulong   blk;
        ushort  rec;
        char  *rbuf;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        db_get_blk(df, df->df_prev_blk, buf);
        if (db_error) return;

	buf->buf_rec_inx = df->df_prev_rec - 1;

        rbuf = buf->buf_data + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

	for (rec=buf->buf_rec_inx; rec >= 1; rec--)
        {       seq = (SEQ_REC) rbuf;

                if (seq->seq_stat == DB_INUSE)
                {       buf->buf_rec_inx = rec;
                        memcpy(user_data, rbuf+fh->fh_ctl_size, fh->fh_data_size);
                        return;
                }
		rbuf -= fh->fh_rec_size;
        }

	for (blk=buf->buf_cur_blk-1; blk >= 1; blk--)
        {       db_get_blk(df, blk, buf);
                if (db_error) return;

		rbuf = buf->buf_data + (fh->fh_recs_per_blk - 1) * fh->fh_rec_size;

		for (rec=fh->fh_recs_per_blk; rec >= 1; rec--)
                {       seq = (SEQ_REC) rbuf;

                        if (seq->seq_stat == DB_INUSE)
                        {       buf->buf_rec_inx = rec;
                                memcpy(user_data, rbuf+fh->fh_ctl_size,
                                                              fh->fh_data_size);
                                return;
                        }
			rbuf -= fh->fh_rec_size;
                }
        }
        db_error= DB_END_OF_FILE;
}

/*
 *      db_update_seq  -  Update Record in a Sequential File
 */

void db_update_seq(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR  fh;
        BUFFER   buf;
        SEQ_REC  seq;
        char   *rbuf;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        if (buf->buf_cur_blk == 0 || buf->buf_rec_inx == 0)
        {       db_error = DB_NO_CURRENT_REC;
                return;
        }

        rbuf = buf->buf_data + (buf->buf_rec_inx - 1) * fh->fh_rec_size;

        seq = (SEQ_REC) rbuf;

        if (seq->seq_stat != DB_INUSE)
        {       db_error = DB_DELETED_REC;
                return;
        }

        memcpy(rbuf+fh->fh_ctl_size, user_data, fh->fh_data_size);

        db_put_blk(df, buf);
}                           
