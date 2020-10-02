/**********************  smooth2.c  ***************************************

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
unsigned char grads[5][256],depths[3][254],out[256];
char *fgname="gbo.out",*fdname="dbo.out",*outname="goo.out";
int RMAX,avreg;
main()
{int x,y,gline,dline;
 FILE *ing,*ind,*outf;
 gline=2;
 dline=1;
 printf("Enter RMAX: ");
 scanf("%d",&RMAX);
 ing=fopen(fgname,"rb");
 ind=fopen(fdname,"rb");
 outf=fopen(outname,"wb");
 fseek(ind,(long)512,SEEK_SET);
 fread(depths,1,768,ind);
 fread(grads,1,1280,ing);
 fwrite(grads,1,512,outf);
 for(y=2;y<254;y++){
       for(x=0;x<2;x++){
           out[x]=grads[gline][x];
           out[254+x]=grads[gline][254+x];
       }
       for(x=2;x<254;x++)if(uncorrelated(gline,dline,x,RMAX)!=0) out[x]=avreg;
                         else out[x]=grads[gline][x];
       fwrite(out,1,256,outf);
       gline= (gline==4)?0:gline+1;
       dline= (dline==2)?0:dline+1;
       fread(grads[gline<3?gline+2:gline-3],1,256,ing);
       fread(depths[dline==2?0:dline+1],1,256,ind);
       printf(" did %d ",y);
 }
 fclose(ing);
 fclose(ind);
 gline= (gline==4)?0:gline+1;
 fwrite(grads[gline],1,256,outf);
 gline= (gline==4)?0:gline+1;
 fwrite(grads[gline],1,256,outf);
 fclose(out);
}
uncorrelated(yg,yd,x,maxcor)
int yg,yd,x,maxcor;
{ int maxx,maxy;
  if(!slice_edge(yd,x)) return(0);
  maxx=max(grads[yg][x-1],grads[yg][x+1]);
  maxy=max(grads[yg==0?4:yg-1][x],grads[yg==4?0:yg+1][x]);
  avreg=max(maxx,maxy);
  if( (avreg-grads[yg][x])>RMAX ) return(1);
  else return(0);
}
slice_edge(y,x)
int y,x;
{ int a[5],d[4],value;
  a[0]=depths[y][x];
  a[1]=depths[y][x-1];
  a[2]=depths[y][x+1];
  a[3]=depths[y==0?4:y-1][x];
  a[4]=depths[y==4?0:y+1][x];
  d[0]=abs(a[1]-a[0]);
  d[1]=abs(a[2]-a[0]);
  d[2]=abs(a[3]-a[0]);
  d[3]=abs(a[4]-a[0]);
  value=0;
  if( d[0]==1 && d[1]==0) value+=1;
  else if( d[1]==1 && d[0]==0) value+=2;
  if( d[2]==1 && d[3]==0) value+=4;
  else if( d[3]==1 && d[2]==0) value+=8;
  return(value);
}
max(a,b)
int a,b;
{return(a>b?a:b);}
