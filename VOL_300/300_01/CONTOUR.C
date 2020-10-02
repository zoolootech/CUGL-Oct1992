/*   HEADER:   CUG300;
      TITLE:   Contour ploting demonstration;
       DATE:   4/20/1989;
DESCRIPTION:   "Plot contours using mat.lib functions";
    VERSION:   2.04;
   FILENAME:   CONTOUR.C;
   SEE-ALSO:   MAT_V2D.H;
*/

/*===================================================================*/
/* ROUTINES TO GENERATE CADD CONTOUR LINES FROM INPUT GRID VALUES    */
/*===================================================================*/
/*      0,1       1,1                                                */
/*   m4.-----------.m3            ^ 0,jub                            */
/*     | .       . |              |                                  */
/*     |   .   .   |              |                                  */
/*     |     .m0   |              |                                  */
/*     |    .  .   |              |                                  */
/*     |  .      . |              |                                  */
/*   m1|___________|m2           0,0 --------------> iub,0           */
/*     0,0        1,0                                                */
/*                                                                   */
/*   grid square layout            coordinate system layout          */
/*   -------------------           ------------------------          */
/*===================================================================*/

#include <stdio.h>
#include "mat_v2d.h"
#include <math.h>
#include <stdlib.h>
#include <conio.h>


#define  FALSE       0
#define  TRUE        1
#define  NO_ROWS     24
#define  NO_COLS     72

#define  xchk(x)     if(x<0)puts("x<0");if(x>NO_COLS)puts("x>NO_COLS")
#define  ychk(y)     if(y<0)puts("y<0");if(y>NO_COLS)puts("y>NO_COLS")

void  box          (float x1,float y1,float x2,float y2 );
void  dda          (float x1,float y1,float x2,float y2,char symbol);
void  display      (void);
void  erase        (void);
void  GridSelect   (int iub, int jub, int nc, struct fmat *x,
                    struct fmat *y,struct fmat *d, struct fmat *z);
void ContourSelect (int i,int j, int nc,struct fmat *x,
						  struct fmat *y,struct fmat *d,struct fmat *z,
						  int iub, int jub);
void drawit        (float x1, float x2, float y1, float y2,
                    struct fmat *x, struct fmat *y,int iub,int jub );
void set_graphics  (void);
void file_output   (void);


char  frame[NO_COLS][NO_ROWS];


/*============================================================================*/

main()

/*============================================================================*/

{
int i,i1,j,j1,k,iub,jub,nc,prmerr,num_rows,num_cols,token_size;
struct   fmat *x,*y,*d,*z;
struct   tmat *input_data;
char     filename[20];

/*       Input arrays and indexes                                             */

	printf("Enter contour data filename => ");
   scanf("%s",filename);
   mtcnt(filename,&num_rows,&num_cols,&token_size);    /* Get input file size */
   tdim(input_data,num_rows,num_cols,token_size);      /* Dimension input     */
   mtget(filename,input_data);                         /* array               */

	iub = no_toks(input_data,0);                        /* Get indices from    */
	jub = no_toks(input_data,1);                        /* the number of tokens*/
	nc  = no_toks(input_data,2);                        /* stored in the       */
                                                       /* appropriate input   */
   fdim(d,iub,jub+1);                                  /* data line header    */
   fdim(x,iub,DCLVCT);
   fdim(y,jub,DCLVCT);                                 /* Dimension needed    */
   fdim(z,nc,DCLVCT);                                  /* arrays              */

	for(i=0; i<iub; i++) {                              /* Store x grid loca-  */
      fck(x,i,VCT);                                    /* tion.               */
		f(x,i,VCT) = tf(input_data,0,i,iub);
   }
	for(j=0; j<jub; j++) {                              /* Store y grid loca-  */
      fck(y,j,VCT);                                    /* tions               */
		f(y,j,VCT) = tf(input_data,1,j,jub);
   }
   for(i=0; i<nc; i++) {                               /* Store contour       */
      fck(z,i,VCT);                                    /* levels to be        */
      f(z,i,VCT) = tf(input_data,2,i,nc);              /* plotted             */
   }
   for (j=3,k=jub-1; j<num_rows; j++,k--)  {
      for (i=0; i<iub; i++) {                          /* Store f(x,y) for    */
			fck(d,i,k);                                   /* each grid line      */
			f(d,i,k) = tf(input_data,j,i,iub);            /* intersection        */
      }
   }
   iub--;jub--;

/*       Check input parameters for validity                                  */

   prmerr = FALSE;                                     /* Test for no grid    */
	if (iub<=0 || jub<=0)                               /* intersections.      */
      prmerr = TRUE;                                   /*                     */
   if (nc <=0)                                         /* No contours         */
      prmerr = TRUE;                                   /* requested           */
   for (k = 1; k <= (nc-1); k++)                       /*                     */
   {                                                   /*                     */
      if (f(z,k,VCT) <= f(z,k-1,VCT) )                 /* Duplicate contours  */
         prmerr = TRUE;                                /*                     */
   }                                                   /*                     */
   if (prmerr)                                         /*                     */
   {                                                   /*                     */
      printf ("Error in input parameters");            /* Message if any      */
      return;                                          /* errors found        */
   }                                                   /*                     */
   set_graphics();                                     /* Initialize graphic  */
                                                       /* frame buffer        */
   GridSelect (iub, jub, nc, x, y, d, z );             /* Plot contours       */
                                                       /*                     */
   display();                                          /* Display all         */
                                                       /* contours plotted    */
   file_output();                                      /*                     */
                                                       /* Place contour in    */
                                                       /* an ASCII file       */
}                                                      /*                     */
                                                                              

