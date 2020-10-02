/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      db_main.c  v1.3  (c) 1987-1990  Ken Harris                          */
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

static char  *copyright = "db V1.3 (c) 1987-1990 Ken Harris";

int    db_error     = 0;
ulong  db_match_blk = 0;
ushort db_match_rec = 0;
ulong  db_add_blk   = 0;
ushort db_add_rec   = 0;

/*
 *      db_create  -  Create a New Data File 
 */

DATA_FILE db_create(path, fname, options)
 char *path, *fname, *options;               
{
        BUFFER db_alloc_buf();
        DATA_FILE df;
	FILE_HDR  fhdr;
        char  *fname_dflts(), *calloc();

        db_error = 0;

        df = (DATA_FILE) calloc(1, sizeof(struct db_data_file));
	if (!df)
	{	db_error = DB_ALLOC_ERROR;
		return(NULL);
	}

        df->df_fhdr = db_alloc_buf(DB_FILE_HDR_SIZE);
        fhdr        = (FILE_HDR) df->df_fhdr->buf_data;
	if (db_error) { db_free_df(df); return(NULL); }

        df->df_fhdr->buf_cur_size = DB_FILE_HDR_SIZE;

        strcpy(df->df_fname, fname_dflts(fname,path));

	db_file_options(fhdr, options);
        if (db_error) { db_free_df(df); return(NULL); }

        db_fhdr_create(fhdr);
        if (db_error) { db_free_df(df); return(NULL); }

#ifdef MSC
	df->df_fd = open(df->df_fname, O_CREAT|O_TRUNC|O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
#endif
#ifdef TURBO 
	df->df_fd = open(df->df_fname, O_CREAT|O_TRUNC|O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
#endif
#ifdef ULTRIX
	df->df_fd = open(df->df_fname, O_CREAT|O_TRUNC|O_RDWR, S_IREAD|S_IWRITE);
#endif
#ifdef SYSV
	df->df_fd = open(df->df_fname, O_CREAT|O_TRUNC|O_RDWR, S_IREAD|S_IWRITE);
#endif
        if (df->df_fd < 0)
        {       db_error = DB_FILE_NOT_CREATED;
                db_free_df(df);
                return(NULL);
        }                       

        df->df_stat = DB_OPEN;

        switch (fhdr->fh_file_type)
        { 
#ifdef IDX
	 case DB_INDEX:
             df->df_buf = db_alloc_buf(fhdr->fh_block_size+2*fhdr->fh_rec_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

             df->df_tmp = db_alloc_buf(fhdr->fh_block_size+2*fhdr->fh_rec_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

             df->df_aux = db_alloc_buf(fhdr->fh_block_size+2*fhdr->fh_rec_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

             df->df_buf->buf_size = fhdr->fh_block_size;
             df->df_tmp->buf_size = fhdr->fh_block_size;
             df->df_aux->buf_size = fhdr->fh_block_size;
	     break;
#endif

#ifdef RAN
	  case DB_RANDOM:
             df->df_buf = db_alloc_buf(fhdr->fh_block_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

             df->df_tmp = db_alloc_buf(fhdr->fh_block_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

             while (fhdr->fh_last_block < fhdr->fh_base_size)
             {  db_extend(df, df->df_buf);
	        if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

                db_put_blk(df, df->df_buf);
             }
	     break;
#endif
         
#ifdef SEQ
	  case DB_SEQ:
             df->df_buf = db_alloc_buf(fhdr->fh_block_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }
	     break;
#endif

#ifdef VAR
	  case DB_VAR:
             df->df_buf = db_alloc_buf(fhdr->fh_block_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }
	     break;
#endif
	}

        db_put_blk(df, df->df_fhdr);
     	if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

        return(df);
}

/*
 *	db_file_options - parse and setup file options
 */

#define OPTION_SIZE     20
#define SUB_OPTION_SIZE 50

db_file_options(fh, option_str)
 FILE_HDR  fh;
 char     *option_str;
{
	char  option[OPTION_SIZE], sub_option[SUB_OPTION_SIZE];
	char *c, *o_ptr, *s_ptr;
	int   cnt;

	c = option_str;
	while (*c)
	{	while (*c && !isalpha(*c)) c++;
		
		o_ptr  = option;
		s_ptr  = sub_option;
		*o_ptr = *s_ptr = NULL;

		for (cnt=0; isalpha(*c); cnt++, c++)
		{	if (cnt < OPTION_SIZE)
			{	if (islower(*c))
					*o_ptr++ = toupper(*c);
				else
					*o_ptr++ = *c;
			}
		}

		while (*c && *c != '=' && *c != ',') c++;

		if (*c == '=')
		{	c++;
			while (*c && *c <= ' ') c++;

			for (cnt=0; *c && *c!=',' && !isspace(*c); cnt++, c++)
			{	if (cnt < SUB_OPTION_SIZE)
					*s_ptr++ = *c;
			}

			while (*c && *c != ',') c++;
		}

		*o_ptr++ = NULL;
		*s_ptr++ = NULL;

		db_file_1option(fh, option, sub_option);
		if (db_error) return;
	}
}

/*
 *	db_file_1option - Process one file option
 */

db_file_1option(fh, option, sub_option)
 FILE_HDR  fh;
 char     *option, *sub_option;
{
	if (strcmp(option, "SEQ")==0)
	{	if (fh->fh_file_type == 0)
			fh->fh_file_type = DB_SEQ;
		else
			db_error = DB_FTYPE_ERROR;
	}
	else
	if (strcmp(option, "RAN")==0)
	{	if (fh->fh_file_type == 0)
			fh->fh_file_type = DB_RANDOM;
		else
			db_error = DB_FTYPE_ERROR;
	}
	else
	if (strcmp(option, "IDX")==0)
	{	if (fh->fh_file_type == 0)
			fh->fh_file_type = DB_INDEX;
		else
			db_error = DB_FTYPE_ERROR;
	}
	else
	if (strcmp(option, "VAR")==0)
	{	if (fh->fh_file_type == 0)
			fh->fh_file_type = DB_VAR;
		else
			db_error = DB_FTYPE_ERROR;
	}
	else
	if (strcmp(option, "BLK")==0)
		fh->fh_block_size = atoi(sub_option);
	else
	if (strcmp(option, "REC")==0)
		fh->fh_data_size = atoi(sub_option);
	else
	if (strcmp(option, "BASE")==0)
		fh->fh_base_size = atoi(sub_option);
	else
	if (strcmp(option, "KEY")==0)
		fh->fh_key_size = atoi(sub_option);
	else
	if (strcmp(option, "ATR")==0)
		fh->fh_atr_size = atoi(sub_option);
	else
	if (strcmp(option, "MLINKS")==0)
		fh->fh_mlink_cnt = atoi(sub_option);
	else
	if (strcmp(option, "OLINKS")==0)
		fh->fh_olink_cnt = atoi(sub_option);
	else
	if (strcmp(option, "KLINKS")==0)
		fh->fh_klink_cnt = atoi(sub_option);
	else
	if (strcmp(option, "DUPS")==0)
		fh->fh_file_stat &= ~DB_DUP_ALLOWED;
	else
	if (strcmp(option, "NODUPS")==0)
		fh->fh_file_stat |= DB_DUP_ALLOWED;
	else
		db_error = DB_INVALID_OPTION;
}

/*
 *      db_fhdr_create - Check file header data on create
 */

void db_fhdr_create(fh)
 FILE_HDR fh;
{       short hdr_size;

        db_error = 0;

        fh->fh_db_version = DB_VERSION;

        if (fh->fh_file_type != DB_SEQ    &&
            fh->fh_file_type != DB_RANDOM &&
            fh->fh_file_type != DB_INDEX  &&
            fh->fh_file_type != DB_VAR)
        {       db_error = DB_INVALID_FHDR;
                return;
        }

#ifndef SEQ
        if (fh->fh_file_type == DB_SEQ)
        {       db_error = DB_UNSUPP_FEATURE;
                return;
        }
#endif

#ifndef RAN
        if (fh->fh_file_type == DB_RANDOM)
        {       db_error = DB_UNSUPP_FEATURE;
                return;
        }
#endif

#ifndef IDX
        if (fh->fh_file_type == DB_INDEX)
        {       db_error = DB_UNSUPP_FEATURE;
                return;
        }
#endif

#ifndef VAR
        if (fh->fh_file_type == DB_VAR)
        {       db_error = DB_UNSUPP_FEATURE;
                return;
        }
#endif

	fh->fh_file_stat &= ~DB_OPEN;
        fh->fh_last_block = 0;

        if (!fh->fh_block_size) fh->fh_block_size = 512;

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
                     hdr_size = 0;
                     if (!fh->fh_ctl_size) fh->fh_ctl_size = 1;
		     if (fh->fh_ctl_size & 1) fh->fh_ctl_size++;
                     break;
#endif

#ifdef RAN
                case DB_RANDOM:
                     hdr_size = sizeof(struct db_random_uhdr);
                     if (!fh->fh_base_size)
                     {       db_error = DB_INVALID_FHDR;
                             return;
                     }
                     if (!fh->fh_ctl_size) fh->fh_ctl_size = 1;
                     break;
#endif

#ifdef IDX
                case DB_INDEX:
                     if (!fh->fh_ctl_size) fh->fh_ctl_size = 2;
                     hdr_size = sizeof(struct db_index_hdr) + fh->fh_ctl_size;
                     break;
#endif

#ifdef VAR
                case DB_VAR:
                     hdr_size = 0;
                     if (!fh->fh_ctl_size) fh->fh_ctl_size = sizeof(struct db_var_rec);
		     if (!fh->fh_data_size) fh->fh_data_size = 512;
		     fh->fh_block_size = fh->fh_ctl_size + fh->fh_data_size;
		     fh->fh_vfile_size = DB_FILE_HDR_SIZE;
                     break;
#endif

                default:
                     hdr_size = 0;
                     break;
        }

	fh->fh_ctl_size     = fh->fh_ctl_size 
			    + fh->fh_olink_cnt * sizeof(struct db_owner_link)
			    + fh->fh_mlink_cnt * sizeof(struct db_member_link)
			    + fh->fh_klink_cnt * sizeof(struct db_key_link);

        fh->fh_rec_size     = fh->fh_ctl_size + fh->fh_data_size;

	if (fh->fh_file_type == DB_VAR)
		fh->fh_block_size = fh->fh_rec_size;
	else
	if (fh->fh_file_type == DB_INDEX)
	{	if (fh->fh_block_size < 2 * fh->fh_rec_size + hdr_size)
			fh->fh_block_size = 2 * fh->fh_rec_size + hdr_size;

		fh->fh_block_size = ((fh->fh_block_size + 511) / 512) * 512;
	}
	else
	{	if (fh->fh_block_size < fh->fh_rec_size + hdr_size)
			fh->fh_block_size = fh->fh_rec_size + hdr_size;

		fh->fh_block_size = ((fh->fh_block_size + 511) / 512) * 512;
	}

        fh->fh_rec_cnt      = 0;
        fh->fh_recs_per_blk = (fh->fh_block_size - hdr_size) / fh->fh_rec_size;
        fh->fh_root_ptr     = 0;
        fh->fh_next_avail   = 0;

        if (fh->fh_key_size > fh->fh_data_size)
        {       db_error = DB_INVALID_FHDR;
                return;
        }
}

/*
 *      db_open  -  Open a Data File
 */

DATA_FILE db_open(path,fname)
 char *path, *fname;
{
        BUFFER   db_alloc_buf();
        DATA_FILE df;
        FILE_HDR fhdr;
        char    *fname_dflts(), *calloc();
        int      cnt;

        db_error = 0;
        df = (DATA_FILE) calloc(1, sizeof(struct db_data_file));
	if (!df)
	{	db_error = DB_ALLOC_ERROR;
		return(NULL);
	}

        df->df_fhdr = db_alloc_buf(DB_FILE_HDR_SIZE);
        fhdr        = (FILE_HDR) df->df_fhdr->buf_data;
        if (db_error) { db_free_df(df); return(NULL); }
            
        strcpy(df->df_fname, fname_dflts(fname,path));

#ifdef MSC
        df->df_fd = open(df->df_fname, O_RDWR|O_BINARY);
#endif
#ifdef TURBO
        df->df_fd = open(df->df_fname, O_RDWR|O_BINARY);
#endif
#ifdef ULTRIX
	df->df_fd = open(df->df_fname, O_RDWR);
#endif
#ifdef SYSV
	df->df_fd = open(df->df_fname, O_RDWR);
#endif
        if (df->df_fd < 0)
        {       db_error = DB_FILE_NOT_FOUND;
                db_free_df(df);
                return(NULL);
        }
                
        df->df_stat = DB_OPEN;

        db_get_blk(df, 0L, df->df_fhdr);
        if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }
        
        switch (fhdr->fh_file_type)
        { 
#ifdef IDX
	  case DB_INDEX:
             df->df_buf = db_alloc_buf(fhdr->fh_block_size+2*fhdr->fh_rec_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

             df->df_tmp = db_alloc_buf(fhdr->fh_block_size+2*fhdr->fh_rec_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

             df->df_aux = db_alloc_buf(fhdr->fh_block_size+2*fhdr->fh_rec_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

             df->df_buf->buf_size = fhdr->fh_block_size;
             df->df_tmp->buf_size = fhdr->fh_block_size;
             df->df_aux->buf_size = fhdr->fh_block_size;
	     break;
#endif

#ifdef RAN
          case DB_RANDOM:
             df->df_buf = db_alloc_buf(fhdr->fh_block_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

             df->df_tmp = db_alloc_buf(fhdr->fh_block_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }
	     break;
#endif

#ifdef SEQ
          case DB_SEQ:
             df->df_buf = db_alloc_buf(fhdr->fh_block_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }
	     break;
#endif

#ifdef VAR
	  case DB_VAR:
             df->df_buf = db_alloc_buf(fhdr->fh_block_size);
	     if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }
	     break;
#endif
	}

        db_fhdr_open(fhdr);
     	if (db_error) { close(df->df_fd); db_free_df(df); return(NULL); }

        return(df);
}

/*
 *      db_fhdr_open - Check file header data on open
 */

void db_fhdr_open(fh)
 FILE_HDR fh;
{
        db_error = 0;

	if (fh->fh_db_version == 12)
		fh->fh_db_version = 13;
		/* Upgrade shouldn't change data (we hope) */

        if (fh->fh_db_version != DB_VERSION)
        {       db_error = DB_VERSION_ERROR;
                return;
        }

        if (fh->fh_file_type != DB_SEQ    &&
            fh->fh_file_type != DB_RANDOM &&
            fh->fh_file_type != DB_INDEX  &&
            fh->fh_file_type != DB_VAR)
        {       db_error = DB_INVALID_FHDR;
                return;
        }

#ifndef SEQ
        if (fh->fh_file_type == DB_SEQ)
        {       db_error = DB_UNSUPP_FEATURE;
                return;
        }
#endif

#ifndef RAN
        if (fh->fh_file_type == DB_RANDOM)
        {       db_error = DB_UNSUPP_FEATURE;
                return;
        }
#endif

#ifndef IDX
        if (fh->fh_file_type == DB_INDEX)
        {       db_error = DB_UNSUPP_FEATURE;
                return;
        }
#endif

#ifndef VAR
        if (fh->fh_file_type == DB_VAR)
        {       db_error = DB_UNSUPP_FEATURE;
                return;
        }
#endif

        if (fh->fh_block_size   == 0 ||
            fh->fh_rec_size     == 0 ||
            fh->fh_recs_per_blk == 0)
        {       db_error = DB_INVALID_FHDR;
                return;
        }

        if (fh->fh_rec_size != fh->fh_ctl_size + fh->fh_data_size)
        {       db_error = DB_INVALID_FHDR;
                return;
        }

        if (fh->fh_key_size > fh->fh_data_size)
        {       db_error = DB_INVALID_FHDR;
                return;
        }

        if (fh->fh_file_type == DB_RANDOM &&
            fh->fh_base_size == 0)
        {       db_error = DB_INVALID_FHDR;
                return;
        }
}

/*
 *      db_close  -  close a data set
 */

DATA_FILE db_close(df)
 DATA_FILE df;
{
        db_error = 0;

        db_check_df(df);
	if (db_error) return(NULL);

        db_put_blk(df, df->df_fhdr);
	if (db_error) return(NULL);

        close(df->df_fd);

        db_free_df(df);

        return(NULL);
}

/*
 *      db_add  -  Add a new record to a data set
 */

ulong db_add(df, user_data, data_size)
 DATA_FILE df;
 char *user_data;
 int data_size;
{
        FILE_HDR fh;
        BUFFER  buf;
        ulong   rec_no;

        db_error = 0;

        db_check_df(df);
        if (db_error) return(0);

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = (BUFFER) df->df_buf;

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
                        db_add_seq(df, user_data);
                        break;
#endif

#ifdef IDX
                case DB_INDEX:
                        db_add_idx(df, user_data);
                        break;
#endif

#ifdef RAN
                case DB_RANDOM:
                        db_add_ran(df, user_data);
                        break;
#endif

#ifdef VAR
		case DB_VAR:
			db_add_var(df, user_data, data_size);
			break;
#endif

                default:
                        db_error = DB_INVALID_REQUEST;
                        break;
        }

        if (db_error) return(0);

	if (fh->fh_file_type == DB_VAR)
		rec_no = db_add_blk;
	else
	        rec_no = (db_add_blk - 1) * fh->fh_recs_per_blk + db_add_rec;

        buf->buf_cur_blk = 0;
        buf->buf_rec_inx = 0;

        return(rec_no);
}

/*
 *      db_read_first  -  Read First Record in a data set
 */

void db_read_first(df, user_data, data_size)
 DATA_FILE df;
 char *user_data;
 int  *data_size;
{
        FILE_HDR  fh;
        BUFFER   buf;

        db_error = 0;

        db_check_df(df);
        if (db_error) return;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = (BUFFER) df->df_buf;

        if (!fh->fh_rec_cnt)
        {       db_error = DB_END_OF_FILE;
                return;
        }

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
                        db_read_first_seq(df, user_data);
                        break;
#endif

#ifdef IDX
                case DB_INDEX:
                        db_read_first_idx(df, fh->fh_root_ptr, user_data);
                        break;
#endif

#ifdef RAN
                case DB_RANDOM:
                        db_read_first_ran(df, user_data);
                        break;
#endif

#ifdef VAR
		case DB_VAR:
			db_read_first_var(df, user_data, data_size);
			break;
#endif

                default:
                        db_error = DB_INVALID_REQUEST;
                        break;
        }

        if (db_error)
        {       buf->buf_cur_blk  = 0;
                buf->buf_rec_inx  = 0;
		buf->buf_cur_size = 0;
        }

        df->df_prev_blk   = buf->buf_cur_blk;
        df->df_prev_rec   = buf->buf_rec_inx;
	df->df_prev_vsize = buf->buf_cur_size;

	return;
}

/*
 *      db_read_next  -  Read next record from a data set
 */

void db_read_next(df, user_data, data_size)
 DATA_FILE df;
 char *user_data;
 int  *data_size;
{
        FILE_HDR  fh;
        BUFFER   buf;

        db_error = 0;

        db_check_df(df);
        if (db_error) return;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = (BUFFER) df->df_buf;

        if (!fh->fh_rec_cnt)
        {       db_error = DB_END_OF_FILE;
                return;
        }

        if (!df->df_prev_blk)
        {       db_error = DB_NO_CURRENT_REC;
                return;
        }

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
                        db_read_next_seq(df, user_data);
                        break;
#endif

#ifdef IDX
                case DB_INDEX:
                        db_read_next_idx(df, user_data);
                        break;
#endif
                              
#ifdef RAN
                case DB_RANDOM:
                        db_read_next_ran(df, user_data);
                        break;
#endif

#ifdef VAR
		case DB_VAR:
			db_read_next_var(df, user_data, data_size);
			break;
#endif

                default:
                        db_error = DB_INVALID_REQUEST;
                        break;
        }

        if (db_error)
        {       buf->buf_cur_blk  = 0;
                buf->buf_rec_inx  = 0;
		buf->buf_cur_size = 0;
        }

        df->df_prev_blk   = buf->buf_cur_blk;
        df->df_prev_rec   = buf->buf_rec_inx;
	df->df_prev_vsize = buf->buf_cur_size;

	return;
}
 
/*
 *	db_read_last  -  Read Last Record in a data set
 */

void db_read_last(df, user_data, data_size)
 DATA_FILE df;
 char *user_data;
 int  *data_size;
{
        FILE_HDR  fh;
        BUFFER   buf;

        db_error = 0;

        db_check_df(df);
        if (db_error) return;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = (BUFFER) df->df_buf;

        if (!fh->fh_rec_cnt)
        {       db_error = DB_END_OF_FILE;
                return;
        }

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
			db_read_last_seq(df, user_data);
                        break;
#endif

#ifdef IDX
                case DB_INDEX:
			db_read_last_idx(df, fh->fh_root_ptr, user_data);
                        break;
#endif

#ifdef RAN
                case DB_RANDOM:
			db_read_last_ran(df, user_data);
                        break;
#endif

#ifdef VAR
                case DB_VAR:
			db_read_last_var(df, user_data, data_size);
                        break;
#endif

                default:
                        db_error = DB_INVALID_REQUEST;
                        break;
        }

        if (db_error)
        {       buf->buf_cur_blk  = 0;
                buf->buf_rec_inx  = 0;
		buf->buf_cur_size = 0;
        }

        df->df_prev_blk   = buf->buf_cur_blk;
        df->df_prev_rec   = buf->buf_rec_inx;
	df->df_prev_vsize = buf->buf_cur_size;

	return;
}

/*
 *	db_read_prev  -  Read prev record from a data set
 */

void db_read_prev(df, user_data, data_size)
 DATA_FILE df;
 char *user_data;
 int  *data_size;
{
        FILE_HDR  fh;
        BUFFER   buf;

        db_error = 0;

        db_check_df(df);
        if (db_error) return;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = (BUFFER) df->df_buf;

        if (!fh->fh_rec_cnt)
        {       db_error = DB_END_OF_FILE;
                return;
        }

        if (!df->df_prev_blk)
        {       db_error = DB_NO_CURRENT_REC;
                return;
        }

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
			db_read_prev_seq(df, user_data);
                        break;
#endif

#ifdef IDX
                case DB_INDEX:
			db_read_prev_idx(df, user_data);
                        break;
#endif
                              
#ifdef RAN
                case DB_RANDOM:
			db_read_prev_ran(df, user_data);
                        break;
#endif

#ifdef VAR
                case DB_VAR:
			db_read_prev_var(df, user_data, data_size);
                        break;
#endif

                default:
                        db_error = DB_INVALID_REQUEST;
                        break;
        }

        if (db_error)
        {       buf->buf_cur_blk  = 0;
                buf->buf_rec_inx  = 0;
		buf->buf_cur_size = 0;
        }

        df->df_prev_blk   = buf->buf_cur_blk;
        df->df_prev_rec   = buf->buf_rec_inx;
	df->df_prev_vsize = buf->buf_cur_size;

	return;
}

/*
 *      db_find  -  Find a Record
 */

void db_find(df, user_data, key, key_size)
 DATA_FILE df;
 char *user_data;
 char *key;
 int   key_size;
{
        FILE_HDR  fh;
        BUFFER   buf;

        db_error = 0;

        db_check_df(df);
        if (db_error) return;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = (BUFFER) df->df_buf;

        if (!fh->fh_rec_cnt)
        {       db_error = DB_REC_NOT_FOUND;
                return;
        }

        if (!key_size) key_size = fh->fh_key_size;

        switch (fh->fh_file_type)
        {
#ifdef IDX
                case DB_INDEX:
                        db_find_first_idx(df, user_data, key, key_size);
                        break;
#endif

#ifdef RAN
                case DB_RANDOM:
                        db_find_ran(df, user_data, key);
                        break;
#endif

                default:
                        db_error = DB_INVALID_REQUEST;
                        break;
        }

        if (db_error)
        {       buf->buf_cur_blk = 0;
                buf->buf_rec_inx = 0;
        }

        df->df_prev_blk = buf->buf_cur_blk;
        df->df_prev_rec = buf->buf_rec_inx;

	return;
}

/*
 *      db_update  -  Update a record from a data set
 */

void db_update(df, user_data)
 DATA_FILE df;
 char *user_data;
{
        FILE_HDR  fh;
        BUFFER   buf;

        db_error = 0;

        db_check_df(df);
        if (db_error) return;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = (BUFFER) df->df_buf;

        if (!buf->buf_cur_blk)
        {       db_error = DB_NO_CURRENT_REC;
                return;
        }

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
                        db_update_seq(df, user_data);
                        break;
#endif

#ifdef RAN
                case DB_RANDOM:
                        db_update_ran(df, user_data);
                        break;
#endif

#ifdef IDX
                case DB_INDEX:
                        db_update_idx(df, user_data);
                        break;
#endif

#ifdef VAR
                case DB_VAR:
                        db_update_var(df, user_data);
                        break;
#endif

                default:
                        db_error = DB_INVALID_REQUEST;
                        break;
        }

        buf->buf_cur_blk = 0;
        buf->buf_rec_inx = 0;

	return;
}

/*
 *      db_delete  -  Delete a record from a data set
 */

void db_delete(df)
 DATA_FILE df;
{
        FILE_HDR  fh;
        BUFFER   buf;

        db_error = 0;

        db_check_df(df);
        if (db_error) return;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = (BUFFER) df->df_buf;

        if (!buf->buf_cur_blk)
        {       db_error = DB_NO_CURRENT_REC;
                return;
        }

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
                        db_free_rec(df, buf);
                        if (db_error) return;

                        if (fh->fh_rec_cnt > 0) fh->fh_rec_cnt--;
                        db_put_blk(df, df->df_fhdr);
                        break;
#endif

#ifdef IDX
                case DB_INDEX:
                        db_delete_idx(df);
                        break;
#endif

#ifdef RAN
                case DB_RANDOM:
                        db_delete_ran(df);
                        break;
#endif

#ifdef VAR
		case DB_VAR:
			db_delete_var(df);
			break;
#endif

                default:
                        db_error = DB_INVALID_REQUEST;
                        break;
        }

        buf->buf_cur_blk = 0;
        buf->buf_rec_inx = 0;

	return;
}

/*
 *      db_get_rec_no  -  Get relative record number
 */

ulong db_get_rec_no(df)
 DATA_FILE df;
{
        FILE_HDR  fh;
        BUFFER   buf;

        db_error = 0;

        db_check_df(df);
        if (db_error) return(0);

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        if (!buf->buf_cur_blk)
        {       db_error = DB_NO_CURRENT_REC;
                return(0);
        }
 
	if (fh->fh_file_type == DB_VAR)
		return(buf->buf_cur_blk + buf->buf_rec_inx);
	else
	        return((long)(buf->buf_cur_blk - 1) * fh->fh_recs_per_blk
                                                    + buf->buf_rec_inx);
}

/*
 *      db_read_direct  -  Read a record by number
 */                                               

void db_read_direct(df, rec_no, user_data, data_size)
 DATA_FILE     df;
 ulong    rec_no;
 char  *user_data;
 int   *data_size;
{
        FILE_HDR    fh;
        BUFFER      buf;
        INDEX_HDR   ihdr;
        RANDOM_HDR  rhdr;
        RANDOM_REC  rrec;
        SEQ_REC     seq;
	VAR_REC     vrec;
        ulong       blk;
        ushort      rec;
        char       *rbuf;

        db_error = 0;
                                       
        db_check_df(df);
	if (db_error) return;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = df->df_buf;

        buf->buf_cur_blk = 0;
        buf->buf_rec_inx = 0;

	if (fh->fh_file_type == DB_VAR)
	{	rec = 0;
		blk = rec_no;
	}
	else
        {	blk = rec_no / fh->fh_recs_per_blk;
	        rec = rec_no % fh->fh_recs_per_blk;

       		if (rec == 0)
                	rec = fh->fh_recs_per_blk;
	        else
       		        blk++;
	}

        db_get_blk(df, blk, buf);
	if (db_error) return;

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
                        rbuf = buf->buf_data + (rec-1) * fh->fh_rec_size;
                        seq  = (SEQ_REC) rbuf;
                        
                        if (seq->seq_stat != DB_INUSE)
                        {       db_error = DB_DELETED_REC;
				return;
                        }
                        break;
#endif

#ifdef IDX
                case DB_INDEX:
                        ihdr = (INDEX_HDR) buf->buf_data;
                        
                        if (ihdr->idx_stat != DB_INUSE)
                        {       db_error = DB_DELETED_REC;
				return;
                        }

                        if (rec > ihdr->idx_rec_cnt)
                        {       db_error = DB_DELETED_REC;
				return;
                        }

                        rbuf = buf->buf_data + sizeof(struct db_index_hdr)
                                                + (rec - 1) * fh->fh_rec_size;
                        break;
#endif

#ifdef RAN
                case DB_RANDOM:
                        rhdr = (RANDOM_HDR) buf->buf_data;
                        
                        if (rhdr->ran_stat != DB_INUSE)
                        {       db_error = DB_DELETED_REC;
				return;
                        }

                        if (rec > rhdr->ran_rec_cnt)
                        {       db_error = DB_DELETED_REC;
				return;
                        }

                        rbuf = buf->buf_data + sizeof(struct db_random_uhdr)
                                                + (rec - 1) * fh->fh_rec_size;

                        rrec = (RANDOM_REC) rbuf;
                        if (rrec->ran_stat != DB_INUSE)
                        {       db_error = DB_DELETED_REC;
				return;
                        }
                        break;
#endif

#ifdef VAR
		case DB_VAR:
			db_var_chk_csum(buf);
			if (db_error) return;

			vrec = (VAR_REC)(buf->buf_data + buf->buf_rec_inx);

			if (vrec->var_stat != DB_INUSE)
                        {       db_error = DB_DELETED_REC;
				return;
                        }
			*data_size = vrec->var_dsize;
			buf->buf_cur_size = vrec->var_dsize
				          + sizeof(struct db_var_rec);
			break;
#endif

                default:
                        db_error = DB_INVALID_REQUEST;
			return;
        }
	if (fh->fh_file_type == DB_VAR)
		memcpy(user_data, vrec->var_data, vrec->var_dsize);
	else
	        memcpy(user_data, rbuf + fh->fh_ctl_size, fh->fh_data_size);

        df->df_prev_blk   = buf->buf_cur_blk = blk;
        df->df_prev_rec   = buf->buf_rec_inx = rec;
	df->df_prev_vsize = buf->buf_cur_size;

	return;
}

