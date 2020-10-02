/*
	Function Prototypes for all routines of RED.

	Source:  redtmp.h
	Started: December 13, 1989
	Version: January 18, 1990

	Written by
	
		Edward K. Ream
		166 N. Prospect
		Madison WI 53705
		(608) 257-0802


	PUBLIC DOMAIN SOFTWARE

	This software is in the public domain.

	See red.h for a disclaimer of warranties and other information.
*/

/*
	redmain.c
*/
extern void	main	(int argc,char * *argv);
extern void	getcmnd	(char *args);

/*
	redcmnd.c
*/
extern void	again	(void);
extern void	change	(char *args);
extern void	clear1	(void);
extern void	copy	(char *args);
extern void	delete	(char *args);
extern void	extract	(char *args);
extern void	find	(char *args);
extern void	findr	(char *args);
extern void	help	(void);
extern void	inject	(char *args);
extern void	list	(char *args);
extern int	load	(char *args);
extern void	move	(char *args);
extern void	name	(char *args);
extern int	name1	(char *args,char *filename);
extern int	resave	(void);
extern int	save	(void);
extern void	search	(char *args);
extern void	tabs	(char *args);

extern int	chkbuf	(void);			/* Utility routines. */
extern int	chkkey	(void);
extern void	cmndmess(char *mess);
extern int	get3args(char *args,int *val1,int def1,int *val2,int def2,int *val3,int def3);
extern void	message	(char *s);

/*
	reded.c
*/
extern void	edabt	(void);
extern void	edbegin	(void);
extern void	edbword	(void);
extern void	edchng	(char c);
extern void	edclr	(void);
extern void	eddel	(void);
extern void	ed2del	(void);
extern void	eddn	(void);
extern void	edend	(void);
extern void	edfword	(void);
extern void	edgetln	(void);
extern void	edgo	(int newline,int x);
extern int	edins	(char c);
extern void	edjoin	(void);
extern void	edkill	(char c);
extern void	edleft	(void);
extern void	ednewdn	(void);
extern void	ednewup	(void);
extern void	edrepl	(void);
extern void	edright	(void);
extern void	edsplit	(void);
extern void	edsrch	(char c);
extern void	edup	(void);
extern void	edzap	(void);

/*
	redfmt.c
*/
extern void	fmtadj	(char *buf,int minind,int maxind);
extern void	fmtassn	(int listflag);
extern void	fmtcrlf	(void);
extern void	fmtset	(int n);
extern void	fmtsout	(char *buf,int offset);
extern void	fmtsubs	(char *buf,int i,int j);

/*
	redout.c
*/
extern int	outchar	(char c);
extern void	outclr	(void);
extern void	outdel	(void);
extern void	outdelln(void);
extern void	outdeol	(void);
extern void	outinit	(void);
extern void	outins	(void);
extern void	outsdn	(void);
extern void	outsup	(void);
extern void	outxy	(int x,int y);

/*
	redpmt.c
*/
extern void	pmtclr	(void);
extern void	pmthold	(int flag);
extern void	pmtzap	(void);
extern void	pmtupd	(void);
extern void	pmtmess	(char *s1,char *s2);
extern void	pmtmode	(char *newmode);
extern void	pmtfn	(void);
extern int	pmtlast	(void);

/*
	redsys.c
*/
extern char *	sysalloc	(int n);
extern int	syscin		(void);
extern int	sysclose	(int fd);
extern void	syscopfn	(char *args,char *buffer);
extern int	syscout		(char c);
extern int	syscstat	(void);
extern int	syscreat	(char *filename);
extern void	sysexec		(char *args);
extern int	sysexists	(char *filename);
extern int	sysfclose	(FILE *fd);
extern FILE *	sysfcreat	(char *filename);
extern int	sysfgets	(FILE *fd,char *buffer,int maxlen);
extern int	sysflush	(int fd,char *buffer,int n);
extern FILE *	sysfopen	(char *filename);
extern int	sysgetc		(FILE *fd);
extern void	sysinit		(void);
extern void	sysintr		(int systl,int systy,int sysn);
extern int	syslout		(char c);
extern void	sysmove		(char *source,char *dest,int count);
extern int	sysopen		(char *name);
extern int	sysputc		(char c,FILE *fd);
extern int	sysread		(int fd,char *buffer);
extern void	sysrename	(char *oldname,char *newname);
extern int	sysseek		(int fd,int block);
extern int	sysunlink	(char *filename);
extern void	syswait		(void);
extern int	syswrite	(int fd,char *buffer,int n);

/*
	redutl.c
*/
extern void	error	(char *message);
extern int	itoc	(int n,char *str,int size);
extern int	number	(char *args,int *val);
extern void	putdec	(int n,int w);
extern void	warning	(char *message);

/*
	redbuf1.c
*/
extern int	bufatbot	(void);
extern int	bufattop	(void);
extern int	bufchng		(void);
extern void	buf_clr		(void);
extern void	bufdn		(void);
extern void	bufend		(void);
extern void	bufgo		(int n);
extern void	buf_gobk	(void);
extern void	buf_gofw	(void);
extern void	bufinit		(void);
extern int	bufln		(void);
extern int	bufmax		(void);
extern void	bufnew		(void);
extern int	bufnrbot	(void);
extern void	bufout		(int topline,int topy,int nlines);
extern void	bufoutln	(int line);
extern void	bufrepl		(char *line,int n);
extern void	bufreset	(char *window_file);
extern void	bufsaved	(void);
extern void	bufsusp		(void);
extern void	bufup		(void);

/*
	redbuf2.c
*/
extern void	bufdel		(void);
extern void	bufdeln		(int nlines);
extern int	bufgetln	(char *line,int linelen);
extern void	bufins		(char *insline,int inslen);
extern void	buf_d		(int nlines);
extern void	combine		(void);

/*
	redbuf3.c
*/
extern int	data_open	(void);
extern void	do_lru		(struct BLOCK *bp);
extern void	disk_error	(char *message);
extern void	disk_full	(void);
extern void	disk_rdy	(void);
extern void	is_dirty	(struct BLOCK *bp);
extern void	put_block	(struct BLOCK *bp, int diskp);
extern char *	put_buf		(int avail);
extern void	put_slot	(struct BLOCK *bp);
extern void	read_file	(char *file_name);
extern void	swap_all	(void);
extern void	swap_one	(void);
extern struct BLOCK *swap_in	(int diskp);
extern struct BLOCK *swap_new	(int diskp);
extern struct BLOCK *swap_out	(void);
extern void	swap_sync	(int low,int high);
extern void	write_file	(char *file_name);

/*
	redbuf4.c
*/
extern void	bufdump		(void);
extern void	buftrace	(void);
extern void	cant_happen	(char *message);
extern void	check_block	(char *message);
extern void	dump_block	(struct BLOCK *bp);
extern void	dump_memory	(void);
extern void	dump_slots	(void);
extern void	dump_vars	(void);
extern void	trace_block	(struct BLOCK *bp);
extern void	trace_memory	(void);
extern void	trace_vars	(void);
