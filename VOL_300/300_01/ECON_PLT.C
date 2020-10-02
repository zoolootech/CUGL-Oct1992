#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <dos.h>
#include <ctype.h>
#include <stdlib.h>
#include "gr_lib.h"
#include "mat_v2d.h"

#define     VECTOR      1
#define     DATE			0
#define     POINTS      1
#define     MOVE_AVE    2
#define     CUMUL_TOT   3

float time (char date[]);

main()

{
unsigned	  rows,cols,i,j,k,toksize,move_ave,cumul_tot,loops,nostgs,no_pds;
char    xtitle[80],filename[30];
struct  tmat    *data,*headings,*date;
struct  fmat	 *dataf,*plot_data;
float   date_min,date_max,plot_max,plot_min,tic;
string80 *ticstgs;

	printf("Please enter the name of the file to plot => ");
	scanf("%s",filename);
	printf("Please enter the number of periods to be summed => ");
	scanf("%d",&no_pds);

   mtcnt(filename,&rows,&cols,&toksize);
   move_ave = cols-1;
   cumul_tot = cols;
   tdim(data,rows,cols,toksize);
   tdim(headings,VECTOR,cols,toksize);
   fdim(dataf, rows-1, cols+1);
	fdim(plot_data,4,rows-1);

	mtget(filename,data);

   all(data,i,j) {

		if (i==0) {
      	tck(headings,i,j,toksize);
			strcpy ( t(headings,i,j), ts(data,i,j,cols) );
		}
      else {
			if (j==0) {
				fck(plot_data,DATE,i-1);
				f(plot_data,DATE,i-1) =
							(time( ts(data,i,j,cols) )/31536000)+1970;
			}
			else {
				fck(dataf,i-1,j-1);
				f(dataf,i-1,j-1) = tf(data,i,j,cols);
         }
      }
   }

	loops = no_cols(dataf)-3;
	j=1;
	do {
		mfmvac (dataf, no_pds, j, 0, no_rows(dataf)-1, move_ave);
		mfcumc (dataf,j,(no_rows(dataf)-no_pds),no_rows(dataf)-1,
			cumul_tot);
		rows(dataf,i) {
			fck(dataf,i,move_ave);
			f(dataf,i,move_ave)=f(dataf,i,move_ave)*no_pds;
		}

	     cols(plot_data,k) {

			fck(plot_data,POINTS,k);
			fck(dataf,k,j);
			fck(plot_data,MOVE_AVE,k);
			fck(dataf,k,move_ave);
			fck(plot_data,CUMUL_TOT,k);
			fck(dataf,k,cumul_tot);

			f(plot_data,POINTS,k) 		= f(dataf,k,j);
			f(plot_data,MOVE_AVE,k) 	= f(dataf,k,move_ave);
			f(plot_data,CUMUL_TOT,k) 	= f(dataf,k,cumul_tot);

		}
		if(registerbgidriver(CGA_driver)<0) {
			puts("Cannot locate CGA_driver");
			exit(0);
		}
		InitSEGraphics("a:\\");
		SetCurrentWindow(2);
		SetAxesType(0,0);
		SelectColor(3);
		FindMinMax(&f(plot_data,DATE,0),no_cols(plot_data),
			&date_min,&date_max );
		date_min=floor(date_min);
		FindMinMax(&f(plot_data,MOVE_AVE,0),no_cols(plot_data),
			&plot_min,&plot_max );
		if (plot_min>0.0) plot_min=0.0;
		ScalePlotArea(date_min,plot_min,date_max,plot_max);
		SetXYIntercepts(date_min,plot_min);
		DrawXAxis(1,0);
		LabelXAxis(1,0);
		tic=(plot_max-plot_min)/10;
		DrawYAxis(tic,0);
		TitleXAxis("DATE");
		LabelYAxis(1,0);
		BorderCurrentWindow(2);
		TitleWindow(t(headings,0,j));
		DrawGrid(4);

		LinePlotData(&f(plot_data,DATE,no_pds),
			&f(plot_data,MOVE_AVE,no_pds),
			no_cols(plot_data)-no_pds, 3, 0);

		LinePlotData(&f(plot_data,DATE,no_cols(plot_data)-no_pds),
			&f(plot_data,CUMUL_TOT,no_cols(plot_data)-no_pds),
			no_pds, 3, 0);

		LinePlotData(&f(plot_data,DATE,0),
			&f(plot_data,POINTS,0),
			no_cols(plot_data), 3, 0);
		getch();
		ClearWindow();
		CloseSEGraphics();

	} while (j++<loops);
 }


/*====================================================================*/

float time (char date_stg[])

/*====================================================================*/
{
char *end,temp[2],mon,day;
signed int  year;
struct date d1;
struct time t1;

   strncpy(temp,&date_stg[0],2);
   if (isdigit(temp[0]) && isdigit(temp[1]) )
		year = (int) strtol (temp,&end,10)+1900;
   else year = 0;

   strncpy(temp,&date_stg[2],2);
   if (isdigit(temp[0]) && isdigit(temp[1]) )
		mon = (char) strtol(temp,&end,10);
   else mon = 0;

   end = NULL;
   strncpy(temp,&date_stg[4],2);
   if (isdigit(temp[0]) && isdigit(temp[1]) )
		day = (char) strtol (temp,&end,10);
   else day = 0;

   d1.da_year = year;
   d1.da_day  = day;
   d1.da_mon  = mon;

   t1.ti_min  = 0;
   t1.ti_hour = 0;
   t1.ti_hund = 0;
   t1.ti_sec  = 0;

return (float) dostounix (&d1,&t1);
}
