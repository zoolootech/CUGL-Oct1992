/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*	db_dict.c - dictionary (symtable manager) routines                  */
/*                  v1.3  (c) 1990  Ken Harris                              */
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
static void strupr(char *);
#else
static void strupr();
#endif
char *calloc();

/*
 *	db_dict_hash - Computer hash value for a name
 */

static db_dict_hash(name)
 char *name;
{
	long x=0;

	while (*name) x += *name++;
	
	return(x % DICT_HASH_SIZE);
}




/*
 *	db_dict_init - Initialize a Dictionary
 */

DICT db_dict_init()
{	DICT  dict;
	int   i;


	db_error = 0;

	dict = (DICT) calloc(sizeof(struct db_dict_hdr), 1);
	if (!dict)
	{	db_error = DB_ALLOC_ERROR;
		return(NULL);
	}

	dict->magic = DICT_MAGIC;

	for (i=0; i<DICT_HASH_SIZE; i++)
		dict->htbl[i] = NULL;

	return(dict);
}

/*
 *	db_dict_add - add a new object to the dictionary
 */

void *db_dict_add(dict, name, o_type, d_size, data)
 DICT  dict; 
 char *name; 
 int   o_type; 
 int   d_size; 
 void *data;
{
	short hval, o_size;
	DOBJ  dobj;
	char *o_name, *o_data;


	db_error = 0;

	if (!dict || dict->magic != DICT_MAGIC) 
	{	db_error = DB_INVALID_DICT;
		return(NULL);
	}

	if (!name || !*name) 
	{	db_error = DB_INVALID_NAME;
		return(NULL);
	}

	strupr(name);
	hval = db_dict_hash(name);

	if (db_dict_find(dict, name, o_type))
	{	db_dict_delete(dict, name, o_type);
		if (db_error) return(NULL);
	}
	else
		db_error = 0;

	o_size = sizeof(struct db_dict_obj) + strlen(name) + d_size;
	dobj   = (DOBJ) calloc(o_size, 1);
	if (!dobj)
	{	db_error = DB_ALLOC_ERROR;
		return(NULL);
	}

	dobj->o_type     = o_type;
	dobj->next       = dict->htbl[hval];
	dict->htbl[hval] = dobj;
	dobj->n_size     = strlen(name);
	dobj->d_size     = d_size;

	o_data           = &dobj->data;
	o_name           = o_data + dobj->d_size;

	strcpy(o_name, name);
	if (data) memcpy(o_data, data, d_size);

	return(o_data);
}

/*
 *	db_dict_delete - delete an object by name
 */

void db_dict_delete(dict, name, o_type)
 DICT  dict; 
 char *name; 
 int   o_type;
{
	DOBJ  prev, cur, next;
	short hval;
	char *cur_name;


	db_error = 0;

	if (!dict || dict->magic != DICT_MAGIC) 
	{	db_error = DB_INVALID_DICT;
		return;
	}

	if (!name || !*name)
	{	db_error = DB_INVALID_NAME;
		return;
	}

	strupr(name);

	hval = db_dict_hash(name);
	prev = NULL;
	cur  = dict->htbl[hval];

	while (cur)
	{	cur_name = &cur->data + cur->d_size;	
		if (strcmp(cur_name, name)==0 &&
		    (cur->o_type==o_type || o_type==0))
		{	if (prev)
				prev->next       = cur->next;
			else
				dict->htbl[hval] = cur->next;

			next = cur->next;
			free(cur);
			cur = next;
		}
		else
		{	prev = cur;
			cur  = cur->next;
		}
	}
}

/*
 *	db_dict_delete_all - delete all objects of a type
 */

void db_dict_delete_all(dict, o_type)
 DICT  dict; 
 int   o_type;
{
	DOBJ  prev, cur, next;
	short hval, i;


	db_error = 0;

	if (!dict || dict->magic != DICT_MAGIC) 
	{	db_error = DB_INVALID_DICT;
		return;
	}

	for (i=0; i<DICT_HASH_SIZE; i++)
	{	prev = NULL;
		cur  = dict->htbl[i];

		while (cur)
		{	if (cur->o_type==o_type || o_type==0)
			{	if (prev)
					prev->next    = cur->next;
				else
					dict->htbl[i] = cur->next;

				next = cur->next;
				free(cur);
				cur = next;
			}
			else
			{	prev = cur;
				cur  = cur->next;
			}
		}
	}
}

/*
 *	db_dict_find - Find a object by name
 */

void *db_dict_find(dict, name, o_type)
 DICT  dict; 
 char *name; 
 int   o_type;
{
	DOBJ  dobj;
	short hval;
	char *o_name;


	db_error = 0;

	if (!dict || dict->magic != DICT_MAGIC) 
	{	db_error = DB_INVALID_DICT;
		return(NULL);
	}

	if (!name || !*name)
	{	db_error = DB_INVALID_NAME;
		return(NULL);
	}

	strupr(name);

	hval = db_dict_hash(name);
	dobj = dict->htbl[hval];

	while (dobj)
	{	o_name = &dobj->data + dobj->d_size;
		if (strcmp(o_name,name)==0 &&
		    (dobj->o_type==o_type || o_type==0))
			return(&dobj->data);
		
		dobj = dobj->next;
	}

	db_error = DB_REC_NOT_FOUND;
	return(NULL);
}

/*
 *	db_dict_find_all - Find all objects by type
 */

