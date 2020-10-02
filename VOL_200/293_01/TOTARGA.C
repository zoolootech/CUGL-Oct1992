/* This is a simple module to convert image files from
   the reconstruction package to AT&T Targa board
    format                                       */
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
main()
{int fin,fout,i,j,k;
  unsigned char c,outbuf[256][3];
  unsigned char inbuf[256],fnamein[20],fnameout[20];
  printf(" Enter fin: ");  /* The file to be converted */
  scanf("%s",fnamein);
  printf("Enter fout: ");  /* Name of destination file */
  scanf("%s",fnameout);
  printf("Enter number of lines: "); /* of image */
  scanf("%d",&k); 
  fin=open(fnamein,O_RDONLY|O_BINARY,0);
  fout=open(fnameout,O_CREAT|O_WRONLY|O_BINARY);

              /* Write Header */
  c=0;
  write(fout,&c,1);
  write(fout,&c,1);
  c=2;
  write(fout,&c,1);
  c=0;
  for(i=0;i<6;i++)write(fout,&c,1);
  c=1;
  write(fout,&c,1);
  c=0;
  for(i=0;i<3;i++)write(fout,&c,1);
  c=1;
  write(fout,&c,1);
  c=0;
  write(fout,&c,1);
  c=1;
  write(fout,&c,1);
  c=24;
  write(fout,&c,1);
  c=0;
  write(fout,&c,1);

   /* Convert raw data to RGB */

  for(i=0;i<k;i++){
      read(fin,inbuf,256);
      for(j=0;j<256;j++)if(inbuf[j]!=0)
           outbuf[j][0]=outbuf[j][1]=outbuf[j][2]=inbuf[j];
      else{
           outbuf[j][1]=outbuf[j][2]=0;
           outbuf[j][0]=255;
      }
      write(fout,outbuf,768);
  }
  close(fin);

  /* add blank blue lines to image with less than 256 lines */

  for(i=0;i<256;i++){ 
       outbuf[i][0]=255;
       outbuf[i][1]=outbuf[i][2]=0;
  }
  for(i=0;i<(256-k);i++) write(fout,outbuf,768);
  close(fout);
 }
