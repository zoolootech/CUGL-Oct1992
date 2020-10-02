
#ifndef FFT_H
#define FFT_H


#define PI 3.14159265358979
#define FORWARD -1
#define INVERSE 1
	/* the filter option defines a filter from 1.0 to MINVAL.  All */
	/* amplitudes below MINVAL are set to 0.0 */
#define MINVAL 0.00001

void fft(), xy_to_polar(), polar_to_xy();
void unscramble_transform(), minmax(), data_multiply();
double *get_filter();
int dbl_scale(), int_scale(), long_scale();

#endif