/*
 *      db_reorg  -  Reorganize a File
 */

void db_reorg(df)
 DATA_FILE df;
{
        FILE_HDR  fh;
        BUFFER   buf;

        db_error = 0;

        db_check_df(df);
        if (db_error) return;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;
        buf = (BUFFER) df->df_buf;

        switch (fh->fh_file_type)
        {
#ifdef VAR
	 	case DB_VAR:
			db_reorg_var(df);
			break;
#endif

                default:
                        db_error = DB_INVALID_REQUEST;
                        break;
        }

        buf->buf_cur_blk = 0;
        buf->buf_rec_inx = 0;

	return;
}

/*
 *      db_read_atr  - Read Data File Attribute Block
 */

void db_read_atr(df, user_data)
 DATA_FILE df;
 char  *user_data;
{        
        FILE_HDR fh;
	char    *atr_data;


        db_error = 0;

        db_check_df(df);
        if (db_error) return;

        fh = (FILE_HDR) df->df_fhdr->buf_data;

	atr_data = ((char *)fh) + DB_ATR_OFFSET;
        memcpy(user_data, atr_data, fh->fh_atr_size);

	return;
}

/*
 *      db_update_atr  - Update Data File Attribute Block
 */

void db_update_atr(df, user_data)
 DATA_FILE df;
 char  *user_data;
{
        FILE_HDR fh;
	char    *atr_data;

        db_error = 0;

        db_check_df(df);
        if (db_error) return;

        fh = (FILE_HDR) df->df_fhdr->buf_data;

	atr_data = ((char *)fh) + DB_ATR_OFFSET;
        memcpy(atr_data, user_data, fh->fh_atr_size);

        db_put_blk(df, df->df_fhdr);

	return;
}

