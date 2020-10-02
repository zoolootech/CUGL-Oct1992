/*   HEADER:   CUG300;
      TITLE:   Matrix library;
       DATE:   4/09/1989;
DESCRIPTION:   "Series of macros and functions which accomodate dynamic
               declaration of arrays on heap. Functions provide
               declaration, access, and various operations on both float
               and string arrays. Float array operations are row and
               column sums, moving averages, cumulative totals,
               determinants, cofactor, inverse, transpose, solution,
               etc. Token arrays may be input and output from commented
               textfiles. Other functions transfer token and float
               elements between the two types of arrays. Maximum array
               size is 64k.";
    VERSION:   2.04;
   KEYWORDS:   heap,array,textfile,i/o,error,float,token,allocation,matrix,
               mathematics;
   FILENAME:   MAT_V2D.H;
     SYSTEM:   MS-DOS;
    AUTHORS:   John J. Hughes;
  COMPILERS:   Turbo C;
*/
/*
Credits: The following routines were based on functions and macros
developed at the University of Maryland, Computer Science Dept. (July
28, 1983 by Fred Blonder) and placed in the public domain:

mfcof      mfdet   mfcpy   mfdump     mfread     mftrnsp
mfmlt      mfslv   mfinv

Remaining functions and macros were developed by:

                           John J. Hughes III
                           928 Brantley Drive
                           Knoxville, TN 37923
                            (615)    693-5695

Users are encouraged to register at the above address and to advise the
author of any errors or omissions found in the source code.

Since the source code is available for a nominal fee, users must bear
full responsibility for verifying the code's correctness and adequacy
for any particular application.  The author disclaims any responsibility
for errors and omissions in the source code and for any damages
resulting from those errors and omissions.

This header file and any object or executable images (i.e. *.obj, *.exe,
*.com or *.lib files) compiled from the source code for functions
prototyped in this header file may be releasesed to the public domain.
The C language source code for the functions prototyped in this header
file is copyrighted. */

#ifndef __STDLIB
#include <STDLIB.H>
#endif

#ifndef __STDIO_DEF_
#include <STDIO.H>
#endif

#ifndef FA_ARCH
#include <DOS.H>
#endif

/*          CONSTANT DEFINITIONS
*/
#define YES       1
#define NO        0
#define STARTED   1
#define ENDED     0
#define M_NULL    0
#define DCLVCT    1
#define VCT       0
#define NRW       66
#define NRM       80
#define WD        128
#define FTOK      16
#define DP        3
#define TOK       24
#define MAXFLDS   32

/*          STRUCTURE TYPE DEFNITIONS
*/
struct fmat {  unsigned int n_rows,n_cols,atrib1,atrib2; float *f; };
struct tmat {  unsigned int n_rows,n_cols,tok_sz,n_recs,*n_toks; char *t;  };

/*          FUNCTIONS PROTOTYPES

            General Purpose Array Functions
*/
void     error    (unsigned int flag, unsigned int errno, unsigned int arg1,
							unsigned int arg2);
void     msc      (FILE *FN,char *text);
void     msl      (FILE *FN,char *text);
void     msr      (FILE *FN,char *text);
void     mst      (FILE *FN,char *label,char *variable);
void     msv      (FILE *FN,char *label,float value);

/*          Floating Point Array Functions
*/
void     mfcnt    (char filename[],unsigned int *rows,unsigned int *cols);
float    mfcof    (struct fmat *mtx,unsigned int i,unsigned int j);
void     mfcpy    (struct fmat *copy_mtx,struct fmat *mtx);
void     mfcumc   (struct fmat *mtx,unsigned int col,unsigned int bgn_row,
							unsigned int end_row,unsigned int result_col);
void     mfcumr   (struct fmat *mtx,unsigned int row,unsigned int bgn_col,
							unsigned int end_col,unsigned int result_row);
float    mfdet    (struct fmat *mtx);
void     mfdump   (struct fmat *mtx);
void     mfget    (char filename[],struct fmat *mtx);
void     mfinv    (struct fmat *inv_mat,struct fmat *mtx);
void     mfmlt    (struct fmat *result_mat,struct fmat *mat1,
                        struct fmat *mat2);
void     mfmvac   (struct fmat *mtx,unsigned int nopds,unsigned int col,
								unsigned int bgn_row,unsigned int end_row,
								unsigned int result_col);
void     mfmvar   (struct fmat *mtx,unsigned int nopds,unsigned int row,
								unsigned int bgn_col,unsigned int end_col,
								unsigned int result_row);
void     mfpgput  (struct fmat *mtx,char filename[],unsigned int ChPrLn,
								unsigned int FldPrLn,unsigned int DcP);
void     mfput    (FILE *FN,struct fmat *mtx,char hdr_lst[],char sel_lst[],
								unsigned int ChPrLn,unsigned int FldPrLn,
								unsigned int DcP);
void     mfread   (struct fmat *mtx,char filename[]);
void     mfslv    (struct fmat *solu_mat,struct fmat *coef_mat,
                        struct fmat *const_mat);
void     mfstore  (char filename[], struct fmat *mtx);
float    mfsumc   (struct fmat *mtx,unsigned int row,unsigned int bgn_col,
								unsigned int end_col);
float    mfsumr   (struct fmat *mtx,unsigned int col,unsigned int bgn_row,
								unsigned int end_row);
void     mftrnsp  (struct fmat *trns_mat,struct fmat *mtx);

