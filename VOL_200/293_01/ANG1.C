/********************  ang.c  ******************************

   Three dimensional surface reconstruction program

   This program assumes that the views from the main axis
   directions were done thus an angle view between two main
   axis views can be created from the depth_code viewa(floating
   point). 

 Daniel Geist
 Michael W. Vannier

 Mallinckrodt Institute of Radiology
 Washington University School of Medicine
 510 S. Kingshighway Blvd.
 St. Louis, Mo. 63110

 Note:  This program is intended for the private non-commercial
 use of interested individuals to provide a fuller explanation of
 the algorithms described in "Three dimensional reconstruction in
 Medical Imaging" by D. Geist and M. Vannier.

 1988

 A.Wallin
 -Command line input
 -Threshold on gradient shading, background not used for gradient.
 -Combination of gradient and distance is possible.
 -Distance or gradient image turned off
 -Straight views (0, 90, 180 etc) possible. (0 = ydis2.dat, 90 = xdis1.dat
  180 = ydis1.dat, 270 = xdis2.dat)
 -Incremental views possible
 -Views always head up
 *************************************************************/

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#define FLOAT_LINE 256*sizeof(float)
#define PI 3.141592653

/* the structure below is the data for a point on the surface projected
   on the view plane                        */
typedef struct DIS_REC {
      float dist;            /*distance from view plane */
      int indXY;             /* index of data on main axis view */
      char XY;               /* Which main axis view obtained from (X or Y)*/
};

struct DIS_REC distances[256]; /* one projected line */

int NLINES,IXmax,IYmax,xinc,yinc;
int (*xdir)(),(*ydir)();
double THETA,cosTheta,sinTheta,tgnTheta;
float fxbuf[3][256],fybuf[3][256];/* input buffers */

char *usestr = "Usage: ang [-a] [-r] [-g] [-n(d|g)] [-h] [-d]";
float GRAD_THRESHOLD = 10.0;
int  number_pic = 1;
int  dispmode = 3;
float dist_weight;

/*  output files  */
char fng[13],fnd[13],xfile[13],yfile[13],DR;

succ(i)
int i;
{return(i==2?0:i+1);}

prev(i)
int i;
{return(i==0?2:i-1);}

forward(i)
int i;
{ return(i); }

backward(i,start)
int i;
{return(start-i); }

/* PUTD - fill a DIS_REC with values */
putd(pdis,D,i,xy_sym)
struct DIS_REC *pdis;
float D;
int i,xy_sym;

{ pdis->dist=D;
  pdis->indXY=i;
  pdis->XY=xy_sym;
}

/* take one line from X and Y views and create a projrcted line */
getdistances(linex,liney)
float linex[],liney[];

{ int i,IND,X,Y;
  float D;
  float Dhole;
  float INDf;
  int   holex, holey;
  float dx1, dx2, dx;
  float dy1, dy2, dy;
  float temp1, temp2;

  for(i=0;i<256;i++) distances[i].XY=0;
  /* project Y-data onto image line */
  for(i=0;i<256;i++) if(liney[i] < 256.0){
       X=(*ydir)(i,255);
       INDf=IXmax -(X*sinTheta-liney[i]*cosTheta);
       D=liney[i]/sinTheta+(X-liney[i]/tgnTheta)*cosTheta;
       /*  Do the two nearest neighbours */
       IND = INDf;
       if((IND>=0) && (IND<256)){
           if(distances[IND].XY==0)putd(&distances[IND],D,i,1);
           else if(distances[IND].dist>D)putd(&distances[IND],D,i,1);
       }
       IND = INDf + 0.5;
       if((IND>=0) && (IND<256)){
           if(distances[IND].XY==0)putd(&distances[IND],D,i,1);
           else if(distances[IND].dist>D)putd(&distances[IND],D,i,1);
       }
  }

  /*project X-data onto image plane */
 for(i=0;i<256;i++) if(linex[i] < 256.0){
       Y=(*xdir)(i,255);
       D=Y/sinTheta+(linex[i]-Y/tgnTheta)*cosTheta;
       INDf=IXmax-(linex[i]*sinTheta-Y*cosTheta);
       /*  Do the two nearest neighbours */
       IND = INDf;
       if((IND>=0) && (IND<256)){
           if(distances[IND].XY==0)putd(&distances[IND],D,i,2);
           else if(distances[IND].dist>D)putd(&distances[IND],D,i,2);
       }
       IND = INDf + 0.5;
       if((IND>=0) && (IND<256)){
           if(distances[IND].XY==0)putd(&distances[IND],D,i,2);
           else if(distances[IND].dist>D)putd(&distances[IND],D,i,2);
       }
  }

  /* fill holes due to low resolution */
  if (dispmode & 4)
    /* test neighbours */
    for(i=1;i<255;i++)
      if( (distances[i].XY==0) && (distances[i+1].XY!=0) &&
            (distances[i-1].XY!=0))
             putd(&distances[i],distances[i-1].dist,
                   distances[i-1].indXY,(int)distances[i-1].XY);
                   
}