/*
 *      db_check_df  -  Common checks on Data File Validity
 */                                                       

void db_check_df(df)
 DATA_FILE df;
{
        FILE_HDR fh;

        db_error = 0;

        if (!df)
        {       db_error = DB_FILE_NOT_OPEN;
                return;
        }

        if (!df->df_stat & DB_OPEN)
        {       db_error = DB_FILE_NOT_OPEN;
                return;
        }

        if (!df->df_fhdr || !df->df_buf)
        {       db_error = DB_BUFFER_ERROR;
                return;
        }

        fh = (FILE_HDR) df->df_fhdr->buf_data;

        if (fh->fh_file_type == DB_INDEX)
                if (!df->df_tmp || !df->df_aux)
                {       db_error = DB_BUFFER_ERROR;
                        return;
                }
}

/*
 *      db_extend  -  Extend a data set by 1 Block
 */

void db_extend(df, buf)
 DATA_FILE df;
 BUFFER   buf;
{
        FILE_HDR  fhdr;
        long       rec;
        char        *c;
        FREE_REC   fre;
        RANDOM_HDR ran;
        SEQ_REC    seq;
        int        cnt;


        fhdr                = (FILE_HDR) df->df_fhdr->buf_data;
        buf                 = df->df_buf;
        fhdr->fh_last_block = fhdr->fh_last_block + 1;
        buf->buf_cur_blk    = fhdr->fh_last_block;
        buf->buf_cur_size   = fhdr->fh_block_size;

        memset(buf->buf_data, 0, buf->buf_cur_size);

        switch(fhdr->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:     
                        rec = buf->buf_cur_blk * fhdr->fh_recs_per_blk;
                        c   = buf->buf_data + (fhdr->fh_recs_per_blk - 1)
                                                        * fhdr->fh_rec_size;

                        for (cnt=0; cnt < fhdr->fh_recs_per_blk; cnt++)
                        {       fre                 = (FREE_REC) c;
                                fre->fre_stat       = DB_FREE;
                                fre->fre_next       = fhdr->fh_next_avail;
                                fhdr->fh_next_avail = rec--;
                                c                  -= fhdr->fh_rec_size;
                        }
                        break;
#endif

#ifdef RAN
                case DB_RANDOM:
                        c = buf->buf_data + sizeof(struct db_random_uhdr);
                        for (cnt=0; cnt < fhdr->fh_recs_per_blk; cnt++)
                        {       *c  = DB_FREE;
                                 c += fhdr->fh_rec_size;
                        }
                        if (buf->buf_cur_blk <= fhdr->fh_base_size)
                        {       ran = (RANDOM_HDR) buf->buf_data;
                                ran->ran_stat    = DB_INUSE;
                                ran->ran_next    = 0;
                                ran->ran_rec_cnt = 0;
                        }
                        else
                        {       fre = (FREE_REC) buf->buf_data;
                                fre->fre_stat = DB_FREE;
                                fre->fre_next = fhdr->fh_next_avail;
                                fhdr->fh_next_avail = buf->buf_cur_blk;
                        }
                        break;
#endif

#ifdef IDX
                case DB_INDEX:
                        fre = (FREE_REC) buf->buf_data;
                        fre->fre_stat = DB_FREE;               
                        fre->fre_next = fhdr->fh_next_avail;
                        fhdr->fh_next_avail = buf->buf_cur_blk;
                        break;
#endif
        }

        db_put_blk(df,df->df_buf);
        db_put_blk(df,df->df_fhdr);
}

