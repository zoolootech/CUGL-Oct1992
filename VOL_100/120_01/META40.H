/* HEADER: CUG120.13;
   TITLE: META4;
   VERSION: 1.0;
   DATE: 08/00/1981;
   DESCRIPTION: "Header file for Dr. W.A. Gale's META4 compiler-compiler from DDJ August 1981";
   KEYWORDS: compiler-compiler,programming languages;
   SYSTEM: CP/M;
   FILENAME: META40.H;
   CRC: FE57;
   AUTHORS: W.A.Gale, Jan Larsson;
   COMPILERS: BDS C;
   REFERENCES: AUTHORS: W.A.Gale; TITLE: "META4 Compiler-Compiler";
	CITATION: "Doctor Dobb's Journal, August 1981" ENDREF;
*/

#include "bdscio.h"



/*
	the global declarations for Dr. Gales Compiler-Compiler
*/


char er;
int iav[10], iac ;
char m4file[20],metfile[20],outfile[25];
char printflag, spcharflag, eoflag;

char aa,bb,bo[80],c0,c1,c2,c3,c9,cb,cc,cd,ce,cg,cl,cm,cp,cq,cs,ct;
char cu,cv,cx,dd,ds[10],ee,f1[BUFSIZ],f2[BUFSIZ],fl,ks[10000],li,ll;
char mc[3000],mk,mn,nd,nl,ns[80],os[80],pb,pi,pl,pm,pn,po,qi,rc;
char ri[80],sd,wa,wb,x0,x1,x2,x3,x9,xa,xb,xc,xd,xe,xf,xg,xh,xi,xj,xk;
char xl,xm,xn,xo,xp,xq,xr,xs,xt,xu,xv,xw,xx,xy,xz,yp,zp,zx;

int i00,i01,i03,i10,i16,iaa,ibb,ibk,icc,idd,ilb,iln,ilt[1000],imb;
int imd,imf,imi[3000],iml,imm,imt,imx,imz,inl,ipc,ipl,ipr[10];
int ipt,irn,ism,ist[600],itu,iuu,ixx,iys[80],iyy,izc,izs[80],izt;



 FILENAME: USERCODE.C;
   WARNINGS: "Doesn't wo