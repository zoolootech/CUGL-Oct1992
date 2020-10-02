/************************ grint.c *************************************

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
/* some global variables*/
int FIRSTSLICE,LASTSLICE,THRESHOLD,NLINES;
float ZOOM;
int  buffer[2][6][256];          /* input buffer */
float fxbuf[5][256],fybuf[5][256]; /* X,Y Views floating buffers */ 
float huge fzbuf[256][256];        /* Z view floating buffer */
char *fnamein="ctbild.000";        /* input file name  */

succ(i)   /* (i+1) modolus 3 */
int i;
{return(i==2?0:i+1);}

prev(i)   /* (i-1) modolus 3 */
int i;
{return(i==0?2:i-1);}

/* Set input file - add slice number to extension */
setfilename(filenum)
int filenum;
{fnamein[7]=filenum/100+'0';
 fnamein[8]=(filenum%100)/10+'0';
 fnamein[9]='0'+filenum%10;
}

/* interpolate from bottom line to top line n-1 lines  */
interpolate(line,bot,top,n)
int line,bot,top,n;
{int next,i,j,x,inc;
 inc=top>bot?1:(-1);    /* interpolate backward or forwards ? */
 next=bot+inc;
 for(i=1,j=n-1;i<n;i++,j--){   /* do for each next line of interpolation */
      for(x=0;x<256;x++) buffer[line][next][x]=
        (buffer[line][bot][x]*j+buffer[line][top][x]*i)/n;
      next+=inc;
 }
}

/* midpoint - fraction part of threshold transition distance */ 
float midpoint(b,a)
float b,a;
{return( (THRESHOLD-a) / (b-a) );}

/* get floting point distance values  */
getdistances()
{int z,x,y,i,j,start,stop,inc,line,rzoom,inter;
 float remain;
 FILE *fxfloat,*fyfloat,*fzfloat,*fn[2];
  NLINES=0;         /* number of output lines in X,Y view directions */
  remain=0;         /* remainder of interpolation after roundoff */
  rzoom=ZOOM+0.5;   /* rounded zoom factor */
  fxfloat=fopen("d:xdis.dat","wb");    /* X view floating output file */
  fyfloat=fopen("d:ydis.dat","wb");    /* Y view floating output file */
  for(i=0;i<256;i++)
   for(j=0;j<256;j++)fzbuf[i][j]=256; /* clear Z view buffer */
  for(z=0;z<(LASTSLICE-FIRSTSLICE);z++){  /* For each Slice */
      for(i=0;i<2;i++){               /* open next two slice files */
          setfilename(FIRSTSLICE+z+i);
          fn[i]=fopen(fnamein,"rb");
          fseek(fn[i],(long)512,SEEK_SET); /* skip header block */
      } 
      inter=rzoom; /* interpolation factor assumed rounded zoom */
      /* correct interpolation factor according to floating remainder */
      remain+=rzoom-ZOOM;
      if(remain>=1){
          inter-=1;
          remain-=1;
      }
      else if(remain<=(-1)){
          inter+=1;
          remain+=1;
      }

      line=0;               /* current input buffer line */
      for(j=0;j<inter;j++)  /* clear X,Y floating buffers */
       for(i=0;i<256;i++)fxbuf[j][i]=fybuf[j][i]=256;
      for(y=0;y<256;y++){              /* For each line */
          fread(buffer[line][0],1,512,fn[0]);   
          fread(buffer[line][inter],1,512,fn[1]);
          interpolate(line,0,inter,inter); /* interpolate in_between */
          for(i=0;i<inter;i++){   /* For each interplation line */
             for(x=0;x<256;x++)  /* For each Voxel value */
               /* find threshold transition*/
              if (buffer[line][i+1][x]>=THRESHOLD){
                /* if first transition in X direction get floating distance */
                if(fxbuf[i][y]==256.0) fxbuf[i][y]=(x==0)?0:x-1+
                      midpoint((float)buffer[line][i+1][x],
                              (float)buffer[line][i+1][x-1]);
                /* if first transition in Y direction get floating distance */
                if(fybuf[i][x]==256.0) fybuf[i][x]=(y==0)?0:y-1+
                      midpoint((float)buffer[line][i+1][x],
                                       (float)buffer[1-line][i+1][x]);
                /* if first transition in Z direction get floating distance */
                if(fzbuf[y][x]==256.0) fzbuf[y][x]=
                     (z==0) && (buffer[line][0][x]>=THRESHOLD)?0:NLINES+i+
                      midpoint((float)buffer[line][i+1][x],
                                       (float)buffer[line][i][x]);
             }
          }
          line=1-line;  /* change current input buffer line */
      }
      NLINES+=inter;           /* increment number of output lines */  
      fclose(fn[0]);           /* close slice files */
      fclose(fn[1]);
      fwrite(fxbuf,1,inter*1024,fxfloat); /* write output lines to files */
      fwrite(fybuf,1,inter*1024,fyfloat);
      printf("did %d \n",z);
  }
  fclose(fxfloat); /* close X,Y floating files */
  fclose(fyfloat);
  /* write Z floating files */
  fzfloat=fopen("d:zdis.dat","wb");
  for(i=0;i<256;i++)fwrite(fzbuf[i],1,1024,fzfloat);
  fclose(fzfloat);
}

