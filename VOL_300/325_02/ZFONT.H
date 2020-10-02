#ifndef FONT_H
#define FONT_H
#ifdef __cplusplus
    extern "C" {
#endif

enum	SFntDrctnTp {left,right,up,down};

extern	void	far pascal SFntStClr(unsigned char);
extern	void	far pascal SFntStSz(float,float);
extern	void	far pascal SFntStOrntn(float);
extern	void	far pascal SFntStLctn(float,float);
extern	void	far pascal SFntStDrctn(enum SFntDrctnTp);
extern	void	far pascal SFntYRflct(void);
extern	int	far pascal SFntLd(void *);
extern	int	far pascal SFntInit(void *);
extern	void	far pascal SFntDsply(char *);

extern	int	far pascal BFntInit(void *,int);
extern	void	far pascal BFntStLctn(int,int);
extern	void	far pascal BFntStSz(int,int);
extern	int	far pascal BFntLd(void *,int);
extern	void	far pascal BFntDsply(char *);
extern	void	far pascal BFntStOrntn(int);

extern	Byte	near BFntHSz;
extern	Byte	near BFntVSz;
extern	Byte	near BFntFClr;
extern	Word	near BFntHght;
extern	Word	near BFntWdth;

extern	float	near SFntHght;
extern	float	near SFntWdth;

enum	SFntDrctnTp SFntDrctn;
Byte	  near SFntClr;

#define     BFntStClr(c)    BFntFClr=c
#define     SFntYRflct()    YRflct2D(SFntYM);
#define     SFntStClr(Clr)  SFntClr=Clr
#define     SFntStDrctn(d)  SFntDrctn=d

extern	void	far pascal PlayNote(unsigned,unsigned);

#ifdef __cplusplus
    }
#endif

#endif