/*          Text Array Functions
*/
void     mtapnd   (struct tmat *apnd_mat, struct tmat *src_mat,unsigned int src_row);
void     mtcnt    (char filename[], unsigned int *rows,
							unsigned int *maxtoks,unsigned int *maxtoksz);
void     mtcpy    (struct tmat *source, struct tmat *destin);
void     mtdump   (struct tmat *matrix);
void     mtget    (char filename[],struct tmat *mtx);
void     mtput    (FILE *FN,struct tmat *mtx,char hdlst[], char sel_lst[],
                        unsigned int ChPerLn,unsigned int FldPrLn);
void     mtread   (char filename[],struct tmat *mtx);
void     mtstore  (char filename[],struct tmat *mtx);

/*          Text Array Access Functions
*/
void     ft       (struct tmat *mtx,unsigned int line,unsigned int token,
								float value,unsigned int ChPrLn,unsigned int FldPrLn,
								unsigned int DcPlc);
void     it       (struct tmat *mtx,unsigned int line,unsigned int token,
								unsigned int value,unsigned int ChPrLn,
								unsigned int FldPrLn);
float    tf       (struct tmat *mtx,unsigned int line,unsigned int token,
								unsigned int notoks);
unsigned int 		ti   		(struct tmat *mtx,unsigned int line,unsigned int token,
								unsigned int notoks);
void     tmtofm   (struct fmat *mtxf,struct tmat *mtxt);
char    *ts       (struct tmat *mtx,unsigned int line,unsigned int token,
								unsigned int notoks);
void     tt       (struct tmat *mtx,unsigned int line,unsigned int token,
								char *buffer,unsigned int ChPrLn,
								unsigned int FldPrLn);

/*          GENERAL PURPOSE MACROS
*/
#define  flcreat(filename,FN) if((FN=fopen(filename,"w"))==0)\
         error(NO,23,0,0);{struct date today;getdate(&today);\
         fprintf (FN,"{\n%s  %d/%d/%d\n",\
         filename,today.da_mon,today.da_day,today.da_year);}\
         fprintf (FN,"}\n")
#define  no_cols(mx)    ((mx)->n_cols)
#define  no_recs(mx)    ((mx)->n_recs)
#define  no_rows(mx)    ((mx)->n_rows)
#define  no_toks(mx,rw) ((mx)->n_toks[(unsigned)(rw)])
#define  token_sz(mx)   ((mx)->tok_sz)


#define  all(m,rw,cl)   for (rw=0; rw<((m)->n_rows); rw++)\
                        for (cl=0; cl<((m)->n_cols); cl++)
#define  cols(m,cl)     for (cl=0; cl<((m)->n_cols); cl++)
#define  rows(m,rw)     for (rw=0; rw<((m)->n_rows); rw++)
#define  trecs(mx,rw)   for (rw=0; rw<((m)->n_recs); rw++)

/*          MATRIX ACCESS MACROS
*/
#define  f(mx,rw,cl)    ((mx)->f[(unsigned)(rw)*(mx)->n_cols+(cl)])
#define  t(mx,rw,cl)    (char *)(&(mx)->t[(unsigned)(rw)*((mx)->n_cols)*\
								(mx)->tok_sz+(cl)*(mx)->tok_sz])

/*          MATRIX ALLOCATION & POINTER MACROS
*/
#define fck(mx,rw,cl)   if ( (rw)>=(mx)->n_rows || (cl)>=(mx)->n_cols)\
                        error (YES,19,(rw),(cl));
#define fdim(mx,rw,cl)  { unsigned int nnni;  if\
			( (int) (((mx)=(struct fmat *)\
			malloc((unsigned)sizeof(struct fmat))) == 0)\
			) error (YES,3,rw,cl);\
		if ( (int) ((((mx)->f)=(float *)\
			calloc((unsigned)(cl)*(rw),sizeof(float)))==0)\
			) error (YES,3,rw,cl);\
      ((mx)->n_rows)=rw; ((mx)->n_cols)=cl;\
      ((mx)->atrib1)=rw; ((mx)->atrib2)=1;\
      for (nnni=0; nnni<((rw)*(cl)); nnni++)\
			mx->f[nnni] = 0; }
#define  frel(mx)       free( mx->f );free (mx)
#define  tck(mx,rw,cl,tsz)    if( rw>=mx->n_rows || (cl)>=mx->n_cols\
                         || tsz>=mx->tok_sz ) error(YES,29,(rw),(cl));
#define  tdim(mx,rw,cl,tsz) { unsigned nnni;\
		if ( (int) (((mx)=(struct tmat *)\
		malloc((unsigned)sizeof(struct tmat))) == 0)\
      ) error (YES,9,rw,cl);\
		if ( (int) ((((mx)->t)=(char *) calloc((unsigned)(rw)*(cl)*(tsz+1),\
                                    sizeof(char)))==0)\
      ) error (YES,9,rw,cl);\
		if ( (int) ((((mx)->n_toks) = (unsigned *)calloc((unsigned)(rw),\
												sizeof(unsigned)))==0)\
      ) error (YES,9,rw,cl);\
		((mx)->n_rows)=(unsigned)(rw); ((mx)->n_cols)=(unsigned)(cl);\
		((mx)->tok_sz)=(unsigned)(tsz)+1;  ((mx)->n_recs)=0; \
		for(nnni=0;nnni<(rw);nnni++) (mx)->n_toks[nnni]=0;\
		for(nnni=0;nnni<((rw)*(cl));nnni++) \
			(mx)->t[(unsigned)nnni*(tsz+1)]=NULL; }
#define  trel(mx)       free((mx)->t); free((mx)->n_toks); free(mx)

