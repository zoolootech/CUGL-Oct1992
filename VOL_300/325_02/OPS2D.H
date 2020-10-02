
extern	void	far pascal  MapWorld(float,float,float,float);
extern	int	far pascal  WtoDX(float);
extern	int	far pascal  WtoDY(float);
extern	int	far pascal  LWtoDX(float);
extern	int	far pascal  LWtoDY(float);

extern	void	far pascal  Line(float,float,float,float);
extern	void	far pascal  Ellipse(float,float,float,float);

#define WtoDX(xw) (int) (stvwx * (xw - Wrldxy[0]) + Wrldxy[4])
#define WtoDY(yw) (int) (stvwy * (yw - Wrldxy[1]) + Wrldxy[5])
#define LWtoDX(x) abs((int) (stvwx * x))
#define LWtoDY(y) abs((int) (stvwy * y))

#define Line(x1,y1,x2,y2) LineD(WtoDX(x1),WtoDY(y1),WtoDX(x2),WtoDY(y2))
#define Ellipse(xc,yc,a,b) EllipseD(WtoDX(xc),WtoDY(yc),LWtoDX(a),LWtoDY(b))

extern	Word	far pascal Arc(float, float, float, float, float,float,\
			       float *, float *, Word);

extern	int	far pascal PolyLine(Word,float far *,float far *);
extern	void	far pascal FillPoly(Word, float far *,float far *);

extern	void	far pascal MkIdnt2D(float *);
extern	void	far pascal PolyCntr2D(Word,float *,float *,float *,float *);
extern	void	far pascal TrnsfrmPnts2D(Word,float *,float *,float *);
extern	void	far pascal Scale2D(float,float,float,float,float *);
extern	void	far pascal Rotate2D(float,float,float,float *);
extern	void	far pascal Trnslt2D(float,float,float *);
extern	void	far pascal CmbnTrnsfrm2D(float *, float *);
extern	void	far pascal XRflct2D(float *);
extern	void	far pascal YRflct2D(float *);
extern	void	far pascal XShr2D(float, float *);
extern	void	far pascal YShr2D(float, float *);