/* returns gradient shade in point given the variations of the surface */
unsigned char grad(x1,x2,y1,y2,z1,z2,x_fac,y_fac,z_fac)
float x1,x2,y1,y2,z1,z2;
int x_fac,y_fac,z_fac;
{float gx,gy,gz,G,nx,ny;
 unsigned char gxint;
     /* components of gradient */
  gx=(x2-x1)/x_fac;
  gy=(y2-y1)/y_fac;
  gz=(z1-z2)/z_fac;
  G=sqrt(gx*gx+gy*gy+gz*gz);
     /*compute nx,ny normalized x,y component of gradient */
  nx=gx/G;
  ny=gy/G;
  gxint=255*(nx*cosTheta+ny*sinTheta)+0.5; /*scale gradient shade by 256 */
  return(gxint);
}

doline(linex,linex1,linex2,liney,liney1,liney2,z_fac,fg,fd)
float linex[],linex1[],linex2[],liney[],liney1[],liney2[];
int z_fac;
FILE *fg,*fd;

{ int i;
  unsigned char lined[256],lineg[256];
  float x1, x2, y1, y2, z1, z2;
  int   x_fac, y_fac;

      /* empty bit on image line */
  for(i=0;i<256;i++)
    if (distances[i].XY==0)
      lineg[i]=lined[i]=0;
    else {
      if (dispmode & 1)
        lined[i]=(distances[i].dist<256)?255-distances[i].dist+0.5:0;
      /* bit on image line projected from Y view */
      if (dispmode & 2) {
        if(distances[i].XY==1) switch(distances[i].indXY){
           case 0:
             lineg[i]=
                   grad(liney[1]*yinc,liney[0]*yinc,(float)0,(float)2,
                        liney1[0],liney2[0],1,2,z_fac);
             break;
           case 255:
             lineg[i]=
                    grad(liney[255]*yinc,liney[254]*yinc,(float)0,(float)2,
                         liney1[255],liney2[255],1,2,z_fac);
             break;
           default:
             x1 = liney[distances[i].indXY+yinc];
             x2 = liney[distances[i].indXY-yinc];
             x_fac = 2;
             y1 = 0.0;
             y2 = 2.0;
             y_fac = 2;
             z1 = liney1[distances[i].indXY];
             z2 = liney2[distances[i].indXY];
             if (x1 > 255.0) {
               x1 = liney[distances[i].indXY];
               x_fac = 1;
             }
             else if (x2 > 255.0) {
               x2 = liney[distances[i].indXY];
               x_fac = 1;
             }
             else if (fabs(x2 - x1) > GRAD_THRESHOLD) {
               if (fabs(liney[distances[i].indXY] - x1) < GRAD_THRESHOLD/2.0 &&
                   x2 < liney[distances[i].indXY]) {
                 x2 = liney[distances[i].indXY];
                 x_fac = 1;
               }
               else if (fabs(x2 - liney[distances[i].indXY]) <
                        GRAD_THRESHOLD/2.0 && x1 < liney[distances[i].indXY]) {
                 x1 = liney[distances[i].indXY];
                 x_fac = 1;
               }
             }
             if (z1 > 255.0) {
               z1 = liney[distances[i].indXY];
               z_fac = 1;
             }
             else if (z2 > 255.0) {
               z2 = liney[distances[i].indXY];
               z_fac = 1;
             }
             else if (fabs(z2 - z1) > GRAD_THRESHOLD) {
               if (fabs(liney[distances[i].indXY] - z1) < GRAD_THRESHOLD/2.0 &&
                 z2 < liney[distances[i].indXY]) {
                 z2 = liney[distances[i].indXY];
                 z_fac = 1;
               }
               else if (fabs(z2 - liney[distances[i].indXY]) <
                        GRAD_THRESHOLD/2.0 && z1 < liney[distances[i].indXY]) {
                 z1 = liney[distances[i].indXY];
                 z_fac = 1;
               }
             }
             lineg[i]= grad(x1, x2, y1, y2, z1, z2, x_fac, y_fac, z_fac);
             break;
        }
        /* bit on image line projected from X view */
        else 
          switch(distances[i].indXY){
           case 0:
             lineg[i]=
                   grad((float)0,(float)2,linex[1]*xinc,linex[0]*xinc,
                        linex1[0],linex2[0],2,1,z_fac);
             break;
           case 255:
             lineg[i]=
                    grad((float)0,(float)2,linex[255]*xinc,linex[254]*xinc,
                         linex1[255],linex2[255],2,1,z_fac);
             break;
           default:
             x1 = 0.0;
             x2 = 2.0;
             x_fac = 2;
             y1 = linex[distances[i].indXY+xinc];
             y2 = linex[distances[i].indXY-xinc];
             y_fac = 2;
             z1 = linex1[distances[i].indXY];
             z2 = linex2[distances[i].indXY];
             if (y1 > 255.0) {
               y1 = linex[distances[i].indXY];
               y_fac = 1;
             }
             else if (y2 > 255.0) {
               y2 = linex[distances[i].indXY];
               y_fac = 1;
             }
             else if (fabs(y2 - y1) > GRAD_THRESHOLD) {
               if (fabs(linex[distances[i].indXY] - y1) < GRAD_THRESHOLD/2.0 &&
                 y2 < linex[distances[i].indXY]) {
                 y2 = linex[distances[i].indXY];
                 y_fac = 1;
               }
               else if (fabs(y2 - linex[distances[i].indXY]) <
                        GRAD_THRESHOLD/2.0 && y1 < linex[distances[i].indXY]) {
                 y1 = linex[distances[i].indXY];
                 y_fac = 1;
               }
             }
             if (z1 > 255.0) {
               z1 = linex[distances[i].indXY];
               z_fac = 1;
             }
             else if (z2 > 255.0) {
               z2 = linex[distances[i].indXY];
               z_fac = 1;
             }
             else if (fabs(z2 - z1) > GRAD_THRESHOLD) {
               if (fabs(linex[distances[i].indXY] - z1) < GRAD_THRESHOLD/2.0 &&
                 z2 < linex[distances[i].indXY]) {
                 z2 = linex[distances[i].indXY];
                 z_fac = 1;
               }
               else if (fabs(z2 - linex[distances[i].indXY]) <
                        GRAD_THRESHOLD/2.0 && z1 < linex[distances[i].indXY]) {
                 z1 = linex[distances[i].indXY];
                 z_fac = 1;
               }
             }
             lineg[i]= grad(x1, x2, y1, y2, z1, z2, x_fac, y_fac, z_fac);
             break;
          }
        if (dispmode & 8) {
          /* combine with distance */
          lineg[i] = (float) lineg[i] * (1.0 - dist_weight) +
                     ((distances[i].dist < 255.0) ? 
                        (255.0-distances[i].dist) : 0.0) * dist_weight;
        }

      }
  }

  if (fg != NULL) fwrite(lineg,sizeof(unsigned char),256,fg);
  if (fd != NULL) fwrite(lined,sizeof(unsigned char),256,fd);
}





