// VECTOR.H - a library of C++ routines for manipulating VECTORS
//			 which are ordered arrays of real or complex numbers.
#ifndef  VECTOR_H
	#define VECTOR_H


#include <math.h>
#include <values.h>		// MINFLOAT, MAXFLOAT
#include <complex.h>

#ifndef SMALL_VAL
	#define SMALL_VAL  (1e-30)
	#define LARGE_VAL  (1e+30)
#endif

#ifndef PI
	#define PI  M_PI
#endif


class CVector;		// forward reference to confer friend status.
class Matrix;
class CMatrix;

class Vector
	{
	private:
		int n;  float *v;
		friend  CVector;
		friend  Matrix;
		friend  CMatrix;
	public:
	Vector(int=0);					// initialize n elements to zero

	Vector(int i, float minval, float maxval)
		{Vector *vptr=new Vector (i); n=i; v=vptr->v; base(minval, maxval); };




		
	Vector( Vector& );				// copy Constructor
	~Vector();

	void realloc ( int=0 );			// change size of vector


	// ways to address components
	float 	operator[](int i) { return v[i]; };	// value lookup
			operator float*() { return v; };	// cast to float
			
	float * data()	{return v; };
    int		dim()	{ return n; };				// dimension

	float   set(int i, float f=0)	{ v[i]=f; return f; }	// assign into.
	float   get(int i)				{ return v[i]; }



	Vector& operator=(Vector&); 	// assign Vector to Vector
	Vector& operator=(float);		// assign float to Vector

	Vector& operator+=(float);		// add to Vector... a float
	Vector& operator+=(Vector&);	// 				... a Vector

	Vector& operator*=(float);		// multiply Vector by scalar.
	Vector& operator*=(Vector&);	// form inner product of 2 V's.

	Vector& operator-=(float);
	Vector& operator-=(Vector&);

	Vector& operator/=(float);		// checks for zero divide.

	friend float sum (Vector&,  int=0, int=-1 );
									// sum between point1 and point2
									// if point2 == -1, sums to end of V.
	friend float norm (Vector&,  int=0, int=-1 );
									// norm between point1 and point2
									// if point2 == -1, sums to end of V.

	Vector& rotate ( int phase );	// rotate phase# of pts to the left, wraps.
									// VECROT.CPP
									
	Vector& reverse ( void );		// reverse vector in place. VECRVRS.CPP
	
	void base ( float minval,  float maxval );		// fill in linear interp.
													//  VECBASE.CPP
													

	void show (char *title);		// VECSHOW.CPP

													
	// cross correlation between 2 vectors. results placed in r & t.
	//					found in VECCORLT.CPP
	friend void correlate ( Vector&, Vector&, float *r, float *t );

	};	//  end definition of class Vector.


		// graphVector () - popup window with graph of vector
		// 					Xbase is used to label the X axis. default is V.n
		//					Window is auto-centering.
		//					SOURCE:  VECGRPH.CPP
void  graphVector( Vector&, char *title=NULL, float Xbase= -1, int seg_win=10 );

class CVector			// Complex Vectors.
	{
	public:
		int ispolar;
		Vector x;
		Vector y;

		void rectify(void);	// make sure all mags are + if polar
							// ie: if CVector is polar,
							// flip angles to make mags>0
							// source: VECZRECT.CPP


	// source for assignment, addition, multiplication: VECZ.CPP
	
	CVector(int i=0)					// initialize n elements to zero
		: x(i), y(i) { ispolar=0; };
	CVector( Vector& xv )				// initialize from a real Vector
		: x(xv), y(xv.n) { ispolar =0; };
	
	CVector( CVector& zv )		
		: x(zv.x), y(zv.y)  {ispolar = zv.ispolar;};	// copy constructor.
	
	// construct CVectors with linear interpolation of values.
	CVector ( int i, float minval, float maxval ) 
			: x(i,minval,maxval), y(i) {ispolar=0;};
	CVector ( int i, complex minval, complex maxval ) 
			: x(i,real(minval),real(maxval)), y(i,imag(minval),imag(maxval))
			 {ispolar=0;};
	
		// no destructor needed.
		// taken care of by Vector class.


	void realloc (int i =0)			// change size of Cvector
		{ x.realloc(i), y.realloc(i);};
		
	// ways to address components
	complex	operator[](int i) { return complex(x[i],y[i]); };
	int		dim()	{ return x.dim(); };				// dimension
	friend  Vector& real ( CVector& vec )    {return  vec.x; };
	friend  Vector& imag ( CVector& vec )    {return  vec.y; };

	complex set(int i, complex z)
				  {
				  x.set(i, real(z));
				  y.set(i, imag(z));
				  return z;
				  }

	complex set(int i, float rl, float im=0)
				  {
				  x.set(i, rl);
				  y.set(i, im);
				  return complex (rl,im);
				  }

	complex get(int i)	{ return complex(x[i], y[i] ); }	// like []




	CVector& operator=(CVector&); 	// assign      	CVector to CVector
	CVector& operator=(Vector&); 	// assign real 	Vector 	to CVector
	CVector& operator=(float);		// assign 		float 	to CVector
	CVector& operator=(complex);	// assign 		complex to CVector

	CVector& operator+=(float);		// add to CVector... a float
	CVector& operator+=(complex);	// add to CVector... a complex
	CVector& operator+=(Vector&);	// 				... a Vector
	CVector& operator+=(CVector&);	// 				... a CVector

	CVector& operator*=(float);		// multiply CVector by scalar.
	CVector& operator*=(complex);	// multiply CVector by complex scalar.
	CVector& operator*=(CVector&);	// form inner product of 2 CV's.
	
	CVector& operator-=(float);		// source: VECZSUB.CPP
	CVector& operator-=(complex);
	CVector& operator-=(Vector&);	// subtract real part from CVector.
	CVector& operator-=(CVector&);


	CVector& operator/=(float);		// source: VECZDIV.CPP
	CVector& operator/=(complex);	// checks for zero divide.

	friend complex sum (CVector&,  int=0, int=-1 );
									// sum between point1 and point2
									// if point2 == -1, sums to end of V.
									// source: VECZSUM.CPP
	friend float norm (CVector&,  int=0, int=-1 );
									// norm between point1 and point2
									// if point2 == -1, sums to end of V.
									// source: VECZNORM.CPP

	CVector& rotate (int phase)	{x.rotate(phase);y.rotate(phase);return *this;};
	
	CVector& reverse (void) { x.reverse(); y.reverse(); return *this; };


	// polar 2 rectangular conversions and vice versa.
	// source:  VECPOLAR.CPP, VECRECT.CPP and VECPHCNT.CPP
	void ToPolar ( void );		// angle ranges -PI to PI
	void ToRect  ( void );
	void PhaseCenter ( float new_min_phase = -PI );		// adjust all phases

	};   // end of definition of CVector.

// conversions for single complex numbers. performed in place	
// 	also found in VECPOLAR.CPP, VECRECT.CPP and VECPHCNT.CPP
void ToPolar ( complex& );
void ToRect  ( complex& );
void PhaseCenter ( complex& , float new_min_phase = -PI);	
	
	

// extraction of eeg buffered, interleaved data to single channel Vector form.
// 		v = this Vector of pre-set size will be filled with eeg data 
//		scan and chan indicated start scan in buffer and channel number to use
//		skip defines number of points to average together 
//					from eeg buffer for each newly created vector point.
//					vector is shorter than eeg by factor of skip. 
//		source: EEG2VEC.CPP
void eeg2Vector ( Vector& v,  int scan, int chan, int skip =1 );


	
#endif 		// end of VECTOR_H