/*
 *      db_get_next_avail  -  Get Next Available Block
 */

void db_get_next_avail(df, buf)
 DATA_FILE df;
 BUFFER  buf;
{
        FILE_HDR     fh;
        FREE_REC    fre;
        RANDOM_HDR rhdr;
        INDEX_HDR  ihdr;
        ulong       blk;
        ushort      rec;
        char      *rbuf;

        db_error = 0;

        fh  = (FILE_HDR) df->df_fhdr->buf_data;

        if (!fh->fh_next_avail)
        {       db_extend(df, buf);
                if (db_error) return;
        }

        if (fh->fh_file_type == DB_SEQ)
        {       blk = fh->fh_next_avail / fh->fh_recs_per_blk;
                rec = fh->fh_next_avail % fh->fh_recs_per_blk;

                if (rec==0)
                        rec = fh->fh_recs_per_blk;
                else
                        blk++;
        }
        else
        {       blk = fh->fh_next_avail;
                rec = 1;
        }

        db_get_blk(df, blk, buf);
        if (db_error) return;

        buf->buf_rec_inx = rec;
        rbuf = buf->buf_data + (rec - 1) * fh->fh_rec_size;

        fre = (FREE_REC) rbuf;
        if (fre->fre_stat != DB_FREE)
        {       db_error = DB_INVALID_FREE;
                return;
        }

        fre->fre_stat     = DB_INUSE;
        fh->fh_next_avail = fre->fre_next;

        if (fh->fh_file_type == DB_INDEX)
                if (!fh->fh_root_ptr)
                        fh->fh_root_ptr = blk;

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
                        break;
#endif

#ifdef RAN
                case DB_RANDOM:
                        rhdr = (RANDOM_HDR) rbuf;
                        rhdr->ran_next = 0;
                        rhdr->ran_rec_cnt = 0;
                        break;
#endif

#ifdef IDX
                case DB_INDEX:
                        ihdr = (INDEX_HDR) rbuf;
                        ihdr->idx_parent = 0;
                        ihdr->idx_rec_cnt = 0;
                        break;
#endif
        }
}