/*============================================================================*/

void GridSelect  (   int iub, int jub, int nc, struct fmat *x,
                     struct fmat *y,struct fmat *d, struct fmat *z )

/*============================================================================*/
{
int i, j;


/*    Scan the input array top down & left to right                           */



   for  (j = (jub-1); j >= 0; j--)
   {
      for (i = 0; i <= (iub-1); i++)
      {
		ContourSelect (i,j,nc,x,y,d,z,iub,jub);          /* Select contour  to  */
      }                                                /* plot                */
   }
}

/*============================================================================*/

void ContourSelect ( int i,int j, int nc,struct fmat *x,
                     struct fmat *y, struct fmat *d,
                     struct fmat *z, int iub, int jub )

/*============================================================================*/

{

/*   Declare & initialize variables                                           */

float     h [5],    xh [5],     yh[5];

int  	ish[5];
int    caseval,k,i1,j1,m1, m2, m3;
int			m;
static int  im[4] =   { 0, 1, 1, 0 };
static int  jm[4] =   { 0, 0, 1, 1 };
static int  castab [3][3][3] =
                         { { { 0, 0, 8 },
                             { 0, 2, 5 },              /*   (k,i,j)       */
                             { 7, 6, 9 }, },           /*   -- i ---->    */
                                                       /*  | \            */  
                                 { { 0, 0, 4 },        /*      k          */  
                                   { 1, 0, 1 },        /*  j              */  
                                   { 4, 0, 0 }, },     /*  |       \      */  
                                                       /*  v         @    */  
                                        { { 9, 6, 7 },                        
                                          { 5, 2, 0 },                        
                                          { 8, 0, 0 }  }  };                  
                                                                              
float dmin, dmax, x1, y1, x2, y2;                                             
                                                                              
/*   Find the lowest vertex                                                   */
                                                                              
   if ( f(d,i,j) < f(d,i,j+1) )
      dmin = f(d,i,j);                                                        
   else dmin = f(d,i,j+1);
   if ( f(d,i+1,j)  < dmin )
      dmin = f(d,i+1,j);                                                      
   if ( f(d,i+1,j+1) < dmin )
      dmin = f(d,i+1,j+1);                                                    
                                                                              
/*   Find highest vertex                                                      */
                                                                              
   if ( f(d,i,j) > f(d,i,j+1) )
      dmax = f(d,i,j);
      else dmax = f(d,i,j+1);                                                 
   if ( f(d,i+1,j) > dmax)
      dmax= f(d,i+1,j);
   if (f(d,i+1,j+1) > dmax)
       dmax = f(d,i+1,j+1);                                                   
   if ( dmax < f(z,0,VCT) || dmin > f(z,nc-1,VCT) )
       return;  /* No contours to be plotted in grid */

/*   Draw each contour in grid square                                         */

   for (k = 0; k <= (nc-1); k++)
   {
      if (!( f(z,k,VCT) < dmin || f(z,k,VCT) > dmax ))
      {   /* Contour in grid */

/*   Determine line segment case                                              */

         for (m = 4; m >= 0; m--)
         {
            if ( m > 0 )
            {
               h[m] = f(d,(i+im[m-1]),(j+jm[m-1])) - f(z,k,VCT);
               xh[m]= f(x,i+im[m-1],VCT);
               yh[m]= f(y,j+jm[m-1],VCT);
            }
            if ( m==0 )
            {
               h[0] = (h[1]+h[2]+h[3]+h[4])/4;
               xh[0] = ( f(x,i,VCT) + f(x,i+1,VCT) )/2;
               yh[0] = ( f(y,j,VCT) + f(y,j+1,VCT) )/2;
            }

            if      ( h[m] > 0 )  ish[m]= 2;
            else if ( h[m] < 0 )  ish[m]= 0;
            else                  ish[m]= 1;

         } /* end m loop */

         for (m = 1; m <= 4; m++)
         {
            m1 =m;
            m2 = 0;
            m3 = m+1;
            if (m3 == 5) m3 = 1;
            caseval = castab [ish[m1]] [ish[m2]] [ish[m3]];
                                                                              
/*    Scan each triangle in grid square                                       */
                                                                              
            switch (caseval)
            {                                                                 
               case 0 : break;      /* No intersection */
                                                                              
               case 1 : x1 =xh[m1]; /* Line between vertices m1 and m2        */
                        y1=yh[m1];                                            
                        x2=xh[m2];                                            
                        y2=yh[m2];                                            
                        break;                                                
               case 2 : x1 =xh[m2]; /* Line between vertices m2 and m3        */
                        y1=yh[m2];                                            
                        x2=xh[m3];                                            
                        y2=yh[m3];                                            
                        break;                                                
               case 3 : x1 =xh[m3];/* Line between vertices m3 and m1         */
                        y1=yh[m3];                                            
                        x2=xh[m1];                                            
                        y2=yh[m1];                                              
                        break;                                                
               case 4 : x1=xh[m1]; /* Line between vertices m1 and side m2-m3 */
                        y1=yh[m1];                                            
                        x2=(h[m3]*xh[m2]-h[m2]*xh[m3])/(h[m3]-h[m2]);         
                        y2=(h[m3]*yh[m2]-h[m2]*yh[m3])/(h[m3]-h[m2]);         
                        break;                                                
               case 5 : x1=xh[m2]; /* Line between vertices m2 and side m3-m1 */
                        y1=yh[m2];                                            
                        x2=(h[m1]*xh[m3]-h[m3]*xh[m1])/(h[m1]-h[m3]);         
                        y2=(h[m1]*yh[m3]-h[m3]*yh[m1])/(h[m1]-h[m3]);         
                        break;                                                
               case 6 : x1=xh[m3]; /* Line between vertices m3 and side m1-m2 */
                        y1=yh[m3];                                            
                        x2=(h[m2]*xh[m1]-h[m1]*xh[m2])/(h[m2]-h[m1]);         
                        y2=(h[m2]*yh[m1]-h[m1]*yh[m2])/(h[m2]-h[m1]);         
                        break;                                                
                                   /*  Line between sides m1-m2 & m2-m3       */
                                                                              
               case 7 : x1 =(h[m2]*xh[m1]-h[m1]*xh[m2])/(h[m2]-h[m1]);        
                        y1 =(h[m2]*yh[m1]-h[m1]*yh[m2])/(h[m2]-h[m1]);        
                        x2 =(h[m3]*xh[m2]-h[m2]*xh[m3])/(h[m3]-h[m2]);        
                        y2 =(h[m3]*yh[m2]-h[m2]*yh[m3])/(h[m3]-h[m2]);        
                        break;                                                
                                   /*  Line between sides m2-m3 & m3-m1       */
                                                                              
               case 8 : x1 =(h[m3]*xh[m2]-h[m2]*xh[m3])/(h[m3]-h[m2]);        
                        y1 =(h[m3]*yh[m2]-h[m2]*yh[m3])/(h[m3]-h[m2]);        
                        x2 =(h[m1]*xh[m3]-h[m3]*xh[m1])/(h[m1]-h[m3]);        
                        y2 =(h[m1]*yh[m3]-h[m3]*yh[m1])/(h[m1]-h[m3]);        
                        break;                                                
                                   /*  Line between sides m3-m1 & m1-m2       */
                                                                              
               case 9 : x1 =(h[m1]*xh[m3]-h[m3]*xh[m1])/(h[m1]-h[m3]);        
                        y1 =(h[m1]*yh[m3]-h[m3]*yh[m1])/(h[m1]-h[m3]);        
                        x2 =(h[m2]*xh[m1]-h[m1]*xh[m2])/(h[m2]-h[m1]);        
                        y2 =(h[m2]*yh[m1]-h[m1]*yh[m2])/(h[m2]-h[m1]);        
                        break;                                               

			 default : printf("The defined case for vertex %d, contour %d\n",m,k);
                    printf("grid %d,%d does not exist\n", k,i,j);
            }                                                                 
            if (caseval!=0)
            {                                                                 
               drawit(x1,y1,x2,y2,x,y,iub,jub);                               
            }                                                                 
         } /* Loop m */                                                       
      } /* if */                                                              
   } /* Loop k */                                                             
}
                                                                              
