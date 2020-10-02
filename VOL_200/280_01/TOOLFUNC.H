/* [toolfunc.h of JUGPDS Vol.46] */
/*
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*       Modifird by Toshiya Oota   (JUG-CPM No.10)              *
*                   Sakae ko-po 205 				*
*		    5-19-6 Hosoda				*
*		    Katusikaku Tokyo 124			*
*								*
*		for MS-DOS Lattice C V3.1J & 80186/V20/V30	*
*								*
*	Edited & tested by Y. Monma (JUG-CP/M Disk Editor)	*
*			&  T. Ota   (JUG-CP/M Sub Disk Editor)	*
*								*
*****************************************************************
*/

/* Header files for Software Tools main routine */

#ifndef	NARGS
extern	int	makpat(char *,int,char,char *);
extern	void	_copy(char *,char *,int);
extern	int	getccl(char *,int *,char *,int *);
extern	int	stclos(char *,int *,int ,int);
extern	void	filset(char ,char *,int *,char *,int *,int);
extern	int	esc(char *,int *);
extern	void	dodash(char *,char *,int *,char *,int *,int);
extern	int	addset(char ,char *,int *,int);
extern	int	xindex(char *,int ,int ,int);
extern	int	cindex(char *,int);
extern	int	getlin(char *,int);
extern	int	strdcmp(char *,char *);
extern	int	strdfcmp(char *,char *);
extern	int	strfcmp(char *,char *);
extern	int	atoi(char *);
extern	void	error(char *);
extern	int	fgetoin(FILE *,char *,int);
extern	int	getword(char *,int);
extern	int	getlin(char *,int);
extern	int	getwrd(char *,int,char *);
extern	int	*itoa(char *,int);
extern	char	*strsave(char *);
extern	int	type(char);
extern	char	*_itoa(char *,int);
extern	int	locate(char,char *,int);
extern	int	getpat(char *,char *);
#else
extern	int	makpat();
extern	void	_copy();
extern	int	getccl();
extern	int	stclos();
entern	void	filset();
extern	int	esc();
exterm	void	dodash();
extern	int	addset();
extern	int	xindex();
extern	int	cindex();
extern	int	getlin();
extern	int	strdcmp();
extern	int	strdfcmp();
extern	int	strfcmp();
extern	int	atoi();
extern	void	error();
extern	int	fgetoin();
extern	char	getword();
extern	int	getlin();
extern	int	getwrd();
extern	int	*itoa();
extern	char	*strsave();
extern	char	type();
extern	char	*_itoa();
extern	int	locate();
extern	int	getpat();
#endif