/*
 *      db_free_rec  -  Free a deleted record
 */

void db_free_rec(df, buf)
 DATA_FILE df;
 BUFFER   buf;
{
        FILE_HDR  fh;
        FREE_REC fre;
	VAR_REC  var;

        db_error = 0;

        fh = (FILE_HDR) df->df_fhdr->buf_data;

        switch (fh->fh_file_type)
        {       
#ifdef SEQ
		case DB_SEQ:
                        fre = (FREE_REC) (buf->buf_data 
                                + (buf->buf_rec_inx - 1) * fh->fh_rec_size);
                        memset(fre, 0, fh->fh_rec_size);
                        fre->fre_stat     = DB_FREE;
                        fre->fre_next     = fh->fh_next_avail;
                        fh->fh_next_avail = (long) (buf->buf_cur_blk - 1)
                                  * fh->fh_recs_per_blk + buf->buf_rec_inx;
                        break;
#endif

#ifdef RAN
                case DB_RANDOM:
                        fre = (FREE_REC) buf->buf_data;
                        fre->fre_stat     = DB_FREE;
                        fre->fre_next     = fh->fh_next_avail;
                        fh->fh_next_avail = buf->buf_cur_blk;
                        break;                               
#endif

#ifdef IDX
                case DB_INDEX:
                        fre = (FREE_REC) buf->buf_data;
                        memset(fre, 0, fh->fh_block_size);
                        fre->fre_stat     = DB_FREE;
                        fre->fre_next     = fh->fh_next_avail;
                        fh->fh_next_avail = buf->buf_cur_blk;
                        break;                               
#endif

#ifdef VAR
		case DB_VAR:
			var           = (VAR_REC) buf->buf_data;
			var->var_stat = DB_FREE;
			memset(var->var_data, 0 , var->var_dsize);
			break;
#endif
        }
        db_put_blk(df, buf);
        if (db_error) return;
}

