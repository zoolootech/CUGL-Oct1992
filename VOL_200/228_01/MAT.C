/*
HEADER:         CUGXXX;
TITLE:          Matrix decomposition & double-back substitution;
DATE:           3-20-86;
DESCRIPTION:    Matrix mathematics;
KEYWORDS:       Matrix decomposition, Matrix double-back substitution;
FILENAME:       MAT.C;
WARNINGS:       None;
AUTHORS:        Unknown;
COMPILER:       DeSmet C;
REFERENCES:     US-DISK 1308;
ENDREF
*/
mdecomp(a,n,intr1,cdim,det)  /* matrix decomposition routine
				in single precision */

/* includes partial pivoting and determinant calculation */

/* returns:	+1	if successful
		-val	if singularity is detected, val is the step it was on
		0	if input is invalid.
*/
float a[]; /* a is the matrix stored in a[row][column] form */
int n; /* n is the order of the matrix */
int intr1[]; /* intr1 is a vector used to store the row interchanges */
int cdim; /* cdim is the column dimension of a and intr1 */
double *det; /* det is the determinant of the matrix */

{
    int row,col,nm1; /* nm1 is n-1 */
    int lpr; /* lpr is the largest pivot row */
    int col2; /* col2 is the column for row interchanging */
    float lpe; /* lpe is the largest pivot element */
    float ab; /* ab is a tempary absolute value */
    float temp; /* temp is a temporary storage */
    double q; /* q is a quotient */
    float *pa1,*pa2,*pa3,*pa4; /* 1st 2nd 3rd and 4th pointers to a */
    int itr=1; /* itr keeps track of even or odd row interchanges */
    int cdimp1; /* cdimp1 is cdim+1 */
    double fabs();

	intr1[0] = 0; /* initialize intr1[0] to 0 to check errors in mback */
	nm1 = n-1;
	cdimp1=cdim+1;
	*det = 0.;

/* check input */
	if( n <= 1  ||	cdim < n )
		return(0);

/* decompose matrix */
	pa1 = a; /* pa1 points to a(col,col) */
	for (col = 1 ; col <= nm1 ; col++)
	{

	    intr1[col] = 0;
	    lpr = col; /* the current row is col */
	    pa2 = pa1; /* pa2 points to a(row,col) */
	    lpe = fabs( *pa2 );
	    for (row = col+1 ; row <= n ; row++) /* search for largest pivot */
	    {
		pa2 += cdim; /* pa2 points to a(row,col) */
		ab = fabs( *pa2 );
		/* if the largest pivot element is smaller than the current
		row, then make the current row the pivot row */
		if(lpe < ab)
		{
		    lpr = row;
		    lpe = ab;
		}
	    }

	    if (lpe == 0.) /* check for singularity */
		return(-col);

	    if(lpr != col) /* check for necessity of row interchanges */
	    {
		itr *= -1; /* each row interchange changes the sign of det */
		intr1[col] = lpr;
		pa2 = a + (lpr-1)*cdim; /* pa2 points to a(lpr,col2) */
		pa3 = pa1 - col + 1; /* pa3 points to a(col,col2) */
		for(col2 = 1 ; col2 <= n ; col2++) /* interchange rows */
		{
		    temp = *pa2;
		    *pa2++ = *pa3;
		    *pa3++ = temp;
		}
	    }

	    pa3 = pa1; /* pa3 points to a(col,col2) */
	    for (col2 = col + 1 ; col2 <= n ; col2++) /* normalize */
		*++pa3 /= *pa1;    /* a(col,col+1) through a(col,n) */

	    pa2 = pa1; /* pa2 points to a(row,col) */
	    for (row = col+1 ; row <= n ; row++) /* calc new mat elements */
	    {
		pa2 += cdim; /* pa2 points to a(row,col) */
		pa3 = pa1; /* pa3 points to a(col,col2) */
		pa4 = pa2; /* pa4 points to a(row,col2) */
		for (col2 = col+1 ; col2 <= n ; col2++)
		    *++pa4 -= (*pa2)*(*++pa3);
		    /* the previous statement is a(row,col2) =
		    a(row,col2) - a(row,col)*a(col,col2) */
	    }
	    pa1 += cdimp1; /* pa1 points to a(col,col) */
	}
	if(*pa1 == 0.) /* check if a(n,n) is 0. */
	    return(-n);

 /* find determinant */

	*det = 1.;
	pa1=a; /* pa1 points to a(col,col) */
	for (col = 1 ; col <= n ; col++)
	{
	    *det *= *pa1;
	    pa1 += cdimp1;
	}
	*det *= itr;
	intr1[0] = itr;

/* return */
	return(1);

}