unsigned char grad(y1,y2,z1,z2,y_factor,z_factor)
float y1,y2,z1,z2;
int y_factor,z_factor;
{float gx,gy,gz;
 unsigned char gxint;
     /* get z and y components of gradient */
  gz=(z1-z2)/y_factor;
  gy=(y1-y2)/z_factor;
     /*compute gx - normalized x component of gradient */
  gx=1/sqrt(1+gz*gz+gy*gy);
  gxint=255*gx+0.5;      /*scale gx by 256 and round */
  return(gxint);
}

/* get gradient and depth shades for one image line */
doline(lineg,lined,line,prevline,succline,z_factor,fg,fd)
unsigned char lineg[],lined[];  /*output buffers */
int line,prevline,succline;  /* input buffer configuration */
int z_factor; /* for choosing forward, backward or central differences */
FILE *fg,*fd;  /* output files */
{int i;
  /* do first pixel in line */
  if(fxbuf[line][0]==256)lineg[0]=lined[0]=0;
  else{
      lined[0]=255-fxbuf[line][0]; /*distance shade */
      lineg[0]=grad(fxbuf[line][0],fxbuf[line][1],fxbuf[prevline][0],
                       fxbuf[succline][0],1,z_factor);
  }
  /* do rest of pixels inside line */
  for(i=1;i<255;i++) if(fxbuf[line][i]==256)lineg[i]=lined[i]=0;
  else{
      lined[i]=255-fxbuf[line][i]; /*distance shade */
      lineg[i]=grad(fxbuf[line][i-1],fxbuf[line][i+1],fxbuf[prevline][i],
                       fxbuf[succline][i],2,z_factor);
  }
  /* do last pixel in line */
  if(fxbuf[line][255]==256)lineg[255]=lined[255]=0;
  else{
      lined[255]=255-fxbuf[line][255]; /*distance shade */
      lineg[255]=grad(fxbuf[line][255],fxbuf[line][254],fxbuf[prevline][255],
                       fxbuf[succline][255],1,z_factor);
  }
  fwrite(lineg,1,256,fg); /* write to output files */
  fwrite(lined,1,256,fd);
}

/* create an gradient and distance shaded view */
doviews(namedis,nameg,named,nlines)
char *namedis,*nameg,*named; /* floating file , gradiet and distance files*/
int nlines;                  /* number of lines in image */
{FILE *fg,*fd,*ffloat;
 int z,i,j,k,midline;
 char lined[256],lineg[256]; /* gradient and distance value buffers */
 midline=1;                  /* middle line in input buffer */
 fd=fopen(named,"wb");       /* open output and input files */
 fg=fopen(nameg,"wb");
 ffloat=fopen(namedis,"rb");
 fread(fxbuf,1,3072,ffloat); /* read first three floating lines */
 /* do first line */
 doline(lineg,lined,0,0,1,1,fg,fd);
 /* do rest of lines */
 for(z=0;z<(nlines-2);z++){      /*for each inside line */
     doline(lineg,lined,midline,prev(midline),succ(midline),2,fg,fd);
     fread(fxbuf[prev(midline)],1,1024,ffloat); /*read next floating line */
     midline=succ(midline);
     printf(" did %d \n",z);
 }
 /* do last line */
 doline(lineg,lined,midline,prev(midline),midline,1,fg,fd);
 fclose(fg);  /* close all files */
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

 getdistances(); /* get floating distance values */
 /* create images */
 printf("doing bottom (Z) views\n");
 doviews("d:zdis.dat","gbo.out","dbo.out",256);
 printf("doing right lateral (X) views\n");
 doviews("d:xdis.dat","grl.out","drl.out",NLINES);
 printf("doing rear (Y) views\n");
 doviews("d:ydis.dat","gre.out","dre.out",NLINES);
 printf("number of lines %d\n",NLINES);
}
