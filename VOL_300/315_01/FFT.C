
/* int_scale(), dbl_scale(), and long_scale() moved from herc.c */
/* 10/89, because herc.c is being discontinued, and msherc.com is */
/* being used for hercules graphcis support.  T Clune */

/* fft.c is a file of functions to support fourier transforms. */
/* Written 3/89 by T Clune.  Copyright (c) 1989, Eye Research Institute, */
/* 20 Staniford St, Boston, MA 02114.  All Rights Reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <conio.h>
#include "fft.h"



/* data_multiply() multiplies two data files of equal length together */

void data_multiply(file1, file2)
char file1[], file2[];
{
    FILE *f1, *f2, *f3;
    char string[80];
    int i, numpts1, numpts2;
    double d1, d2, *d_out, minval, maxval;


    f1=fopen(file1,"r");
    if(f1==NULL)
    {
	printf("Error opening data file.  Exiting routine.\n");
	printf("Press any key to continue...\n");
	mgetch();
	return;
    }

    f2=fopen(file2,"r");
    if(f2==NULL)
    {
	printf("Error opening data file.  Exiting routine.\n");
	printf("Press any key to continue...\n");
	mgetch();
	return;
    }

    fscanf(f1,"%d %*lf %*lf", &numpts1);
    fscanf(f2,"%d %*lf %*lf", &numpts2);
    if(numpts1 != numpts2)
    {
	printf("Different size input files.  Exiting routine.\n");
	printf("Press any key to continue...\n");
	mgetch();
	return;
    }

    while(kbhit()) ;
    printf("Enter filespec for output file\n");
    mgets(string);
    f3=fopen(string,"w");

    if(f3==NULL)
    {
	printf("Error opening output file.  Exiting routine.\n");
	printf("Press any key to continue...\n");
	mgetch();
	return;
    }
    d_out=(double *)malloc(sizeof(double)*numpts1);
    if(d_out==NULL)
    {
	printf("malloc() error in data_multiply(). Program aborting.\n");
	exit(-1);
    }


    for(i=0;i<numpts1;i++)
    {
	fscanf(f1,"%lf", &d1);
	fscanf(f2,"%lf",&d2);
	d_out[i]=d1*d2;
    }

    minmax(d_out,numpts1,&minval,&maxval);
    fprintf(f3,"%d %f %f\n", numpts1, minval, maxval);

    for(i=0;i<numpts1;i++)
	fprintf(f3,"%f\n",d_out[i]);

    fclose(f1);
    fclose(f2);
    fclose(f3);
    free(d_out);

}




/* dbl_scale() scales an extended precision floating point number */
/* to an integer value.  It is useful for scaling floating point  */
/* numbers for graphing on the screen.                            */
/* moved from herc.c 10/89 by T Clune */


int dbl_scale(val, min, max, out_min, out_max)

double val, min, max;  /* val= number to be scaled; min= smallest possible */
	/* value for val; max=largest possible value */
int out_min, out_max;  /* out_min=integer to return if val=min; */
	    /* out_max=integer to return if val=max.  NOTE THAT out_min */
	    /* MAY BE LARGER THAN out_max.  If you want to change screen */
	    /* values from 4th quadrant to 1st quadrant, the y-value */
	    /* out_min would be larger than the y-value out_max, while */
	    /* x-value out_max would be larger than the x-value out_min */

{

    int out_val, largest, smallest; /* val returned, larger of out_min */
	    /* and out_max, smaller of the same */

    double scale_factor;    /* ratio of in and out ranges */
	    /* scale the input value */

    scale_factor = (out_max - out_min) / (max - min);
    out_val = (int)((val - min) * scale_factor + out_min);


    if(out_min>out_max)     /* check for max/min inversion */
    {
	smallest = out_max;
	largest = out_min;

    }
    else
    {
	smallest = out_min;
	largest = out_max;
    }

    if(out_val > largest)      /* check for out-of-bounds values */
	out_val = largest;     /* since val is not checked to insure */
    if(out_val < smallest)      /* that it is between min and max, this */
	out_val = smallest;     /* is necessary.  By checking at the last */
			    /* step, any arithmetic round-off accumulation */
			    /* is also trapped */

    return out_val;          /* return the scaled value */

}





/* fft() accepts the time-domain data x,y (n points, where n is a power of 2) */
/* and converts to frequency domain rectangular data if flag==FORWARD, */
/* or accepts frequency-domain data in x,y and converts to time domain if */
/* flag==INVERSE.  FORWARD and INVERSE are defined in fft.h */