dostrline(linex,linex1,linex2,z_fac,fg,fd)
float linex[],linex1[],linex2[];
int z_fac;
FILE *fg,*fd;

{ int i;
  unsigned char lined[256],lineg[256];
  float x1, x2, y1, y2, z1, z2;
  int   x_fac, y_fac;

  if(linex[0] > 255.0)
    lineg[0] = lined[0] = 0;
  else {
    if (dispmode & 1)
      lined[0] = 255.5 - linex[0];
    if (dispmode & 2) {
      lineg[0] = grad(0.0, 2.0, linex[0], linex[1], linex1[0], linex2[0],
                      1, 2, z_fac);
      if (dispmode & 8) {
        lineg[i] = (float) lineg[0] * (1.0 - dist_weight) + 
                   (255.0-linex[0]) * dist_weight;
        }
    }
   }

   for(i = 1; i < 255; i++)
    if(linex[i] > 255.0)
      lineg[i] = lined[i] = 0;
    else {
      if (dispmode & 1)
	    lined[i] = 255.5 - linex[i];
      if (dispmode & 2) {
        x1 = 0.0;
        x2 = 2.0;
        x_fac = 2;
        y1 = linex[i-1];
        y2 = linex[i+1];
        y_fac = 2;
        z1 = linex1[i];
        z2 = linex2[i];
        if (y1 > 255.0) {
          y1 = linex[i];
          y_fac = 1;
        }
        else if (y2 > 255.0) {
          y2 = linex[i];
          y_fac = 1;
        }
        else if (fabs(y2 - y1) > GRAD_THRESHOLD) {
          if (fabs(linex[i] - y1) < GRAD_THRESHOLD/2. && y2 < linex[i]) {
            y2 = linex[i];
            y_fac = 1;
          }
          else if ((y2 - linex[i]) < GRAD_THRESHOLD/2. && y1 > linex[i]) {
            y1 = linex[i];
            y_fac = 1;
          }
        }
        if (z1 > 255.0) {
          z1 = linex[i];
          z_fac = 1;
        }
        else if (z2 > 255.0) {
          z2 = linex[i];
          z_fac = 1;
        }
        else if (fabs(z2 - z1) > GRAD_THRESHOLD) {
          if (fabs(linex[i] - z1) < GRAD_THRESHOLD/2. && z2 < linex[i]) {
            z2 = linex[i];
            z_fac = 1;
          }
          else if (fabs(z2 - linex[i]) < GRAD_THRESHOLD/2. && z1 < linex[i]) {
            z1 = linex[i];
            z_fac = 1;
          }
        }
        lineg[i] = grad(x1, x2, y1, y2, z1, z2, x_fac, y_fac, z_fac);
        if (dispmode & 8) {
          /* combine with distance */
          lineg[i] = (float) lineg[i] * (1.0 - dist_weight) + 
                     (255.0-linex[i]) * dist_weight;
        }
      }
    }

  if (linex[255] > 255.0)
    lineg[255] = lined[255] = 0;
  else {
    if (dispmode & 1)
      lined[255] = 255.5 - linex[255];
    if (dispmode & 2) {
      lineg[255] = grad(0.0, 2.0, linex[254], linex[255], linex1[255], linex2[255],
                        1, 2, z_fac);
      if (dispmode & 8) {
        lineg[i] = (float) lineg[255] * (1.0 - dist_weight) + 
                   (255.0-linex[255]) * dist_weight;
        }

    }
   }


  if (fg != NULL) fwrite(lineg,sizeof(unsigned char),256,fg);
  if (fd != NULL) fwrite(lined,sizeof(unsigned char),256,fd);

}




