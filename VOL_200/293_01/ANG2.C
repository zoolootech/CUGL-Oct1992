/*********************** ang2.c **************************************

	  3-D Reconstruction of Medical Images

	Three Dimensional Reconstruction Of Medical
	Images from Serial Slices - CT, MRI, Ultrasound


   These programs process a set of slices images (scans) for one
   patient. It outputs two sets of files containing nine predefined
   views of bony surfaces. One set contains distance values and
   the other gradient values.

   The distance values are used as 3-D spatial topographic surface
   coordinate maps for geometrical analysis of the scanned object.

   The gradient values are used for rendering the surface maps on
   CRT displays for subjective viewing where perception of small
   surface details is important.

	Daniel Geist, B.S.
	Michael W. Vannier, M.D.

	Mallinckrodt Institute of Radiology
	Washington University School of Medicine
	510 S. Kingshighway Blvd.
	St. Louis, Mo. 63110

	These programs may be copied and used freely for non-commercial
	purposes by developers with inclusion of this notice.


********************************************************************/
#include <stdio.h>
#include <math.h>
#define FLOAT_LINE 1024
#define PI 3.141592653
/* the structure below is the data for a point on the surface projected
   on the view plane                        */
typedef struct DIS_REC {
      float dist;            /*distance from view plane */
      int indXY;             /* index of data on main axis view */
      char XY;               /* Which main axis view obtained from (X or Y)*/
};
struct DIS_REC distances[256]; /* one projected line */ 
int NLINES,IXmax,IYmax;
float THETA,cosTheta,sinTheta,tgnTheta;
float fxbuf[3][256],fybuf[3][256];/* input buffers */
/*  output files  */
char *fng="gang.out",*fnd="dang.out";
succ(i)
int i;
{return(i==2?0:i+1);}
prev(i)
int i;
{return(i==0?2:i-1);}
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
{ int i,IND;
  float D;
  for(i=0;i<256;i++) distances[i].XY=0;
  /* project Y-data onto image line */
  for(i=0;i<256;i++){
       IND=(255-i)*cosTheta-liney[i]*sinTheta;
       D=liney[i]/cosTheta-(255-i-liney[i]*tgnTheta)*sinTheta;
       IND=IXmax-IND;
       if((IND>=0) && (IND<256)){
           if(distances[IND].XY==0)putd(&distances[IND],D,i,1);
           else if(distances[IND].dist>D)putd(&distances[IND],D,i,1);
       }
  }
  /*project X-data onto image plane */
  for(i=0;i<256;i++){
       IND=(255-i)*sinTheta-linex[i]*cosTheta;
       D=linex[i]/sinTheta-(255-i-linex[i]/tgnTheta)*cosTheta;
       IND+=IXmax;
       if((IND>=0) && (IND<256)){
           if(distances[IND].XY==0)putd(&distances[IND],D,i,2);
           else if(distances[IND].dist>D)putd(&distances[IND],D,i,2);
       }
  }
  /* fill holes due to low resolution */
  for(i=1;i<255;i++)if( (distances[i].XY==0) && (distances[i+1].XY!=0) &&
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
  gxint=255*(nx*sinTheta+ny*cosTheta)+0.5; /*scale gradient shade by 256 */
  return(gxint);
}
doline(linex,linex1,liney,linex2,liney1,liney2,z_fac,fg,fd)
float linex[],linex1[],linex2[],liney[],liney1[],liney2[];
int z_fac;
FILE *fg,*fd;
{ int i;
  unsigned char lined[256],lineg[256];
      /* empty bit on image line */
  for(i=0;i<256;i++)if(distances[i].XY==0)lineg[i]=lined[i]=0;
  else{
      lined[i]=(distances[i].dist<256)?255-distances[i].dist:0;
      /* bit on image line projected from Y view */
      if(distances[i].XY==1) switch(distances[i].indXY){
           case 0:lineg[i]=
                   grad(liney[0],liney[1],(float)0,(float)2,
                        liney1[0],liney2[0],1,2,z_fac);
                  break;
           case 255:lineg[i]=
                    grad(liney[254],liney[255],(float)0,(float)2,
                         liney1[255],liney2[255],1,2,z_fac);
                  break;
           default:lineg[i]=
                   grad(liney[distances[i].indXY-1],
                        liney[distances[i].indXY+1],(float)0,(float)2,
                          liney1[distances[i].indXY],
                          liney2[distances[i].indXY],2,2,z_fac); 
                  break;
      }
      /* bit on image line projected from X view */
      else switch(distances[i].indXY){
           case 0:lineg[i]=
                   grad((float)0,(float)2,linex[0],linex[1],
                        linex1[0],linex2[0],2,1,z_fac);
                  break;
           case 255:lineg[i]=
                    grad((float)0,(float)2,linex[254],linex[255],
                         linex1[255],linex2[255],2,1,z_fac);
                  break;
           default:lineg[i]=
                   grad((float)0,(float)2,linex[distances[i].indXY-1],
                        linex[distances[i].indXY+1],
                          linex1[distances[i].indXY],
                          linex2[distances[i].indXY],2,1,z_fac);
                  break;
      }
  }
  fwrite(lineg,1,256,fg);
  fwrite(lined,1,256,fd);
}
doviews()
{FILE *fg,*fd,*fx,*fy;
 int z,i,j,k,mid;
 mid=1;
 fd=fopen(fnd,"wb");
 fg=fopen(fng,"wb");
 fx=fopen("xdis.dat","rb");
 fy=fopen("ydis.dat","rb");
 fread(fxbuf,1,3*FLOAT_LINE,fx);
 fread(fybuf,1,3*FLOAT_LINE,fy);
 /* do first line (forward differene)*/
 getdistances(fxbuf[0],fybuf[0]);
 doline(fxbuf[0],fxbuf[0],fxbuf[1],fybuf[0],fybuf[0],fybuf[1],1,fg,fd);
 /* do middle lines (central diffrence) */
 printf("Begining computation of  views\n");
 for(z=0;z<(NLINES-2);z++){      /*for each slice */
     getdistances(fxbuf[mid],fybuf[mid]);
     doline(fxbuf[mid],fxbuf[prev(mid)],fxbuf[succ(mid)],
            fybuf[mid],fybuf[prev(mid)],fybuf[succ(mid)],2,fg,fd);
     fread(fxbuf[prev(mid)],1,FLOAT_LINE,fx);
     fread(fybuf[prev(mid)],1,FLOAT_LINE,fy);
     mid=succ(mid);
     printf(" did %d \n",z);
 }
 /* do last line (backward difference)*/
 getdistances(fxbuf[mid],fybuf[mid]);
 doline(fxbuf[mid],fxbuf[prev(mid)],fxbuf[mid],
            fybuf[mid],fybuf[prev(mid)],fybuf[mid],1,fg,fd);
 fclose(fg);
 fclose(fd);
 fclose(fx);
 fclose(fy);
}
/**********************************************************/
/**** MAIN ***** MAIN ***** MAIN ***** MAIN ***** MAIN ****/
/**********************************************************/
main()
{
 /* first get some parameters from user */
 printf("Enter angle: ");
 scanf("%f",&THETA);
 IXmax=255*(1-THETA/90);
 IYmax=255*(THETA/90);
 THETA*=PI/180;
 sinTheta=sin(THETA);
 cosTheta=cos(THETA);
 tgnTheta=sinTheta/cosTheta;
 printf("Enter number of lines: ");
 scanf("%d",&NLINES);
 doviews();
}
