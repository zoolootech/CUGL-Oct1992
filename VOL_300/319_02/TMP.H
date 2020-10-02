/*	CPP V5 -- function templates

	Source:  tmp.h
	Started: September 28, 1987
	Version: May 31, 1988.

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#ifndef MICRO_SOFT

/* cpp.c */
void	copy_file	(void);
void	begin_line	(bool flag);
void	bump_line	(void);
void	get_token	(bool expand_flag);

/* def.c */
void	pp_def		(void);
bool	outer_expand	(char *name, bool old_mflag);

/* dir.c */
void	do_pp		(void);

/* exp.c */
void	con_expr	(en_tokens delim);
void	expr		(en_tokens delim);
void	expr1		(en_tokens delim);
bool	is_expr_tok	(void);

/* mem.c */
void *	m_alloc		(int n);
void	m_begin		(void);
void	m_end		(void);
void	m_free		(void * mem);
void	m_stat		(void);

/* mst.c */
void	mst_init	(void);
void	mst2_init	(void);
void	mst_delete	(char *name);
void	mst_enter	(char *name, char *text, int nargs);
bool	mst_lookup	(char *name, char **text, int *nargs);
void	mst_unarg	(char *s);

/* pr.c */
char *	pr_bool		(bool flag);
char *	pr_ch		(int c);
char *	pr_op		(en_tokens tok);
char *	pr_str		(char *s);
char *	pr_tok		(void);

/* str.c */
char *	str_alloc	(char *s);
void	str_cat		(char *s1, char *s2);
char *	str_mcat	(char *s1, char *s2);
void	str_cpy		(char *s1, char *s2);
bool	str_eq		(char *s1, char *s2);
int	str_len		(char *s);
void	str_lower	(char *s);
void	str_upper	(char *s);

/* sys.c */
void	sysabort	(void);
void	syspushback	(int c);
void	sysnext		(void);
void	syspremac	(char old_ch);
void	sysiclose	(void);
int	sysopen		(char *file_name);
int	syscreat	(char *file_name);
void	sysoclose	(void);
void	sysnlput	(void);
void	syscput		(char c);
void	syssput		(char *s);
void	sysend		(void);
void	sysinit		(void);
bool	sysargeq	(char *s1, char *s2);
void	syscsts		(void);
char *	systime		(void);
char *	sysdate		(void);

/* tok.c */
void	t_comment	(void);
void	skip_comment	(void);
void	t_id		(char *buffer, int max_length);
en_tokens
	t_number	(bool dot_flag);
void	t_string	(char *out, int max_out, bool copy_flag);
int	in_string	(char *in, char *out, int max_out);

/* utl.c */
int	char_val	(char *s);
unsigned int
	str_val		(unsigned char *s);

int	conv2i		(char *string, int radix);
void	conv2s		(int a, char *s);
void	convl2s		(long a, char *s);
void	conul2sc	(unsigned long a, char *s, int count);
void	conl2h		(unsigned long a, char *s, int count);

void	skip_bl		(void);
void	skip_1line	(void);
void	skip_past	(void);
void	skip_pp		(void);
void	copy_ws		(bool nl_flag);
void	skip_ws		(bool nl_flag);

void	fatal		(char *message);
void	error		(char *message);
void	err2		(char *mess1, char *mess2);
void	err3		(char *mess1, char *mess2, char *mess3);
void	syserr		(char *message);

#else

/* cpp.c */
void	copy_file	(void);
void	begin_line	(bool);
void	bump_line	(void);
void	get_token	(bool);

/* def.c */
void	pp_def		(void);
bool	outer_expand	(char *, bool);

/* dir.c */
void	do_pp		(void);

/* exp.c */
void	con_expr	(en_tokens);
void	expr		(en_tokens);
void	expr1		(en_tokens);
bool	is_expr_tok	(void);

/* mem.c */
void *	m_alloc		(int);
void	m_begin		(void);
void	m_end		(void);
void	m_free		(void *);
void	m_stat		(void);

/* mst.c */
void	mst_init	(void);
void	mst2_init	(void);
void	mst_delete	(char *);
void	mst_enter	(char *, char *, int);
bool	mst_lookup	(char *, char **, int *);
void	mst_unarg	(char *);

/* pr.c */
char *	pr_bool		(bool);
char *	pr_ch		(int);
char *	pr_str		(char *);
char *	pr_tok		(void);

/* str.c */
char *	str_alloc	(char *);
void	str_cat		(char *, char *);
char *	str_mcat	(char *, char *);
void	str_cpy		(char *, char *);
bool	str_eq		(char *, char *);
int	str_len		(char *);
void	str_lower	(char *);
void	str_upper	(char *);

/* sys.c */
void	sysabort	(void);
void	syspushback	(int);
void	sysnext		(void);
void	syspremac	(char);
void	sysiclose	(void);
int	sysopen		(char *);
int	syscreat	(char *);
void	sysoclose	(void);
void	sysnlput	(void);
void	syscput		(char);
void	syssput		(char *);
void	sysend		(void);
void	sysinit		(void);
bool	sysargeq	(char *, char *);
void	syscsts		(void);
char *	systime		(void);
char *	sysdate		(void);

/* tok.c */
void	t_comment	(void);
void	skip_comment	(void);
void	t_id		(char *, int);
en_tokens
	t_number	(bool);
void	t_string	(char *, int, bool);
int	in_string	(char *, char *, int);

/* utl.c */
int	char_val	(char *);
unsigned int
	str_val		(unsigned char *);

int	conv2i		(char *, int);
void	conv2s		(int, char *);
void	convl2s		(long, char *);
void	conul2sc	(unsigned long, char *, int);
void	conl2h		(unsigned long, char *, int);

void	skip_bl		(void);
void	skip_1line	(void);
void	skip_past	(void);
void	skip_pp		(void);
void	copy_ws		(bool);
void	skip_ws		(bool);

void	fatal		(char *);
void	error		(char *);
void	err2		(char *, char *);
void	err3		(char *, char *, char *);
void	syserr		(char *);

#endif /* MICRO_SOFT */