doviews()
{
 FILE *fg,*fd,*fx,*fy;
 int z,i,j,k,mid;

 mid=1;

 fd = fg = NULL;
 if(dispmode & 1) {
   if ((fd=fopen(fnd,"wb")) == NULL) {
     printf("Error opening %s\n", fnd);
     exit(1);
   }
   printf("Created %s\n", fnd);
 }
 if (dispmode & 2) {
   if ((fg=fopen(fng,"wb")) == NULL) {
     printf("Error opening %s\n", fng);
     exit(1);
   }
   printf("Created %s\n", fng);
 }
 if ((fx=fopen(xfile,"rb")) == NULL || (fy=fopen(yfile,"rb")) == NULL) {
   printf("Error opening %s or %s\n", xfile, yfile);
   exit(1);
 }

/*read first three lines */
 for(i=0;i<3;i++){
     fseek(fx,(long)FLOAT_LINE*i,SEEK_SET);
     fread(fxbuf[i],1,FLOAT_LINE,fx);
     fseek(fy,(long)FLOAT_LINE*i,SEEK_SET);
     fread(fybuf[i],1,FLOAT_LINE,fy);
 }

 /* do first line (forward differene)*/
 getdistances(fxbuf[0],fybuf[0]);
 doline(fxbuf[0],fxbuf[0],fxbuf[1],fybuf[0],fybuf[0],fybuf[1],1,fg,fd);

 /* do middle lines (central diffrence) */
 for(z=0;z<(NLINES-2);z++){      /*for each slice */
     getdistances(fxbuf[mid],fybuf[mid]);
     doline(fxbuf[mid],fxbuf[prev(mid)],fxbuf[succ(mid)],
            fybuf[mid],fybuf[prev(mid)],fybuf[succ(mid)],2,fg,fd);
     fseek(fx,(long)FLOAT_LINE*i,SEEK_SET);
     fseek(fy,(long)FLOAT_LINE*i,SEEK_SET);
     i++;
     fread(fxbuf[prev(mid)],1,FLOAT_LINE,fx);
     fread(fybuf[prev(mid)],1,FLOAT_LINE,fy);
     mid=succ(mid);
     printf(" line %d\r", z+2);
 }
 printf("\n");

 /* do last line (backward difference)*/
 getdistances(fxbuf[mid],fybuf[mid]);
 doline(fxbuf[mid],fxbuf[prev(mid)],fxbuf[prev(mid)],
            fybuf[mid],fybuf[prev(mid)],fybuf[prev(mid)],1,fg,fd);

 if (fg != NULL) fclose(fg);
 if (fd != NULL) fclose(fd);
 fclose(fx);
 fclose(fy);
}

