/************************* deb.c ************************************

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
int outfile,ZMAX,FIRSTSLICE,LASTSLICE,THRESHOLD,
    RIGHTMID,LEFTMID,TOPINT,midslice,midline,NLINES;
float ZOOM;
int  buffer[2][6][256];
float fxbuf[5][256],fybuf[5][256];
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
interpolate(line,bot,top,n)
int line,bot,top,n;
{int next,i,j,x,inc;
 inc=top>bot?1:(-1);
 next=bot+inc;
 for(i=1,j=n-1;i<n;i++,j--){
      for(x=0;x<256;x++) buffer[line][next][x]=
        (buffer[line][bot][x]*j+buffer[line][top][x]*i)/n;
      next+=inc;
 }
}
float midpoint(b,a)
float b,a;
{return( (THRESHOLD-a) / (b-a) );}
getdistances()
{int z,x,y,i,j,start,stop,inc,line,rzoom,inter;
 float remain;
 FILE *fxfloat,*fyfloat,*fzfloat,*fn[2];
  NLINES=0;
  remain=0;
  rzoom=ZOOM+0.5;
  fxfloat=fopen("xdis.dat","wb");
  fyfloat=fopen("ydis.dat","wb");
  for(i=0;i<256;i++)
   for(j=0;j<256;j++)fzbuf[i][j]=256;
  for(z=0;z<(LASTSLICE-FIRSTSLICE);z++){
      for(i=0;i<2;i++){
          setfilename(LASTSLICE-(z+i));
          fn[i]=fopen(fnamein,"rb");
          fseek(fn[i],(long)512,SEEK_SET);
      }
      inter=rzoom;
      remain+=rzoom-ZOOM;
      if(remain>=1){
          inter-=1;
          remain-=1;
      }
      else if(remain<=(-1)){
          inter+=1;
          remain+=1;
      }
      line=0;
      for(j=0;j<inter;j++)
       for(i=0;i<256;i++)fxbuf[j][i]=fybuf[j][i]=256;
      for(y=255;y>=0;y--){
          fseek(fn[0],(long)512*(y+1),SEEK_SET);
          fread(buffer[line][0],1,512,fn[0]);
          fseek(fn[1],(long)512*(y+1),SEEK_SET);
          fread(buffer[line][inter],1,512,fn[1]);
          interpolate(line,0,inter,inter);
          for(i=0;i<inter;i++){
             for(x=255;x>=0;x--)if (buffer[line][i+1][x]>=THRESHOLD){
                if(fxbuf[i][y]==256.0) fxbuf[i][y]=(x==255)?0:255-(x+1)+
                      midpoint((float)buffer[line][i+1][x],
                              (float)buffer[line][i+1][x+1]);
                if(fzbuf[y][x]==256.0) fzbuf[y][x]=
                     (z==0) && (buffer[line][0][x]>=THRESHOLD)?0:NLINES+i+
                      midpoint((float)buffer[line][i+1][x],
                                       (float)buffer[line][i][x]);
             }
          }
          line=1-line;
      }
      NLINES+=inter;  
      fclose(fn[0]);
      fclose(fn[1]);
      fwrite(fxbuf,1,inter*1024,fxfloat);
      fwrite(fybuf,1,inter*1024,fyfloat);
      printf("did %d \n",z);
  }
  fclose(fxfloat);
  fclose(fyfloat);
  fzfloat=fopen("zdis.dat","wb");
  for(i=0;i<256;i++){fwrite(fzbuf[i],1,1024,fzfloat);printf("wr %d",i);}
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
 fread(fxbuf,1,3072,ffloat);
 /* do first line */
 if(fxbuf[0][0]==256.0)lineg[0]=lined[0]=0;
 else{
     lined[0]=256-fxbuf[0][0];
     lineg[0]=gradx(fxbuf[0][0]*2,fxbuf[0][1]*2,fxbuf[0][0]*2,fxbuf[1][0]*2);
 }
 for(i=1;i<255;i++)
  if(fxbuf[0][i]==256.0) lineg[i]=lined[i]=0;
  else{
      lined[i]=256-fxbuf[0][i];
      lineg[i]=gradx(fxbuf[0][i-1],fxbuf[0][i+1],2*fxbuf[0][i],2*fxbuf[1][i]);
 }
 if(fxbuf[0][255]==256.0)lineg[255]=lined[255]=0;
 else{
     lined[255]=256-fxbuf[0][255];
     lineg[255]=
       gradx(fxbuf[0][255]*2,fxbuf[0][254]*2,fxbuf[0][255]*2,fxbuf[1][255]*2);
 }
 fwrite(lineg,1,256,fg);
 fwrite(lined,1,256,fd);
 printf("Begining computation of  views\n");
 for(z=0;z<(nlines-2);z++){      /*for each slice */
     if(fxbuf[midline][0]==256.0)lineg[0]=lined[0]=0;
     else{
         lined[0]=256-fxbuf[midline][0];
         lineg[0]=
           gradx(fxbuf[midline][0]*2,fxbuf[midline][1]*2,
                   fxbuf[prev(midline)][0],fxbuf[succ(midline)][0]);
     }
     for(i=1;i<255;i++)
       if(fxbuf[midline][i]==256.0) lineg[i]=lined[i]=0;
      else{
          lined[i]=256-fxbuf[midline][i];
          lineg[i]=gradx(fxbuf[midline][i-1],fxbuf[midline][i+1],
           fxbuf[prev(midline)][i],fxbuf[succ(midline)][i]);
     }
     if(fxbuf[midline][255]==256.0)lineg[255]=lined[255]=0;
     else{
         lined[255]=256-fxbuf[midline][255];
        lineg[255]=gradx(fxbuf[midline][255]*2,fxbuf[midline][254]*2
            ,fxbuf[prev(midline)][255],fxbuf[succ(midline)][255]);
     }
     fwrite(lineg,1,256,fg);
     fwrite(lined,1,256,fd);
     fread(fxbuf[prev(midline)],1,1024,ffloat);
     midline=succ(midline);
     printf(" did %d \n",z);
 }
 /* do last line */
 if(fxbuf[midline][0]==256.0)lineg[0]=lined[0]=0;
 else{
     lined[0]=256-fxbuf[midline][0];
     lineg[0]=gradx(fxbuf[midline][0]*2,fxbuf[midline][1]*2,
         fxbuf[midline][0]*2,fxbuf[prev(midline)][0]*2);
 }
 for(i=1;i<255;i++)
  if(fxbuf[midline][i]==256.0) lineg[i]=lined[i]=0;
  else{
      lined[i]=256-fxbuf[0][i];
      lineg[i]=gradx(fxbuf[midline][i-1],fxbuf[midline][i+1],
         2*fxbuf[midline][i],2*fxbuf[prev(midline)][i]);
 }
 if(fxbuf[midline][255]==256.0)lineg[255]=lined[255]=0;
 else{
     lined[255]=256-fxbuf[midline][255];
     lineg[255]=
       gradx(fxbuf[midline][255]*2,fxbuf[midline][254]*2,
         fxbuf[midline][255]*2,fxbuf[prev(midline)][255]*2);
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
 scanf("%f",&ZOOM);
 printf("Enter Starting scan number: ");
 scanf("%d",&FIRSTSLICE);
 printf("Enter ending scan number: ");
 scanf("%d",&LASTSLICE);
 printf("Enter threshold number: ");
 scanf("%d",&THRESHOLD);
 THRESHOLD+=1024;
 getdistances();
 printf("doing left views");
 doviews("xdis.dat","gll.out","dll.out",NLINES);
 printf("doing bottom views");
 doviews("zdis.dat","gto.out","dto.out",256);
 printf("numberof lines %d\n",NLINES);
}
