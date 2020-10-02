/***********************  interp.c  **************************************

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
int outfile,ZMAX,FIRSTSLICE,LASTSLICE,THRESHOLD,ZOOM,
    RIGHTMID,LEFTMID,TOPINT,midslice,midline;
int  buffer[10][256];
float huge fzbuf[256][256];
/*            standard 18 output files ( 9 views x 2) */
char *fnamein="ctbild.000",*fgll="gll.out",*fdll="dll.out";
succ(i)
int i;
{return(i==2?0:i+1);}
prev(i)
int i;
{return(i==0?2:i-1);}
setfilename(filenum)
int filenum;
{fnamein[7]=filenum/100+'0';
 fnamein[8]=(filenum%100)/10+'0';
 fnamein[9]='0'+filenum%10;
}
interpolate(bot,top)
int bot,top;
{int next,i,j,x,inc;
 inc=top>bot?1:(-1);
 next=bot+inc;
 for(i=1,j=ZOOM-1;i<ZOOM;i++,j--){
      for(x=0;x<256;x++) buffer[next][x]=
        (buffer[bot][x]*j+buffer[top][x]*i)/ZOOM;
      next+=inc;
 }
}
float midpoint(b,a)
float b,a;
{return( (THRESHOLD-a) / (b-a) );}
getdistances()
{int z,x,y,i,j,start,stop,inc;
 FILE *fzfloat,*fn[2];
  for(i=0;i<256;i++)
   for(j=0;j<256;j++)fzbuf[i][j]=256;
  for(z=0;z<(LASTSLICE-FIRSTSLICE);z++){
      for(i=0;i<2;i++){
          setfilename(FIRSTSLICE+z+i);
          fn[i]=fopen(fnamein,"rb");
          fseek(fn[i],(long)512,SEEK_SET);
      }
      for(y=0;y<256;y++){
          fread(buffer[0],1,512,fn[0]);
          fread(buffer[ZOOM],1,512,fn[1]);
          interpolate(0,ZOOM);
          for(i=0;i<ZOOM;i++){
             for(x=0;x<256;x++)if (buffer[i+1][x]>=THRESHOLD){
                if(fzbuf[y][x]==256.0) fzbuf[y][x]=
                     (z==0) && (buffer[0][x]>=THRESHOLD)?0:z*ZOOM+i+
                      midpoint((float)buffer[i+1][x],
                                       (float)buffer[i][x]);
             }
          }
      }
      fclose(fn[0]);
      fclose(fn[1]);
      printf("did %d \n",z);
  }
  fzfloat=fopen("zdis.dat","wb");
  for(i=0;i<256;i++)fwrite(fzbuf[i],1,1024,fzfloat);
  fclose(fzfloat);
}
unsigned char gradx(y1,y2,z1,z2)
float y1,y2,z1,z2;
{float gx,gy,gz;
 unsigned char gxint;
     /* get z and y components of gradient */
  gz=(z1-z2)/2;
  gy=(y1-y2)/2;
     /*compute gx - normalized x component of gradient */
  gx=1/sqrt(1+gz*gz+gy*gy);
  gxint=255*gx+0.5;      /*scale gx by 256 */
  return(gxint);
}
doviews(namedis,nameg,named,nlines)
char *namedis,*nameg,*named;
int nlines;
{FILE *fg,*fd,*ffloat;
 int z,i,j,k,midline;
 char lined[256],lineg[256];
 midline=1;
 fd=fopen(named,"wb");
 fg=fopen(nameg,"wb");
 ffloat=fopen(namedis,"rb");
 fread(fzbuf,1,3072,ffloat);
 /* do first line */
 if(fzbuf[0][0]==256.0)lineg[0]=lined[0]=0;
 else{
     lined[0]=256-fzbuf[0][0];
     lineg[0]=gradx(fzbuf[0][0]*2,fzbuf[0][1]*2,fzbuf[0][0]*2,fzbuf[1][0]*2);
 }
 for(i=1;i<255;i++)
  if(fzbuf[0][i]==256.0) lineg[i]=lined[i]=0;
  else{
      lined[i]=256-fzbuf[0][i];
      lineg[i]=gradx(fzbuf[0][i-1],fzbuf[0][i+1],2*fzbuf[0][i],2*fzbuf[1][i]);
 }
 if(fzbuf[0][255]==256.0)lineg[255]=lined[255]=0;
 else{
     lined[255]=256-fzbuf[0][255];
     lineg[255]=
       gradx(fzbuf[0][255]*2,fzbuf[0][254]*2,fzbuf[0][255]*2,fzbuf[1][255]*2);
 }
 fwrite(lineg,1,256,fg);
 fwrite(lined,1,256,fd);
 printf("Begining computation of  views\n");
 for(z=0;z<(nlines-2);z++){      /*for each slice */
     if(fzbuf[midline][0]==256.0)lineg[0]=lined[0]=0;
     else{
         lined[0]=256-fzbuf[midline][0];
         lineg[0]=
           gradx(fzbuf[midline][0]*2,fzbuf[midline][1]*2,
                   fzbuf[prev(midline)][0],fzbuf[succ(midline)][0]);
     }
     for(i=1;i<255;i++)
       if(fzbuf[midline][i]==256.0) lineg[i]=lined[i]=0;
      else{
          lined[i]=256-fzbuf[midline][i];
          lineg[i]=gradx(fzbuf[midline][i-1],fzbuf[midline][i+1],
           fzbuf[prev(midline)][i],fzbuf[succ(midline)][i]);
     }
     if(fzbuf[midline][255]==256.0)lineg[255]=lined[255]=0;
     else{
         lined[255]=256-fzbuf[midline][255];
        lineg[255]=gradx(fzbuf[midline][255]*2,fzbuf[midline][254]*2
            ,fzbuf[prev(midline)][255],fzbuf[succ(midline)][255]);
     }
     fwrite(lineg,1,256,fg);
     fwrite(lined,1,256,fd);
     fread(fzbuf[prev(midline)],1,1024,ffloat);
     midline=succ(midline);
     printf(" did %d \n",z);
 }
 /* do last line */
 if(fzbuf[midline][0]==256.0)lineg[0]=lined[0]=0;
 else{
     lined[0]=256-fzbuf[midline][0];
     lineg[0]=gradx(fzbuf[midline][0]*2,fzbuf[midline][1]*2,
         fzbuf[midline][0]*2,fzbuf[prev(midline)][0]*2);
 }
 for(i=1;i<255;i++)
  if(fzbuf[midline][i]==256.0) lineg[i]=lined[i]=0;
  else{
      lined[i]=256-fzbuf[0][i];
      lineg[i]=gradx(fzbuf[midline][i-1],fzbuf[midline][i+1],
         2*fzbuf[midline][i],2*fzbuf[prev(midline)][i]);
 }
 if(fzbuf[midline][255]==256.0)lineg[255]=lined[255]=0;
 else{
     lined[255]=256-fzbuf[midline][255];
     lineg[255]=
       gradx(fzbuf[midline][255]*2,fzbuf[midline][254]*2,
         fzbuf[midline][255]*2,fzbuf[prev(midline)][255]*2);
 }
 fwrite(lineg,1,256,fg);
 fwrite(lined,1,256,fd);
 fclose(fg);
 fclose(fd);
 fclose(ffloat);
}
/**********************************************************/
/**** MAIN ***** MAIN ***** MAIN ***** MAIN ***** MAIN ****/
/**********************************************************/
main()
{
 /* first get some parameters from user */
 printf("Enter Zoom factor: ");
 scanf("%d",&ZOOM);
 printf("Enter Starting scan number: ");
 scanf("%d",&FIRSTSLICE);
 printf("Enter ending scan number: ");
 scanf("%d",&LASTSLICE);
 printf("Enter threshold number: ");
 scanf("%d",&THRESHOLD);
 THRESHOLD+=1024;
 getdistances();
 printf("doing bottom views");
 doviews("zdis.dat","fgbo.out","fdbo.out",256);
}