mback(b,m,cdimb,intr1,a,n,cdima) /* matrix double back substitution
					in single precision */
/* used with mdecomp */
/* returns:
		0	if input is invalid
		+1	if successful
*/

float b[]; /* right hand side matrix on input, solution matrix on output */
int m; /* number of right hand sides */
int cdimb; /* column dimension of b */
int intr1[]; /* vector used to store row interchanges */
float a[]; /* decomposed a matrix, is not altered in this routine */
int n; /* order of matrix a */
int cdima; /* column dimension of a */

{
    int row,col,bcol;
    int nm1,bcolm1; /* nm1 is n-1, bcolm1 is bcol-1 */
    int rowint; /* rowint stands for row interchange */
    float *pa1,*pa2; /* pa1 and pa2 are pointers for a */
    float *pb1,*pb2; /* pb1 and pb2 are pointers for b */
    float temp; /* temporary storage */
    int cdimap1; /* cdimap1 is cdimb+1 */

/* check input */
	if (m < 1 || cdimb < m || n <= 1 || cdima < n || intr1[0] == 0)
	    return(0); /* intr1[0]=0 if there was an error in mdecomp or
			a was singular */

	nm1 = n-1;

/* interchange the right hand side vector if row interchanges were made */

	for (row = 1 ; row <= nm1 ; row++)
	{
	    rowint = intr1[row];
	    if (rowint != 0) /* there was a row interchange */
	    {
		pb1 = b + (row-1)*cdimb; /* pb1 points to b(row,bcol) */
		pb2 = b + (rowint-1)*cdimb; /* pb2 points to b(rowint,bcol) */
		for (bcol = 1 ; bcol <= m ; bcol++)
		{
		    temp = *pb1;
		    *pb1++ = *pb2;
		    *pb2++ = temp;
		}
	    }
	}

/* calculate new b's based on row addition
as previously performed on a in mdecomp */

	cdimap1 = cdima + 1;

	for (bcol = 1 ; bcol <= m ; bcol++)
	{
	    bcolm1 = bcol - 1;
	    pb1 = b + bcolm1; /* pb1 points to b(col,bcol) */
	    pa1 = a; /* pa1 points to a(col,col) */
	    for (col = 1 ; col <= n ; col++)
	    {
		*pb1 /= *pa1;
		pb2 = pb1; /* pb2 points to b(row,bcol) */
		pa2 = pa1; /* pa2 points to a(row,col) */
		for (row = col + 1 ; row <= n ; row++)
		{
		    pb2 += cdimb;
		    pa2 += cdima;
		    *pb2 -= (*pa2)*(*pb1);
		}
		pb1 += cdimb;
		pa1 += cdimap1;
	    }
	}

/* perform the double back substitution */

	for (bcol = 1 ; bcol <= m ; bcol++)
	{
	    bcolm1 = bcol - 1;
	    pb1 = b + n*cdimb + bcolm1; /* pb1 points to b(col+1,bcol) */
	    for (col = nm1 ; col >= 1 ; col--)
	    {
		pa1 = a + col; /* pa1 points to a(row,col+1) */
		pb1 -= cdimb;
		pb2 = b + bcolm1; /* pb2 points to b(row,bcol) */
		for (row = 1 ; row <= col ; row++)
		{
		    *pb2 -= (*pa1)*(*pb1);
		    pa1 += cdima;
		    pb2 += cdimb;
		}
	    }
	}
	return(1);

}
