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
 *************************************************************/

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

int NLINES,IXmax,IYmax,xinc,yinc;
int (*xdir)(),(*ydir)();
float THETA,cosTheta,sinTheta,tgnTheta;
float fxbuf[3][256],fybuf[3][256];/* input buffers */

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
  for(i=0;i<256;i++) distances[i].XY=0;
  /* project Y-data onto image line */
  for(i=0;i<256;i++) if(liney[i]!=256.0){
       X=(*ydir)(i,255);
       IND=IXmax -(X*sinTheta-liney[i]*cosTheta)+0.5;
       D=liney[i]/sinTheta+(X-liney[i]/tgnTheta)*cosTheta;
       if((IND>=0) && (IND<256)){
           if(distances[IND].XY==0)putd(&distances[IND],D,i,1);
           else if(distances[IND].dist>D)putd(&distances[IND],D,i,1);
       }
  }

  /*project X-data onto image plane */
 for(i=0;i<256;i++) if(linex[i]!=256.0){
       Y=(*xdir)(i,255);
       IND=IXmax-(linex[i]*sinTheta-Y*cosTheta)+0.5;
       D=Y/sinTheta+(linex[i]-Y/tgnTheta)*cosTheta;
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
  gxint=255*(nx*cosTheta+ny*sinTheta)+0.5; /*scale gradient shade by 256 */
  return(gxint);
}

doline(linex,linex1,linex2,liney,liney1,liney2,z_fac,fg,fd)
float linex[],linex1[],linex2[],liney[],liney1[],liney2[];
int z_fac;
FILE *fg,*fd;

{ int i;
  unsigned char lined[256],lineg[256];
      /* empty bit on image line */
  for(i=0;i<256;i++)if(distances[i].XY==0)lineg[i]=lined[i]=0;
  else{
      lined[i]=(distances[i].dist<256)?255-distances[i].dist+0.5:0;
      /* bit on image line projected from Y view */
      if(distances[i].XY==1) switch(distances[i].indXY){
           case 0:lineg[i]=
                   grad(liney[1]*yinc,liney[0]*yinc,(float)0,(float)2,
                        liney1[0],liney2[0],1,2,z_fac);
                  break;
           case 255:lineg[i]=
                    grad(liney[255]*yinc,liney[254]*yinc,(float)0,(float)2,
                         liney1[255],liney2[255],1,2,z_fac);
                  break;
           default:lineg[i]=
                   grad(liney[distances[i].indXY+yinc],
                        liney[distances[i].indXY-yinc],(float)0,(float)2,
                          liney1[distances[i].indXY],
                          liney2[distances[i].indXY],2,2,z_fac); 
                  break;
      }
      /* bit on image line projected from X view */
      else switch(distances[i].indXY){
           case 0:lineg[i]=
                   grad((float)0,(float)2,linex[1]*xinc,linex[0]*xinc,
                        linex1[0],linex2[0],2,1,z_fac);
                  break;
           case 255:lineg[i]=
                    grad((float)0,(float)2,linex[255]*xinc,linex[254]*xinc,
                         linex1[255],linex2[255],2,1,z_fac);
                  break;
           default:lineg[i]=
                   grad((float)0,(float)2,linex[distances[i].indXY+xinc],
                        linex[distances[i].indXY-xinc],
                          linex1[distances[i].indXY],
                          linex2[distances[i].indXY],2,2,z_fac);
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
 fx=fopen(xfile,"rb");
 fy=fopen(yfile,"rb");

/*read first three lines */
 for(i=0;i<3;i++){
     fseek(fx,(long)FLOAT_LINE*(*ydir)(i,NLINES-1),SEEK_SET);
     fread(fxbuf[i],1,FLOAT_LINE,fx);
     fseek(fy,(long)FLOAT_LINE*(*xdir)(i,NLINES-1),SEEK_SET);
     fread(fybuf[i],1,FLOAT_LINE,fy);
 }

 /* do first line (forward differene)*/
 getdistances(fxbuf[0],fybuf[0]);
 doline(fxbuf[0],fxbuf[0],fxbuf[1],fybuf[0],fybuf[0],fybuf[1],1,fg,fd);

 /* do middle lines (central diffrence) */
 printf("Begining computation of  views\n");
 for(z=0;z<(NLINES-2);z++){      /*for each slice */
     getdistances(fxbuf[mid],fybuf[mid]);
     doline(fxbuf[mid],fxbuf[prev(mid)],fxbuf[succ(mid)],
            fybuf[mid],fybuf[prev(mid)],fybuf[succ(mid)],2,fg,fd);
     fseek(fx,(long)FLOAT_LINE*(*ydir)(i,NLINES-1),SEEK_SET);
     fseek(fy,(long)FLOAT_LINE*(*xdir)(i,NLINES-1),SEEK_SET);
     i++;
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

getang(ang)
int *ang;
{ scanf("%d",ang);
  sprintf(fng,"gang%d.out",*ang);
  sprintf(fnd,"dang%d.out",*ang);
  if( ( *ang > 0) && ( *ang < 90) ){
       xdir=forward;
       ydir=forward;
       sprintf(xfile,"%c:xdis1.dat",DR);
       sprintf(yfile,"%c:ydis1.dat",DR);
       xinc=1;
       yinc=1;
   }
   else if( ( *ang > 90) && ( *ang < 180) ){
       xdir=backward;
       ydir=forward;
       sprintf(xfile,"%c:ydis1.dat",DR);
       sprintf(yfile,"%c:xdis2.dat",DR);
       xinc=-1;
       yinc=1;
       *ang-=90;
   }
   else if( ( *ang > 180) && ( *ang < 270) ){
       xdir=backward;
       ydir=backward;
       sprintf(xfile,"%c:xdis2.dat",DR);
       sprintf(yfile,"%c:ydis2.dat",DR);
       xinc=-1;
       yinc=-1;
       *ang-=180;
   }
   else if( ( *ang > 270) && ( *ang < 360) ){
       xdir=forward;
       ydir=backward;
       sprintf(xfile,"%c:ydis2.dat",DR);
       sprintf(yfile,"%c:xdis1.dat",DR);
       xinc=1;
       yinc=-1;
       *ang-=270;
   }
   else {
       printf(" Bad angle\n");
       exit(1);
  }
}
 
/**********************************************************/
/**** MAIN ***** MAIN ***** MAIN ***** MAIN ***** MAIN ****/
/**********************************************************/

main()

{ char filname[13];
  FILE *par;
  int ANG;

 /* first get some parameters */
 par=fopen("param.dat","rb");
 fscanf(par,"%c %d",&DR,&NLINES);
 fclose(par);
 printf("Enter angle: ");
 getang(&ANG);

 IXmax=(255*ANG)/90;
 THETA=(float)ANG*PI/180;
 sinTheta=sin(THETA);
 cosTheta=cos(THETA);
 tgnTheta=sinTheta/cosTheta;

 doviews();
 exit(0);
}