/*
 *      db_get_blk  -  Get a Block from a Data File
 */

void db_get_blk(df,blk,buf)
 DATA_FILE df;         
 long     blk;
 BUFFER   buf;
{
        FILE_HDR  fhdr;
	VAR_REC   var;
        long psn, lseek();
        short cnt;          
                                
        db_error = 0;

        if (!df)
        {       db_error = DB_FILE_NOT_OPEN;
                return;
        }

        if (!df->df_stat & DB_OPEN)
        {       db_error = DB_FILE_NOT_OPEN;
                return;
        }
        
        if (!buf)
        {       db_error = DB_BUFFER_ERROR;
                return;
        }

        fhdr = (FILE_HDR) df->df_fhdr->buf_data;
                      
        if (blk == 0)
        {       psn = 0;
		cnt = buf->buf_size;
	}
        else
	if (fhdr->fh_file_type == DB_VAR)
	{	psn  = blk;
		if (psn >= DB_FILE_HDR_SIZE)
			cnt = buf->buf_size;
		else
		{	cnt = buf->buf_size - (DB_FILE_HDR_SIZE - psn);
			psn = DB_FILE_HDR_SIZE;
			blk = DB_FILE_HDR_SIZE;
			if (cnt <= 0)
			{	db_error = DB_INVALID_BLOCK;
				return;
			}
		}
	}
	else
        {       psn = DB_FILE_HDR_SIZE + (blk-1) * (long)fhdr->fh_block_size;
		cnt = buf->buf_size;
	}

        buf->buf_cur_blk  = 0;
        buf->buf_cur_size = 0;
	buf->buf_rec_inx  = 0;

        if (lseek(df->df_fd, psn, 0) < 0)
        {       db_error = DB_INVALID_BLOCK;
                return;
        }

        cnt = read(df->df_fd, buf->buf_data, cnt);
        if (cnt < 0)
        {       db_error = DB_READ_ERROR;
                return;
        }

        if (cnt == 0)
        {       db_error = DB_END_OF_FILE;
                return;
        }            

        buf->buf_cur_blk  = blk;
        buf->buf_cur_size = cnt;
	
}