void fft(x, y, n, flag)
double x[], y[];
int n, flag;
{
   int maxpower, arg, pnt0, pnt1;
   int i, j=0, a, b, k;
   double prodreal, prodimag, harm, temp;
   double *cos_coef;
   double *sin_coef;

   cos_coef=(double*)calloc(n,sizeof(double));
   sin_coef=(double*)calloc(n,sizeof(double));

   if((cos_coef==NULL) || (sin_coef==NULL))
   {
	printf("malloc() error in fft function\n");
	if(cos_coef != NULL)
	    free(cos_coef);
	if(sin_coef != NULL)
	    free(sin_coef);
	return;
    }

   if(flag==INVERSE)
   {
      for (i=0;i<n;i++)
      {
	 x[i] /=n;
	 y[i] /=n;
      }
   }

   for(i=0;i<(n-1);i++)
   {
      if(i<j)
      {
	 temp=x[i];
	 x[i]=x[j];
	 x[j]=temp;
	 temp=y[i];
	 y[i]=y[j];
	 y[j]=temp;
      }
      k=n/2;
      while(k<=j)
      {
	 j -=k;
	 k /=2;
      }
      j +=k;
   }
   maxpower=0;
   i=n;
   while(i>1)
   {
      maxpower++;
      i /=2;
   }
   harm=2*PI/n;
   for(i=0;i<n;i++)
   {
      sin_coef[i]=flag*sin(harm*i);
      cos_coef[i]=cos(harm*i);
   }
   a=2;
   b=1;

   for(j=0;j<maxpower;j++)
   {
      pnt0=n/a;
      pnt1=0;
      for(k=0;k<b;k++)
      {
	 i=k;
	 while(i<n)
	 {
	    arg=i+b;
	    if(k==0)
	    {
	       prodreal=x[arg];
	       prodimag=y[arg];
	    }
	    else
	    {
	       prodreal=x[arg]*cos_coef[pnt1]-y[arg]*sin_coef[pnt1];
	       prodimag=x[arg]*sin_coef[pnt1]+y[arg]*cos_coef[pnt1];
	    }
	    x[arg]=x[i]-prodreal;
	    y[arg]=y[i]-prodimag;
	    x[i] +=prodreal;
	    y[i] +=prodimag;
	    i +=a;
	 }
	 pnt1 +=pnt0;
      }
      a *=2;
      b *=2;
   }
   free(cos_coef);
   free(sin_coef);
}





/* get_filter() builds a high_pass or low_pass filter.  The arguments are: */
/* FILTER is the array for the filter coefficients (frequency-domain */
/* AMPLITUDE coefficients [0 to 1]), N is the number of elements in FILTER */
/* SIGN is -1 for lowpass, +1 for highpass; NYQUIST is the nyquist freq */
/* for the FILTER data set, CUTOFF is the filter cutoff frequency */
/* ROLLOFF is the rolloff in dBells per filter_unit; */
/* FILTER_UNIT is 2.0 for rolloff as dB/octave or 10.0 for dB/decade; */
/* get_filter() will create positive/negative format amplitude filter */
/* coefficients ONLY. The function returns the coefficients in FILTER */

double * get_filter(filter, n, sign, nyquist, cutoff, rolloff, filter_units)
double filter[];
int n, sign;
double nyquist, cutoff, rolloff, filter_units;
{
    double freq_interval, active_freq;
    double half_power, exp_var, octaves;
    double filter_factor;
    int i,m;

    half_power=sqrt(0.5);
    filter_factor = log10(filter_units);
    m=n/2;
    freq_interval=nyquist/m;

    if(sign== (-1))
	filter[0]=1.0;
    else
	filter[0]=0.0;

    for(i=1;i<=m;i++)
    {
	active_freq=freq_interval*i;

	    /* number of octaves: */
	octaves=log10(active_freq/cutoff)/filter_factor;
	    /* 20=constant for dB formula as amplitude */
	exp_var=rolloff*octaves*sign/20.0;
	exp_var=pow(10.0,exp_var);
	filter[i]=exp_var*half_power;
	if(filter[i]>1.0)
	    filter[i]=1.0;
	if(filter[i]<MINVAL)
	filter[i]=0.0;
	if(sign==(-1) && filter[i]==0.0)
	    for( ;i<=m;i++)
		filter[i]=0.0;
	if(sign==1 && filter[i]==1.0)
	    for( ;i<=m;i++)
		filter[i]=1.0;
    }
    for(i=1;i<m;i++)
	filter[m+i]=filter[m-i];
    unscramble_transform(filter,n);

    return filter;

}





/* int_scale() is a routine to scale integer values to fit graph */
/* moved from herc.c 10/89 by T Clune */

int int_scale(in_val, in_min, in_max, out_min, out_max)

	    /* NOTICE THAT ALL PARAMETERS ARE INT */
	    /* in_val = number to be scaled */
	    /* in_min = smallest possible in_val */
	    /* in_max = largest possible in_val */
	    /* out_min = smallest acceptable scaling value */
	    /* out_max = largest acceptable scaling value */
	    /* to scale x-values to the screen, out_min will */
	    /* usually be larger than out_max.  This has the effect */
	    /* of making the screen appear to be first-quadrant */
	    /* instead of absolute value of fourth-quadrant. */


int in_val, in_min, in_max, out_min, out_max;