db_dict_find_all(dict, o_type, nlist, dlist)
 DICT   dict; 
 int    o_type; 
 char **nlist[]; 
 void **dlist[];
{
	DOBJ  dobj;
	short i, j, cnt;
	char  *tmp, *o_name, *o_data;


	db_error = 0;

	if (!dict || dict->magic != DICT_MAGIC) 
	{	db_error = DB_INVALID_DICT;
		return(0);
	}

	for (i=0, cnt=0; i<DICT_HASH_SIZE; i++)
	{	dobj = dict->htbl[i];
		while (dobj)
		{	if (dobj->o_type==o_type || o_type==0) cnt++;
			dobj = dobj->next;
		}
	}

	if (!cnt)
	{	db_error = DB_REC_NOT_FOUND;
		*nlist = NULL;
		*dlist = NULL;
		return(0);
	}

	*nlist = (char **) calloc(sizeof(char *), cnt);
	if (!*nlist)
	{	db_error = DB_ALLOC_ERROR;
		return(0);
	}

	*dlist = (void **) calloc(sizeof(char *), cnt);
	if (!*dlist) 
	{	db_error = DB_ALLOC_ERROR;
		free(*nlist);
		return(0);
	}
	for (i=0, cnt=0; i<DICT_HASH_SIZE; i++)
	{	dobj = dict->htbl[i];
		while (dobj)
		{	if (dobj->o_type==o_type || o_type==0)
			{	o_data = &dobj->data;
				o_name = o_data + dobj->d_size;
 				(*nlist)[cnt] = o_name;
				(*dlist)[cnt] = o_data;
				cnt++;

				for (j=cnt-1; j>0; j--)
				{	if (strcmp((*nlist)[j],(*nlist)[j-1])<0)
					{	tmp           = (*nlist)[j];
						(*nlist)[j]   = (*nlist)[j-1];
						(*nlist)[j-1] = tmp;
						tmp           = (*dlist)[j];
						(*dlist)[j]   = (*dlist)[j-1];
						(*dlist)[j-1] = tmp;
					}
					else
						break;
				}
			}
			dobj = dobj->next;
		}
	}
	return(cnt);
}

/*
 *	db_dict_load - Load dictionary entries from a file
 */

void db_dict_load(dict, path, fname)
 DICT  dict; 
 char *path, *fname;
{
	DATA_FILE df;
	FILE_HDR  fh;
	DOBJ      dobj;
	char     *o_name, *buf;
	unsigned  o_size, hval;


	db_error = 0;

	if (!dict || dict->magic != DICT_MAGIC) 
	{	db_error = DB_INVALID_DICT;
		return;
	}

	if (!fname || !*fname)
	{	db_error = DB_INVALID_NAME;
		return;
	}

	df = db_open(path, fname);
	if (db_error) return;

	fh  = (FILE_HDR)df->df_fhdr;
	buf = (char *)calloc(fh->fh_rec_size, 1);
	if (!buf)
	{	db_error = DB_ALLOC_ERROR;
		return;
	}

	db_read_first(df, buf, &o_size);
	while (!db_error)
	{	dobj = (DOBJ) calloc(o_size, 1);
		if (!dobj) 
		{	db_error = DB_ALLOC_ERROR;
			free(buf);
			return;
		}

		memcpy(dobj, buf, o_size);

		o_name           = &dobj->data + dobj->d_size;
		hval             = db_dict_hash(o_name);
		dobj->next       = dict->htbl[hval];
		dict->htbl[hval] = dobj;

		db_read_next(df, buf, &o_size);
	}
	if (db_error = DB_END_OF_FILE) db_error = 0;
	free(buf);
	db_close(df);
}

/*
 *	db_dict_dump - Dump dictionary objects to a file
 */

void db_dict_dump(dict, path, fname)
 DICT  dict; 
 char *path, *fname;
{
	DATA_FILE df;
	DOBJ      dobj;
	unsigned  o_size, i, cnt, max_size;
	char      options[50];

	db_error = 0;

	if (!dict || dict->magic != DICT_MAGIC) 
	{	db_error = DB_INVALID_DICT;
		return;
	}

	if (!fname || !*fname)
	{	db_error = DB_INVALID_NAME;
		return;
	}

	for (i=0, cnt=0, max_size=0; i<DICT_HASH_SIZE; i++)
	{	dobj = dict->htbl[i];
		while (dobj)
		{	o_size = sizeof(struct db_dict_obj) + dobj->d_size
			       + dobj->n_size;
			if (o_size > max_size) max_size = o_size;	
			cnt++;
			dobj = dobj->next;
		}
	}

	if (!cnt)
	{	db_error = DB_REC_NOT_FOUND;
		return;
	}

	sprintf(options,"VAR, REC=%d", max_size);
	df = db_create(path, fname, options);
	if (db_error) return;

	for (i=0; i<DICT_HASH_SIZE; i++)
	{	dobj = dict->htbl[i];
		while (dobj)
		{	o_size = sizeof(struct db_dict_obj) + dobj->d_size
			       + dobj->n_size;
			db_add(df, dobj, o_size);
			if (db_error) return;
			dobj = dobj->next;
		}
	}
	db_close(df);
}

/*
 *	db_dict_close - Close a dictionary and dispose of its contents
 */

void db_dict_close(dict)
 DICT dict;
{
	DOBJ  dobj, next;
	short i;


	db_error = 0;

	if (!dict || dict->magic != DICT_MAGIC) 
	{	db_error = DB_INVALID_DICT;
		return;
	}

	for (i=0; i<DICT_HASH_SIZE; i++)
	{	dobj = dict->htbl[i];
		while (dobj)
		{	next = dobj->next;
			free(dobj);
			dobj = next;
		}
	}
	dict->magic = 0;
	free(dict);
}

/*
 *	strupr - convert string to upper case
 */

static void strupr(str)
 char *str;
{
	while (*str)
	{	if (islower(*str)) *str = toupper(*str);
		str++;
	}
}
