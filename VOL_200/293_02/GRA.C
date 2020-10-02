/************************ gra.c *************************************

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
#define DZ    3.0
#define DX    1.0
#define DY    1.0
int outfile,ZMAX,FIRSTSLICE,LASTSLICE,THRESHOLD,ZOOM,
    RIGHTMID,LEFTMID,TOPINT,midslice,midline;
int huge buffer[16][16][256];
FILE *fg,*fd;
/*            standard 18 output files ( 9 views x 2) */
char *fnamein="ctbild.000",*fgll="gll.out",*fdll="dll.out";
succ(i)
int i;
{return(i==14?0:i+1);}
prev(i)
int i;
{return(i==0?14:i-1);}
setfilename(filenum)
int filenum;
{fnamein[7]=filenum/100+'0';
 fnamein[8]=(filenum%100)/10+'0';
 fnamein[9]='0'+filenum%10;
}
interpolate(slice,line)
int slice,line;
{int next,endslice,i,j,x;
 next=succ(slice);
 endslice=slice+ZOOM;
 if(endslice>15) endslice-=16;
 for(i=1,j=ZOOM-1;i<ZOOM;i++,j--){
      for(x=0;x<256;x++) buffer[next][line][x]=
              (buffer[slice][line][x]*j+buffer[endslice][line][x]*i)/ZOOM;
      next=succ(next);
 }
}
readline(filenum,line,bufslice,bufline)
int filenum,line,bufslice,bufline;
{FILE *fn;
 setfilename(filenum);
 fn=fopen(fnamein,"rb");
 fseek(fn,(long)512*(line+1),SEEK_SET);
 fread(buffer[bufslice][bufline],1,512,fn);
 fclose(fn);
}
readsection(firstfile,bufslice,bufline,line)
int firstfile,bufslice,bufline,line;
{int file,k,slice;
 for(file=firstfile,slice=bufslice,k=0;k<16;file++,k+=ZOOM,slice+=ZOOM){
       if(slice>15)slice-=16;
       readline(file,line,slice,bufline); 
 }
 if(ZOOM>1) for(slice=bufslice,k=0;k<16-ZOOM;k+=ZOOM,slice+=ZOOM){
       if(slice>15)slice-=16;
       interpolate(slice,bufline);
 }
}
readblock(firstfile)
int firstfile;
{int i;
  for(i=0;i<16;i++) readsection(firstfile,0,i,i);
}
/******************* VAR1X ****************************/
/*place of change on x axis reference to point (positive search) */
double var1x(x,y,z,zero)
int x,y,z,zero;
{int i;
 double delta1,delta;
  i=x;
  while((buffer[z][y][i]>=THRESHOLD)&&(i>0))i--;
  if(buffer[z][y][i]>=THRESHOLD)return(DX*(i-x));
  while((buffer[z][y][i]<THRESHOLD)&&(i<255))i++;
  if(buffer[z][y][i]<THRESHOLD)return(DX*(i-x));
  else {
        delta1=THRESHOLD-buffer[z][y][i];
        delta=buffer[z][y][i]-buffer[z][y][i-1];
       return((delta1/delta+i-x)*DX);
  }
}

/**************** GETGRADX ***************************************/
/* grad=  2048*(normalized @F/@x of surface func F)              */
/*****************************************************************/
unsigned char getgradx(func,x,y,slice,limit)
int func,x,y,slice,limit;
{double sy[2],sz[2],gx,gy,gz;
 unsigned char gxint;
     /* get x and y components of gradient */
  sz[0]=var1x(x,y,prev(slice),limit);
  sz[1]=var1x(x,y,succ(slice),limit);
  gz=(sz[1]-sz[0])/(2*DZ);
  sy[0]=var1x(x,prev(y),slice,limit);
  sy[1]=var1x(x,succ(y),slice,limit);
  gy=(sy[1]-sy[0])/(2*DY);
     /*compute gx - normalized x component of gradient */
  gx=1/sqrt(1+gz*gz+gy*gy);
  gxint=256*gx+0.5;      /*scale gx by 256 */
  return(gxint);
}
slice_pass(midslice,startfile,z)
int midslice,startfile,z;
{ int midline,i,x,y;
  char lined[256],lineg[256];
   for(i=0;i<256;i++){
        lineg[i]=0;
        lined[i]=0;
   }
   midline=1;
   for(y=1;y<255;y++){ /*for each line*/
         for(x=1;x<255;x++)   /*for each pixel*/
         if((buffer[midslice][midline][x]>=THRESHOLD)&&(lined[y]==0)){
                   lineg[y]=getgradx(0,x,midline,midslice,0);
                   lined[y]=255-x;
         }
         midline=succ(midline);
         if((y>6)&&(y<247))
             readsection(startfile,0,
                            midline>6?midline-7:midline+9,y);
   printf("%d ",y);
   }
   fwrite(lineg,1,256,fg);
   fwrite(lined,1,256,fd);
}
/**********************************************************/
/**** MAIN ***** MAIN ***** MAIN ***** MAIN ***** MAIN ****/
/**********************************************************/
main()
{int z,i,j,k,r,startfile;
 midslice=1;
/* first get some parameters from user */
 printf("Enter Zoom factor: ");
 scanf("%d",&ZOOM);
 printf("Enter Starting scan number: ");
 scanf("%d",&FIRSTSLICE);
 printf("Enter ending scan number: ");
 scanf("%d",&LASTSLICE);
 ZMAX=LASTSLICE-FIRSTSLICE+1;
 printf("Enter threshold number: ");
 scanf("%d",&THRESHOLD);
 THRESHOLD+=1024;

 /*creat files for first pass */
 fd=fopen(fdll,"w+b");
 fg=fopen(fgll,"w+b");
 /* read first block of scans */
 readblock(FIRSTSLICE);
 startfile=FIRSTSLICE;

 /* first pass on scan data (forward) */
 printf("Begining computation of REAR,LEFT,REAR,and LEFT-MID views\n");
 for(z=0;z<LASTSLICE-FIRSTSLICE;z++){                 /*for each slice */
    for(i=1;i<=ZOOM;i++,midslice++)slice_pass(midslice,startfile,z*ZOOM+i);
    printf("did slice %d\n",z);
    if(((z+1)*ZOOM>8)&&((LASTSLICE-startfile)*ZOOM+1>16)){
        startfile++;
        midslice-=ZOOM;
    }
    readblock(startfile);
 }
 fclose(fg);
 fclose(fd);
}