{
    int out_val;                /* return value variable */
    int largest, smallest;      /* out_min, out_max ordered by size */
    double scale_factor;        /* ratio of out:in ranges */



    scale_factor = (double)(out_max - out_min) / (in_max - in_min);
	 /* without the DOUBLE cast, scale_factor values would be INT */
	 /* because all rvalues in the expression are INT */

    out_val = (int)((in_val - in_min)*scale_factor + out_min);
	 /* out_val will be INT without the cast, but why take chances? */

    if(out_min > out_max)   /* define largest and smallest return values */
    {
	smallest = out_max;
	largest = out_min;
    }
    else
    {
	smallest = out_min;
	largest = out_max;
    }

	    /* make sure integerizing has not put out_val out of bounds */
    if(out_val > largest)
	    out_val = largest;

    if(out_val < smallest)
	    out_val = smallest;


    return out_val;

}







/* long_scale() is a routine to scale histogram long integer values */
/* to fit on a screen graph.  N.B.: Returns int, not long.          */
/* moved from herc.c 10/89 by T Clune */

int long_scale(in_val, in_min, in_max, out_min, out_max)

	    /* in_val = number to be scaled */
	    /* in_min = smallest possible in_val */
	    /* in_max = largest possible in_val */
	    /* out_min = smallest acceptable scaling value */
	    /* out_max = largest acceptable scaling value */
	    /* to scale y-values to the screen, out_min will */
	    /* usually be larger than out_max.  This has the effect */
	    /* of making the screen appear to be first-quadrant */
	    /* instead of absolute value of fourth-quadrant. */


long int in_val, in_min, in_max;
int out_min, out_max;

{
    int out_val;                /* return value variable */
    int largest, smallest;      /* out_min, out_max ordered by size */
    double scale_factor;        /* ratio of out:in ranges */


    if(in_val>=in_max)  /* kludgey patch for histarray[0] & [255] anomalies */
	return(out_max);

    scale_factor = (double)(out_max - out_min) / (in_max - in_min);
	 /* without the DOUBLE cast, scale_factor values would be INT */
	 /* because all r-values in the expression are INT */

    out_val = (int)((in_val - in_min)*scale_factor + out_min);
	 /* out_val will be INT without the cast, but why take chances? */

    if(out_min > out_max)   /* define largest and smallest return values */
    {
	smallest = out_max;
	largest = out_min;
    }
    else
    {
	smallest = out_min;
	largest = out_max;
    }

	    /* make sure integerizing has not put out_val out of bounds */
    if(out_val > largest)
		out_val = largest;

    if(out_val < smallest)
	    out_val = smallest;


    return out_val;

}





/* minmax() finds the min and max of the data set array.  n is the number of */
/* points in the data set */

void minmax(array,n,minval,maxval)
double array[];
int n;
double *minval, *maxval;
{
    int i;

    *minval= *maxval=array[0];
    for(i=1;i<n;i++)
    {
	if(array[i]< *minval)
	    *minval=array[i];
	if(array[i]> *maxval)
	    *maxval=array[i];
    }

}







/* polar_to_xy() converts the amplitude,phase data into x,y data. */
/* n is the number of data points. */

void polar_to_xy(amp, phase, x, y, n)
double amp[], phase[], x[], y[];
int n;
{
    int i;
    double t_amp, t_phase;
    for(i=0;i<n;i++)
    {
	t_amp=amp[i];   /* temp variables allow amp, phase to be x,y */
	t_phase=phase[i];
	x[i]=t_amp*cos(t_phase);
	y[i]=t_amp*sin(t_phase);
    }

}




/* unscramble_transform() places the negative frequency components at the */
/* beginning of the data array, so the x-axis of the graph is continuous */
/* calling this function twice will restore the data array to its original */
/* order.   If both amplitude & phase or x & y are used, call this function */
/* twice, once with each array as an argument.  */

void unscramble_transform(array, n)
double array[];
int n;
{
    int i, j;
    double temp;

    /* place negative freqs at beginning of fft array. */
    j=n/2;
    for(i=0;i<j;i++)
    {
	temp=array[i];
	array[i]=array[i+j];
	array[i+j]=temp;
    }

}




/* xy_to_polar() converts from rectangular coords to polar coords */
/* n is the number of points in each array.  MINVAL is the threshold */
/* value for an amplitude to be considered real.  It is used to decide */
/* whether the PHASE value is real or just noise.  If AMP[i] is <= MINVAL */
/* PHASE[i] is set to zero. xy_to_polar() allows x,y and amp,phase */
/* to be the same arrays. phase is an angle from 0 to 2*PI. */

void xy_to_polar(x, y, amp, phase, n, minval)
double x[], y[], amp[], phase[];
int n;
double minval;
{
    int i;
    double tx, ty;

    for(i=0;i<n;i++)
    {
	tx=x[i];    /* the temp variables allow AMP,PHASE to be X,Y */
	ty=y[i];
	amp[i]=sqrt(tx*tx+ty*ty);
	if(amp[i]<=minval)
	    phase[i]=0.0;
	else
	    phase[i]=atan2(ty,tx);

	    /* shift negative angles to 180 to 360 for compatibility with */
	    /* polar_to_xy() conversion format */

	if(phase[i]<0.0)
	    phase[i]=2*PI+phase[i];

    }

}