/*============================================================================*/
                                                                              
void drawit (float x1, float y1, float x2, float y2,                          
             struct fmat *x, struct fmat *y,int iub, int jub )
                                                                              
/*============================================================================*/
{                                                                             
   x1 = (NO_COLS-3)*x1/(f(x,iub,VCT))+1;               /* Draw the selected   */
   y1 = (NO_ROWS-3)*y1/(f(y,jub,VCT))+1;               /* line segment in     */
   x2 = (NO_COLS-3)*x2/(f(x,iub,VCT))+1;               /* the frame buffer    */
   y2 = (NO_ROWS-3)*y2/(f(y,jub,VCT))+1;                                      
                                                                              
   dda(x1,y1,x2,y2,'+');                                                      
}                                                                             
                                                                              
/*============================================================================*/
                                                                              
void set_graphics(void)
                                                                              
/*============================================================================*/
{                                                                             
erase();                                               /* Clear the fuffer    */
box(0,0,NO_COLS-1,NO_ROWS-1);                          /* Draw box in frame   */
}                                                      /* buffer              */
                                                                              
/*===========================================================================*/
                                                                              
void  box(float x1,float y1,float x2,float y2 )                               
                                                                              
/*===========================================================================*/
                                                                              
{                                                                             
   xchk(x1);xchk(x2);ychk(y1);ychk(y2);                /* Draw box with ASCII */
   frame[x1][y1]       ='*';                           /* characters in       */
   dda(x1,y1+1, x1,y2-1,'|');                          /* the graphic frame   */
   frame[x1][y2]       ='*';                                                  
   dda(x1+1,y2, x2-1,y2,'-');                                                 
   frame[x2][y2]       ='*';                                                  
   dda(x2,y2-1, x2,y1+1,'|');                                                 
   frame[x2][y1]       ='*';                                                  
   dda(x2-1,y1, x1+1,y1,'-');                                                 
}                                                                             
                                                                              