dostrview()
{
 FILE *fg,*fd,*fx;
 int z,i,j,k,mid;

 mid=1;

 fd = fg = NULL;
 if(dispmode & 1) {
   if ((fd=fopen(fnd,"wb")) == NULL) {
     printf("Error opening %s\n", fnd);
     exit(1);
   }
   printf("Created %s\n", fnd);
 }
 if (dispmode & 2) {
   if ((fg=fopen(fng,"wb")) == NULL) {
     printf("Error opening %s\n", fng);
     exit(1);
   }
   printf("Created %s\n", fng);
 }
 if ((fx=fopen(xfile,"rb")) == NULL) {
   printf("Error opening %s\n", xfile);
   exit(1);
 }

/*read first three lines */
 for(i=0;i<3;i++) {
   fseek(fx,(long)FLOAT_LINE*i,SEEK_SET);
   fread(fxbuf[i],1,FLOAT_LINE,fx);
 }

 /* do first line (forward differene)*/
 dostrline(fxbuf[0], fxbuf[0], fxbuf[1], 1, fg, fd);

 /* do middle lines (central diffrence) */
 for(z=0;z<(NLINES-2);z++){      /*for each slice */
     dostrline(fxbuf[mid],fxbuf[prev(mid)],fxbuf[succ(mid)],2,fg,fd);
     fseek(fx,(long)FLOAT_LINE*i,SEEK_SET);
     i++;
     fread(fxbuf[prev(mid)],1,FLOAT_LINE,fx);
     mid=succ(mid);
     printf(" line %d\r",z+2);
 }
 printf("\n");

 /* do last line (backward difference)*/
 dostrline(fxbuf[mid],fxbuf[prev(mid)],fxbuf[prev(mid)],
           1,fg,fd);

 if (fg != NULL) fclose(fg);
 if (fd != NULL) fclose(fd);
 fclose(fx);
}

