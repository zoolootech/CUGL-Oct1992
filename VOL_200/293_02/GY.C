/**********************  gy.c  ***************************************

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
#define PI    3.141592653
int outfile,ZMAX,FIRSTSLICE,LASTSLICE,THRESHOLD,
    RIGHTMID,LEFTMID,TOPINT,midslice,midline;
int huge  buffer[3][19][256];

/*            standard 18 output files ( 9 views x 2) */
char *fnamein="ctbild.000",*fgfr="gfr.out",*fdfr="dfr.out";
succ(i)
int i;
{return(i==18?0:i+1);}
prev(i)
int i;
{return(i==0?18:i-1);}
setfilename(filenum)
int filenum;
{fnamein[7]=filenum/100+'0';
 fnamein[8]=(filenum%100)/10+'0';
 fnamein[9]='0'+filenum%10;
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
{int file;
 for(file=firstfile;file<firstfile+3;file++){
       readline(file,line,bufslice,bufline);
       bufslice=(bufslice==2)?0:bufslice+1;
 }
}
readblock(firstfile)
int firstfile;
{int fil,i,j;
 FILE *fn;
 for(fil=firstfile,i=0;i<3;i++,fil++){
     setfilename(fil);
     fn=fopen(fnamein,"rb");
     fseek(fn,(long)512*238,SEEK_SET);
     for(j=18;j>=0;j--) fread(buffer[i][j],1,512,fn);
     fclose(fn);
 }
}
/******************* VAR1Y ****************************/
/*place of change on y axis reference to point (positive search) */
double var1y(x,line,z,y)
int x,line,z,y;
{int i,j;
 double delta1,delta;
  i=line; j=y;
  while((buffer[z][i][x]>=THRESHOLD)&&(j>0)&&((y-j)<9)){
       i=prev(i);
       j--;
  }
  if(buffer[z][i][x]>=THRESHOLD)return(DY*(j-y));
  while((buffer[z][i][x]<THRESHOLD)&&(j<255)&&((j-y)<9)){
       i=succ(i);
       j++;
  }
  if(buffer[z][i][x]<THRESHOLD)return(DY*(j-y));
  else {
        delta1=THRESHOLD-buffer[z][i][x];
        delta=buffer[z][i][x]-buffer[z][i-1][x];
        return((delta1/delta+j-y)*DY);
  }
}
/**************** GETGRADY ***************************************/
/* grad=  2048*(normalized @F/@y of surface func F)              */
/*****************************************************************/
unsigned char getgrady(func,x,line,slice,y)
int func,x,line,slice,y;
{double sx[2],sz[2],gx,gy,gz;
 unsigned char gyint;
     /* get x and z components of gradient */
  sz[0]=var1y(x,line,0,y);
  sz[1]=var1y(x,line,2,y);
  gz=(sz[1]-sz[0])/(2*DZ);
  sx[0]=var1y(x-1,y,slice,y);
  sx[1]=var1y(x+1,y,slice,y);
  gx=(sx[1]-sx[0])/(2*DX);
     /*compute gy - normalized y component of gradient */
  gy=1/sqrt(1+gz*gz+gx*gx);
  gyint=256*gy+0.5;      /*scale gy by 256 */
  return(gyint);
}

/**********************************************************/
/**** MAIN ***** MAIN ***** MAIN ***** MAIN ***** MAIN ****/
/**********************************************************/
main()
{int x,y,z,i,j,k,r;
 FILE *fg,*fd;
 unsigned char lined[256],lineg[256];
 midslice=1;
 midline=1;   
/* first get some parameters from user */
 printf("Enter Starting scan number: ");
 scanf("%d",&FIRSTSLICE);
 printf("Enter ending scan number: ");
 scanf("%d",&LASTSLICE);
 ZMAX=LASTSLICE-FIRSTSLICE+1;
 printf("Enter threshold number: ");
 scanf("%d",&THRESHOLD);
 THRESHOLD+=1024;
 /*creat files for first pass */
 fd=fopen(fdfr,"wb");
 fg=fopen(fgfr,"wb");
 /* read first 3 scans */
 readblock(FIRSTSLICE);
 /* first pass on scan data (forward) */
 printf("Begining computation of REAR,LEFT,REAR,and LEFT-MID views\n");
 for(z=FIRSTSLICE+1;z<LASTSLICE;z++){                 /*for each slice */
       for(i=0;i<256;i++){
             lineg[i]=0;
             lined[i]=0;
       }
      for(y=254;y>1;y--){ /*for each line*/
             for(x=1;x<255;x++)if( buffer[1][midline][x]>=THRESHOLD){
                 if(lined[x]==0){
                       lineg[x]=getgrady(0,x,midline,1,255-y);
                       lined[x]=y;
                 }
             }
             if((y>9)&&(y<247)) 
                readsection(z-1,0,(midline>8)?midline-9:midline+10,y-10);
             midline=succ(midline);
       }
       fwrite(lineg,1,256,fg);
       fwrite(lined,1,256,fd);
       printf("did slice %d\n",z);
       if(z<LASTSLICE-1) readblock(z);
       midline=1;
 }
 fclose(fg);
 fclose(fd);
}