/*===========================================================================*/
                                                                              
void  dda (float x1,float y1,float x2,float y2,char symbol)                   
                                                                              
/*===========================================================================*/
{                                                                             
float    x,y,dx_step,dy_step;                           /* Draw a line in    */
int steps,i,dx,dy;                                 /* the bufffer       */
                                                                              
   dx=x2-x1;                              /* Calc deltas */                   
   dy=y2-y1;                                                                  
   steps=max(abs(dx),abs(dy))+1;          /* Calc no steps */
   dx_step=(float)dx/steps;               /* Step size */
   dy_step=(float)dy/steps;
   x=x1+.5;                               /* Offsets for rounding */
   y=y1+.5;
   for (i=0; i<steps; i++) {              /* Set pixels */
      frame[floor(x)][floor(y)]=symbol;
      x+=dx_step;
      y+=dy_step;
   }
   frame[floor(x)][floor(y)]=symbol;
                                          /* Last pixel */
}

/*===========================================================================*/

void  display (void)

/*===========================================================================*/
{
int        x,y;                                        /* Display the frame  */
                                                       /* buffer on the std  */
   clrscr();                                           /* output device      */
   for (y=NO_ROWS-1; y>=0; y--) {
      for (x=0; x<NO_COLS; x++) {
         putch(frame[x][y]);
      }
      putch('\n');
      putch('\r');
   }
}
                                                                              
/*===========================================================================*/
                                                                              
void  erase (void)

/*===========================================================================*/
{
int   x,y;
                                                       /* Clear the frame    */
   for(y=NO_ROWS; y>0; y--) {                          /* buffer             */
      for (x=1; x<=NO_COLS; x++) {
         frame[x][y]='\x20';
      }
   }
}

/*===========================================================================*/

void  file_output (void)

/*===========================================================================*/
{
char ch,filename[24];
int   x,y;
FILE  *FN;


   printf("ASCII file copy desired ? (Y/N) => ");ch=getche();

   if (ch=='Y'||ch=='y') {                             /* Output the frame   */
                                                       /* buffer to a text-  */
      clrscr();                                        /* file               */
      printf("Enter the output filename desired:");
      scanf("%s",filename);

      FN=fopen(filename,"w");

      for (y=NO_ROWS-1; y>=0; y--) {
         for (x=0; x<NO_COLS; x++) {
            putc(frame[x][y],FN);
         }
      putc('\n',FN);
      }
      fclose(FN);
   }
}