getang(ang)
int ang;
{
  sprintf(fng,"gang%03d.out",ang);
  sprintf(fnd,"dang%03d.out",ang);
  if( ( ang > 0) && ( ang < 90) ){
       xdir=forward;
       ydir=backward;
       sprintf(xfile,"%c:ydis2.dat",DR);
       sprintf(yfile,"%c:xdis1.dat",DR);
       xinc=1;
       yinc=-1;
   }
   else if( ( ang > 90) && ( ang < 180) ){
       xdir=forward;
       ydir=backward;
       sprintf(xfile,"%c:xdis1.dat",DR);
       sprintf(yfile,"%c:ydis1.dat",DR);
       xinc=1;
       yinc=-1;
       ang-=90;
   }
   else if( ( ang > 180) && ( ang < 270) ){
       xdir=forward;
       ydir=backward;
       sprintf(xfile,"%c:ydis1.dat",DR);
       sprintf(yfile,"%c:xdis2.dat",DR);
       xinc=1;
       yinc=-1;
       ang-=180;
   }
   else if( ( ang > 270) && ( ang < 360) ){
       xdir=forward;
       ydir=backward;
       sprintf(xfile,"%c:xdis2.dat",DR);
       sprintf(yfile,"%c:ydis2.dat",DR);
       xinc=1;
       yinc=-1;
       ang-=270;
   }
   else if (ang == 0) {
       xdir=forward;
       sprintf(xfile,"%c:ydis2.dat",DR);
       xinc=1;
       cosTheta = 1.0;
       sinTheta = 0.0;
       return(0);
   }
   else if (ang == 90) {
       xdir=forward;
       sprintf(xfile,"%c:xdis1.dat",DR);
       xinc=1;
       cosTheta = 1.0;
       sinTheta = 0.0;
       return(0);
   }
   else if (ang == 180) {
       xdir=forward;
       sprintf(xfile,"%c:ydis1.dat",DR);
       xinc=1;
       cosTheta = 1.0;
       sinTheta = 0.0;
       return(0);
   }
   else if (ang == 270) {
       xdir=forward;
       sprintf(xfile,"%c:xdis2.dat",DR);
       xinc=1;
       cosTheta = 1.0;
       sinTheta = 0.0;
       return(0);
   }
   else {
       printf(" Bad angle\n");
       exit(1);
  }
  IXmax = 127.5 * (1.0 + sqrt(2.0) * sin(((double) ang - 45.0) * PI / 180.0));
  THETA=(double)ang * PI/180;
  sinTheta=sin(THETA);
  cosTheta=cos(THETA);
  tgnTheta=sinTheta/cosTheta;
}
 
/**********************************************************/
/**** MAIN ***** MAIN ***** MAIN ***** MAIN ***** MAIN ****/
/**********************************************************/
/* Command line input:
   ang [-a] [-r] [-g] [-n(d|g)] [-h] [-d]
   -a  : angle (no default)
   -r  : number of output images (default: 1)
   -g  : threshold for gradient (default: 3.0)
   -n  : not making distance (d) or gradient (g) image
   -h  : filling holes
   -d  : weight for distance combined with the gradient
 */

main(argc, argv)
int argc;
char *argv[];

{
  char filname[13];
  FILE *par;
  int ANG;
  int i;

 /* first get some parameters */
 if ((par=fopen("param.dat","r")) == NULL) {
   printf("Error opening PARAM.DAT\n");
   exit(1);
 }
 fscanf(par,"%c %d",&DR,&NLINES);
 fclose(par);
 if (argc != 1) {
   for (argc--; argc > 0; argc--) {
     if (**++argv == '-' || **argv == '/') {
       switch (tolower(*(*argv+1))) {
         case 'a':
           ANG = atoi(*argv+2);
           break;
         case 'r':
           number_pic = atoi(*argv+2);
           break;
         case 'g':
           GRAD_THRESHOLD = atof(*argv+2);
           break;
         case 'd':
           dispmode |= 8;
           if (*(*argv+2) != '\0')
             if ((dist_weight = atof(*argv+2)) < 0.0 || dist_weight > 1.0)
               dist_weight = 0.5;
           break;
         case 'n':
           switch (tolower(*(*argv+2))) {
             case 'd':
               dispmode &= 0xfe;
               break;
             case 'g':
               dispmode &= 0xfd;
               break;
             default:
               usage();
           }
           break;
           case 'h':
             dispmode |= 4;
             break;
         default:
           usage();         
       }
     }
     else
       usage();
   }
 }
 else {
   printf("Enter angle: ");
   scanf("%d", &ANG);
   if (ANG != 0) {
     printf("Enter number of rotations: ");
     scanf("%d", number_pic);
   }
 }

 if (number_pic <= 0) {
   printf("number_pic should be larger than 0!\n");
   exit(1);
 }
 if (GRAD_THRESHOLD <= 1.0) {
   printf("Gradient threshold should be larger than 1.0!\n");
   exit(1);
 }
 while (ANG < 0) ANG = 360 - ANG;
 if (ANG == 0 && number_pic > 1) number_pic = 1;

 for (i = 1; i <= number_pic; i++) {
   getang(ANG*i % 360);
   if (ANG*i % 90 != 0)
     doviews();
   else
     dostrview((ANG % 360) / 90);
 }

}


usage()
{
  printf("%s\n", usestr);
  exit(1);
}