/*
 *      db_put_blk  -  Put a Block out to a Data File
 */

void db_put_blk(df,buf)
 DATA_FILE df;         
 BUFFER   buf;
{
        FILE_HDR  fhdr;
	VAR_REC   var;
        long psn, lseek();
	int  cnt;
                                
        db_error = 0;

        if (!df)
        {       db_error = DB_FILE_NOT_OPEN;
                return;
        }

        if (!df->df_stat & DB_OPEN)
        {       db_error = DB_FILE_NOT_OPEN;
                return;
        }
        
        if (!buf)
        {       db_error = DB_BUFFER_ERROR;
                return;
        }

        if (buf->buf_cur_size == 0)
        {       db_error = DB_INVALID_BLK_SIZE;
                return;
        }

        fhdr = (FILE_HDR) df->df_fhdr->buf_data;

        if (buf->buf_cur_blk == 0)
                psn = 0;
        else
	if (fhdr->fh_file_type == DB_VAR)
	{	psn  = buf->buf_cur_blk;
		if (psn < DB_FILE_HDR_SIZE)
		{	db_error = DB_INVALID_BLOCK;
			return;
		}
	}
	else
                psn  = DB_FILE_HDR_SIZE
                     + (buf->buf_cur_blk - 1) * (long)fhdr->fh_block_size;

        if (lseek(df->df_fd, psn, 0) < 0)
        {       db_error = DB_INVALID_BLOCK;
                return;
        }

        cnt = write(df->df_fd, buf->buf_data, buf->buf_cur_size);
        if (cnt != buf->buf_cur_size)
        {       db_error = DB_WRITE_ERROR;
                return;
        }
}

/*
 *      db_alloc_buf  -  Get a Buffer
 */

BUFFER db_alloc_buf(size)
 int size;
{
        BUFFER  buf;
        char   *calloc();

        buf = (BUFFER) calloc(1, sizeof(struct db_data_buf));
	if (!buf)
	{	db_error = DB_ALLOC_ERROR;
		return(NULL);
	}

        if (size)
        {       buf->buf_size  = size;
                buf->buf_data  = (char *) calloc(1, size);

		if (!buf->buf_data)
		{	db_error = DB_ALLOC_ERROR;
			free(buf);
			return(NULL);
		}
        }

        return(buf);
}



/*
 *      db_free_buf  -  Free a Buffer
 */

BUFFER db_free_buf(buf)
 BUFFER buf;
{      
        BUFFER prev,next;

	if (!buf) return(NULL);

        prev = buf->buf_prev;
        next = buf->buf_next;

        if (prev) prev->buf_next = buf->buf_next;

        if (next) next->buf_prev = buf->buf_prev;

        if (buf->buf_data) free(buf->buf_data);

        free(buf);
        
        return(next);
}

/*
 *      db_free_df  -  Free all dynamic structures for a Data File
 */

void db_free_df(df)
 DATA_FILE df;
{
        if (!df) return;

        if (df->df_fhdr)
                db_free_buf(df->df_fhdr);

        while (df->df_buf)
                df->df_buf = db_free_buf(df->df_buf);

        if (df->df_tmp)
                db_free_buf(df->df_tmp);

        if (df->df_aux)
                db_free_buf(df->df_aux);

	free(df);

	return;
}

/*
 *      db_error_msg  -  Return pointer to error message text
 */

static char *db_error_msgs[] =
{"DB-No error",		       /*  0 */
 "DB-Data File Not Found",     /*  1 */
 "DB-Read Error",              /*  2 */
 "DB-End Of File",             /*  3 */
 "DB-Write Error",             /*  4 */
 "DB-Data File Not Created",   /*  5 */
 "DB-Data File Not Open",      /*  6 */
 "DB-Invalid Block",           /*  7 */
 "DB-Buffer Error",            /*  8 */
 "DB-No Current Record",       /*  9 */
 "DB-Record Deleted",          /* 10 */
 "DB-Free List Error",         /* 11 */
 "DB-Invalid Block Size",      /* 12 */
 "DB-Index File Corrupted",    /* 13 */
 "DB-Record Not Found",        /* 14 */
 "DB-Duplicate Key",           /* 15 */
 "DB-Invalid Request",         /* 16 */
 "DB-Random File Corrupted",   /* 17 */
 "DB-Invalid File Header",     /* 18 */
 "DB-File Version Mismatch",   /* 19 */
 "DB-Invalid Link Descriptor", /* 20 */
 "DB-Linked List Corrupted",   /* 21 */
 "DB-Invalid File Type",       /* 22 */
 "DB-Invalid File Option",     /* 23 */
 "DB-Can't Allocate Memory",   /* 24 */
 "DB-Var File Rec Size Error", /* 25 */
 "DB-Var File Check Sum Error",/* 26 */
 "DB-Feature Unavailable",     /* 27 */
 "DB-Invalid Dictionary",      /* 28 */
 "DB-Invalid Name",            /* 29 */
 "DB-Invalid Error #",         /* 30 */
 ""};

char *db_error_msg(error)
 int error;
{
	if (error < 0 || error > DB_LAST_ERROR)
		error = DB_INVALID_ERROR;

        return(db_error_msgs[error]);
}

/*
 *		memcpy - copy a block of memory taking care not to overlap
 */

void *memcpy(dst, src, cnt)
 char *dst, *src;
 int   cnt;
{
	char *s, *d;

	if (dst > src  &&  dst < src+cnt)
	{	s = src + cnt - 1;
		d = dst + cnt - 1;
		while (cnt--) *d-- = *s--;
	}
	else
	{	s = src;
		d = dst;
		while (cnt--) *d++ = *s++;
	}
	return(dst);
}
				 
