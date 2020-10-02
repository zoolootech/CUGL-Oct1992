/******************** GRAD ******************************
  grad.c
  Daniel Geist - Mallinckrodt Institue of Radiology 1987.
   This program reads a set of ct scans for one
   patient. It outputs two sets of files containing predefined 
   views of bone surfaces. One set contains distance values and
   the other gradient values.

   Danny Geist & Mike Vannier

   Contact for further information:
	Michael W. Vannier
	Mallinckrodt Institute of Radiology
	Washington University School of Medicine
	510 South Kingshighway Blvd.
	St. Louis, Mo. 63110 USA

   Ake Wallin has made the following changes - 1988

   -Command line input
   -Threshold of gradient for differencing
   -Views to choose: BO (bottom), TO (top), RL (right lateral),
    LL (left lateral), RE (rear), FR (front), NO (none, just leave distance)
   -Clipping planes: RL (right lateral), LL (lateral left), RE (rear),
    FR (front)
   -Scan of data in "correct" order, assuming:
    object:
    low z - feet, high z - head
    low x - right, high x - left
    low y - back, high y - front
    image:
    low x - left, high x - right
    low y - down, high y - up
		    front
    view BO : right      left
		    back
		    front
    view TO : left       right
		    back
		    head
    view RL : back      front
		    feet
		    head
    view LL : front     back
		    feet
		    head
    view RE : left      right
		    feet
		    head
    view FR : right     left
		    feet
*********************************************************/
#include <stdio.h>
#include <math.h>
#include <ctype.h>

/* some global variables*/
int FIRSTSLICE,LASTSLICE,THRESHOLD,NLINES;
float ZOOM;
char DR;
int  buffer[2][6][256];          /* input buffer */
float fxbuf[5][256],fybuf[5][256]; /* X,Y Views floating buffers */

float huge fzbuf[256][256];        /* Z view floating buffer */
char fnamein[50]="ctbild.000";        /* input file name  */
float GRAD_THRESHOLD = 10.0;
unsigned int   views = 0xFFFF;
int   clipx[2], clipy[2];      /* clipping planes */
int   dispmode;                /* distance and/or gradient */
int   image_or[2];            /* image oriention in x and y */
int   object_or[3];           /* image oriention in x, y and z */
int   header_blocks;          /* number of header blocks in CT's */
char *usestr1 = "Usage: grad [file] [-z] [-f] [-l] [-t] [-d] [-n(g|d)] [-g]";
char *usestr2 = "            [-h] [-v(bo|to|rl|ll|re|fr|no)] [-c(rl|ll|re|fr)]";
char *usestr3 = "            [-i(x(r|l)|y(u|d))] [-o(x(r|l)|y(f|b)|z(h|f))]";

succ(i)   /* (i+1) modolus 3 */
int i;
{
    return(i==2?0:i+1);
}

prev(i)   /* (i-1) modolus 3 */
int i;
{
    return(i==0?2:i-1);
}

/* Set input file - add slice number to extension */
setfilename(filenum)
int filenum;
{
    int i;

    for (i = strlen(fnamein)-1; i > 0; i--)
      if (fnamein[i] == '.') break;
    if (i == 0) {
      printf("CT file names inconsistend!\n");
      exit(1);
    }
    fnamein[++i]=filenum/100+'0';
    fnamein[++i]=(filenum%100)/10+'0';
    fnamein[++i]='0'+filenum%10;
}

/* interpolate from bottom line to top line n-1 lines  */
interpolate(line,bot,top,n)
int line,bot,top,n;
{
    int next,i,j,x,inc;
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
{
    return( (THRESHOLD-a) / (b-a) );
}

/* get floating point distance values  */
getdistances(xstart,xstop,xdir,
             ystart,ystop,ydir,
             start_slice,end_slice,zdir,
             xbufxstart, xbufxdir, xbufystart, xbufydir,
             ybufxstart, ybufxdir, ybufystart, ybufydir,
             zbufxstart, zbufxdir, zbufystart, zbufydir,
             pass)
int xstart,xstop,xdir,ystart,ystop,ydir,start_slice,end_slice,zdir,pass;
int  xbufxstart, xbufxdir, xbufystart, xbufydir;
int  ybufxstart, ybufxdir, ybufystart, ybufydir;
int  zbufxstart, zbufxdir, zbufystart, zbufydir;
/* also used are global variables: fxbuf, fybuf, fzbuf, buffer,
   ZOOM, DR, image_or, object_or. */
{
    int z,x,y,i,j,start,stop,inc,line,rzoom,inter;
    float remain;
    FILE *fxfloat,*fyfloat,*fzfloat,*fn[2];
    char filename[13];
    int  filegap;
    int  xbfx, ybfx, zbfx, zbfy;

    NLINES=0;         /* number of output lines in X,Y view directions */
    remain=0;         /* remainder of interpolation after roundoff */
    rzoom=ZOOM+0.5;   /* rounded zoom factor */
    /* X view floating output file */
    sprintf(filename,"%c:xdis%d.dat",DR,pass);
    if ((fxfloat=fopen(filename,"wb")) == NULL) {
      printf("Error creating %s!\n", filename);
      exit(1);
    }
    /* Y view floating output file */
    sprintf(filename,"%c:ydis%d.dat",DR,pass);
    if ((fyfloat=fopen(filename,"wb")) == NULL) {
      printf("Error creating %s!\n", filename);
      exit(1);
    }
    for(i=0;i<256;i++)
        for(j=0;j<256;j++)fzbuf[i][j]=256; /* clear Z view buffer */
    
    for(z=start_slice; z!=end_slice; z += zdir){  /* For each Slice */
      /* open next two slice files */
	  setfilename(z);
      if ((fn[0]=fopen(fnamein,"rb")) == NULL) continue;
	for (filegap = zdir; filegap + z != end_slice+zdir; filegap+=zdir) {
          setfilename(z+filegap);
          if ((fn[1]=fopen(fnamein,"rb")) != NULL) break;
	  }
      if (fn[1] == NULL) continue;

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
	    for(i=0;i<256;i++) fxbuf[j][i]=fybuf[j][i]=256;
      /* set index for zbuf */
      zbfy = zbufystart;
      /* set index for xbuf */
      xbfx = xbufxstart;
      for(y=ystart; y!=ystop; y+=ydir){              /* For each line */
	/* skip to line*/
	for(i=0;i<2;i++)fseek(fn[i],(long)512*(y+header_blocks),SEEK_SET);
        fread(buffer[line][0],1,512,fn[0]);   
        fread(buffer[line][inter],1,512,fn[1]);
        interpolate(line,0,inter,inter); /* interpolate in_between */
	for(i=0;i<inter;i++){   /* For each interpolation line */
          /* set index for zbuf */
	  zbfx = zbufxstart;
          /* set index for ybuf */
          ybfx = ybufxstart;
	  for(x=xstart; x!=xstop; x+=xdir) { /* For each Voxel value */
            /* find threshold transition*/
	    if (buffer[line][i+1][x] >= THRESHOLD) {
              /* if first transition in X direction get floating
	             distance */
              if(fxbuf[i][xbfx]==256.0) fxbuf[i][xbfx]=(x==xstart)?0:
                                    (x-xstart)*xdir-1+
                     midpoint((float)buffer[line][i+1][x],
                            (float)buffer[line][i+1][x-xdir]);

		      /* if first transition in Y direction get floating
                   distance */
              if(fybuf[i][ybfx]==256.0) fybuf[i][ybfx]=(y==ystart)?0:
                    (y-ystart)*ydir-1+
                     midpoint((float)buffer[line][i+1][x],
                              (float)buffer[1-line][i+1][x]);
 
	          /* if first transition in Z direction get floating
                   distance */
              if(fzbuf[zbfy][zbfx]==256.0) fzbuf[zbfy][zbfx]=
                   (i==0) && (buffer[line][i][x]>=THRESHOLD) ?
                    NLINES : NLINES+i+
                    midpoint((float)buffer[line][i+1][x],
                             (float)buffer[line][i][x]);
            }
            /* change index of ybuf and zbuf */
            ybfx += ybufxdir;
	    zbfx += zbufxdir;
	    if ((ybfx < 0 || ybfx > 255) && x+xdir != xstop) {
              printf("Error in index YBFX!\n");
              exit(1);
            }
	    if ((zbfx < 0 || zbfx > 255) && x+xdir != xstop) {
              printf("Error in index ZBFX!\n");
              exit(1);
            }
          }
        }
        line=1-line;  /* change current input buffer line */
        /* update index of xbuf and zbuf */
        xbfx += xbufxdir;
        zbfy += zbufydir;
	if ((zbfy < 0 || zbfy > 255) && y+ydir != ystop) {
          printf("Error in index ZBFY!\n");
          exit(1);
        }
	if ((xbfx < 0 || xbfx > 255) && y+ydir != ystop) {
          printf("Error in index XBFX!\n");
          exit(1);
        }
      }
      NLINES+=inter;           /* increment number of output lines */

      fclose(fn[0]);           /* close slice files */
      fclose(fn[1]);
      /* write output lines to files */
      if (fwrite(fxbuf,1,sizeof(float)*inter*256,fxfloat) <
		inter*256*sizeof(float))
	printf("Error writing to FXBUF!\n");
      if (fwrite(fybuf,1,sizeof(float)*inter*256,fyfloat) <
		inter*256*sizeof(float))
	printf("Error writing to FYBUF!\n");
      printf(" line %d\r", z);
    }
    printf("\n");
    fclose(fxfloat); /* close X,Y floating files */
    fclose(fyfloat);
    /* write Z floating file */
    sprintf(filename,"%c:zdis%d.dat",DR,pass);
    fzfloat=fopen(filename,"wb");
    for(i=0;i<256;i++)fwrite(fzbuf[i],sizeof(float),256,fzfloat);
    fclose(fzfloat);

    /* should we reverse the order of the lines? */
    if (xbufydir < 0) {
      sprintf(filename,"%c:xdis%d.dat",DR,pass);
      fxfloat=fopen(filename,"rb");
      for (line = 0; line < NLINES; line++)
        fread(fzbuf[line], 1, sizeof(float)*256, fxfloat);
      fclose (fxfloat);
      fxfloat=fopen(filename,"wb");
      for (line = 1; line <= NLINES; line++)
        fwrite(fzbuf[NLINES-line], sizeof(float), 256, fxfloat);
      fclose (fxfloat);
    }
    if (ybufydir < 0) {
      sprintf(filename,"%c:ydis%d.dat",DR,pass);
      fyfloat=fopen(filename,"rb");
      for (line = 0; line < NLINES; line++)
        fread(fzbuf[line], 1, sizeof(float)*256, fyfloat);
      fclose (fyfloat);
      fyfloat=fopen(filename,"wb");
      for (line = 1; line <= NLINES; line++)
        fwrite(fzbuf[NLINES-line], sizeof(float), 256, fyfloat);
      fclose (fyfloat);
    }

}

unsigned char grad(y1,y2,z1,z2,y_factor,z_factor)
float y1,y2,z1,z2;
int y_factor,z_factor;
{
    float gx,gy,gz;
    unsigned char gxint;
    /* get z and y components of gradient */
    gz=(z1-z2)/(float) z_factor;
    gy=(y1-y2)/(float) y_factor;
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
{
    int i;
    float y1, y2, z1, z2;
    int factor;
    /* do first pixel in line */
    i = 0;
    if(fxbuf[line][0]==256)lineg[0]=lined[0]=0;
    else{
        if (dispmode & 1)
          lined[0]=255-fxbuf[line][0]; /*distance shade */
        if (dispmode & 2)
          lineg[0]=grad(fxbuf[line][0],fxbuf[line][1],fxbuf[prevline][0],
                        fxbuf[succline][0],1,z_factor);
    }
    /* do rest of pixels inside line */
    for(i=1;i<255;i++)
      if(fxbuf[line][i]==256)lineg[i]=lined[i]=0;
    else{
        if (dispmode & 1)
          lined[i]=255-fxbuf[line][i]; /*distance shade */
        if (dispmode & 2) {
          y1 = fxbuf[line][i-1];
          y2 = fxbuf[line][i+1];
          factor = 2;
          if (fabs(y1 - y2) > GRAD_THRESHOLD) {
            if (fabs(y1 - fxbuf[line][i]) < GRAD_THRESHOLD/2.) {
              y2 = fxbuf[line][i];
              factor = 1;
            }
            else if (fabs(fxbuf[line][i] - y2) < GRAD_THRESHOLD/2.) {
              y1 = fxbuf[line][i];
              factor = 1;
            }
          }
          z1 = fxbuf[prevline][i];
          z2 = fxbuf[succline][i];
          if (z_factor == 2 && fabs(z1 - z2) > GRAD_THRESHOLD) {
            if (fabs(z1 - fxbuf[line][i]) < GRAD_THRESHOLD/2.) {
              z2 = fxbuf[line][i];
              z_factor = 1;
            }
            else if (fabs(fxbuf[line][i] - z2) < GRAD_THRESHOLD/2.) {
              z1 = fxbuf[line][i];
              z_factor = 1;
            }
          }
 
/*        lineg[i]=grad(fxbuf[line][i-1],fxbuf[line][i+1],fxbuf[prevline][i],
        fxbuf[succline][i],2,z_factor); */
          lineg[i]=grad(y1, y2, z1, z2, factor, z_factor);
        }
    }
    /* do last pixel in line */
    if(fxbuf[line][255]==256) lineg[255]=lined[255]=0;
    else{
        if (dispmode & 1)
          lined[255]=255-fxbuf[line][255]; /*distance shade */
        if (dispmode & 2)
          lineg[255]=grad(fxbuf[line][255],fxbuf[line][254],
                          fxbuf[prevline][255],
                          fxbuf[succline][255],1,z_factor);
    }
    if (dispmode & 2)
      fwrite(lineg,1,256,fg); /* write to output files */
    if (dispmode & 1)
      fwrite(lined,1,256,fd);
}

/* create an gradient and distance shaded view */
doviews(namedis,nameg,named,nlines)
char *namedis,*nameg,*named; /* floating file , gradiet and distance files*/
int nlines;                  /* number of lines in image */
{
    FILE *fg,*fd,*ffloat;
    int z,i,j,k,midline;
    char lined[256],lineg[256]; /* gradient and distance value buffers */
    midline=1;                  /* middle line in input buffer */
    /* open output and input files */
    fd = fg = NULL;
    if (dispmode & 1)
      fd=fopen(named,"wb");
    if (dispmode & 2)
      fg=fopen(nameg,"wb");
    ffloat=fopen(namedis,"rb");
    fread(fxbuf,1,3*256*sizeof(float),ffloat); /* read first three floating lines */
    /* do first line */
    doline(lineg,lined,0,0,1,1,fg,fd);
    /* do rest of lines */
    for(z=0;z<(nlines-2);z++){      /*for each inside line */
        doline(lineg,lined,midline,prev(midline),succ(midline),2,fg,fd);
        fread(fxbuf[prev(midline)],1,256*sizeof(float),ffloat); /*read next floating line */
        midline=succ(midline);
    }
    /* do last line */
    doline(lineg,lined,midline,prev(midline),midline,1,fg,fd);
    /* close all files */
    if (fg != NULL) fclose(fg);
    if (fd != NULL) fclose(fd);
    fclose(ffloat);
}

/**********************************************************/
/**** MAIN ***** MAIN ***** MAIN ***** MAIN ***** MAIN ****/
/**********************************************************/
/* Usage: grad [filename] [-f] [-l] [-z] [-t] [-d] [-n(d|g)] [-g]
			  [-h] [-v(bo|to|rl|ll|re|fr)] [-c(rl|ll|re|fr)]
			  [-i(x(r|l)|y(u|d))] [-o(x(r|l)|y(f|b)|z(h|f))]
 */
main(argc, argv)
int argc;
char *argv[];
{   char  filename[13];
    FILE *par;
    FILE *fp;
    int   i, n;
    char  cmd[80];
    int   scanxdir[3], scanydir[3], scanzdir[3];  /* scanning directions */
    int   imagex[4], imagey[4], imagez[4];        /* image/scan direction */
    int   temp_nlines;                            /* temporary line numbers */

    /* set clipping planes */
    clipx[0] = clipy[0] = 0;
    clipx[1] = clipy[1] = 256;
    /* set image orientation to left->right, down->up */
    image_or[0] = image_or[1] = 1;
    /* set object orientation to right->left, back->front, feet->head */
    object_or[0] = object_or[1] = object_or[2] = 1;
    /* get parameters from the command line */
    /* set other defaults */
    ZOOM = 1.0;
    FIRSTSLICE = 1;
    LASTSLICE = 256;
    THRESHOLD = 175;
    DR = 'D';
    dispmode = 3;
    header_blocks = 1;
    if (argc > 1) {
      /* get parameters */
      for (argc--, argv++; argc > 0; argc--, argv++) {
        if (**argv == '-' || **argv == '/') {
          switch (tolower(*(*argv+1))) {
            case 'z':
              ZOOM = atof(*argv+2);
              break;
            case 'f':
              FIRSTSLICE = atoi(*argv+2);
              break;
            case 'l':
              LASTSLICE = atoi(*argv+2);
              break;
            case 't':
              THRESHOLD = atoi(*argv+2);
              break;
            case 'g':
              GRAD_THRESHOLD = atof(*argv+2);
              break;
	    case 'd':
	      DR = *(*argv+2);
	      break;
	    case 'h':
	      header_blocks = atoi(*argv+2);
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
	    case 'v':
	      /* viewing directions */
	      if (views == 0xFFFF) views = 0;
	      if (tolower(*(*argv+2)) == 'b' &&
		  tolower(*(*argv+3)) == 'o')
		views |= 1;
	      else if (tolower(*(*argv+2)) == 't' &&
		       tolower(*(*argv+3)) == 'o')
		views |= 1 << 1;
	      else if (tolower(*(*argv+2)) == 'r' &&
		       tolower(*(*argv+3)) == 'l')
		views |= 1 << 2;
	      else if (tolower(*(*argv+2)) == 'l' &&
		       tolower(*(*argv+3)) == 'l')
		views |= 1 << 3;
	      else if (tolower(*(*argv+2)) == 'r' &&
		       tolower(*(*argv+3)) == 'e')
		views |= 1 << 4;
	      else if (tolower(*(*argv+2)) == 'f' &&
		       tolower(*(*argv+3)) == 'r')
		views |= 1 << 5;
	      else if (tolower(*(*argv+2)) == 'n' &&
		       tolower(*(*argv+3)) == 'o')
		    views = 0;
	      else
		usage();
	      break;
	    case 'c':
	      /* clipping planes */
	      if (tolower(*(*argv+2)) == 'r' &&
		  tolower(*(*argv+3)) == 'l')
		clipx[0] = atoi(*argv + 4);
	      else if (tolower(*(*argv+2)) == 'l' &&
		       tolower(*(*argv+3)) == 'l')
		clipx[1] = atoi(*argv + 4);
	      else if (tolower(*(*argv+2)) == 'r' &&
		       tolower(*(*argv+3)) == 'e')
		clipy[0] = atoi(*argv + 4);
	      else if (tolower(*(*argv+2)) == 'f' &&
		       tolower(*(*argv+3)) == 'r')
		clipy[1] = atoi(*argv + 4);
	      else
		usage();
	      break;
	    case 'i':
	      /* image orientation */
	      if (tolower(*(*argv+2)) == 'x' &&
		  tolower(*(*argv+3)) == 'r')
		image_or[0] = 1;
	      else if (tolower(*(*argv+2)) == 'x' &&
		  tolower(*(*argv+3)) == 'l')
		image_or[0] = -1;
	      else if (tolower(*(*argv+2)) == 'y' &&
		       tolower(*(*argv+3)) == 'u')
		image_or[1] = 1;
	      else if (tolower(*(*argv+2)) == 'y' &&
		       tolower(*(*argv+3)) == 'd')
		image_or[1] = -1;
	      else
		usage();
	      break;
	    case 'o':
	      /* object orientation */
	      if (tolower(*(*argv+2)) == 'x' &&
		  tolower(*(*argv+3)) == 'r')
		object_or[0] = -1;
	      else if (tolower(*(*argv+2)) == 'x' &&
		  tolower(*(*argv+3)) == 'l')
		object_or[0] = 1;
	      else if (tolower(*(*argv+2)) == 'y' &&
		       tolower(*(*argv+3)) == 'f')
		object_or[1] = 1;
	      else if (tolower(*(*argv+2)) == 'y' &&
		       tolower(*(*argv+3)) == 'b')
		object_or[1] = -1;
	      else if (tolower(*(*argv+2)) == 'z' &&
		       tolower(*(*argv+3)) == 'h')
		object_or[2] = 1;
	      else if (tolower(*(*argv+2)) == 'z' &&
		       tolower(*(*argv+3)) == 'f')
		object_or[2] = -1;
	      else
		usage();
	      break;
	    default:
	      usage();
          }
        }
        else {
          strncpy(fnamein, *argv, 45);
	  strcat(fnamein, ".000");
        }
      }
    }
    else {
      cmd[0] = 1;
      while(cmd[0]) {
	/* display defaults */
	printf("1 : filename          - %s\n", fnamein);
	printf("2 : first slice       - %d, last slice - %d\n", FIRSTSLICE,
								LASTSLICE);
	printf("3 : zoom              - %f\n", ZOOM);
	printf("4 : dens. threshold   - %d\n", THRESHOLD);
	printf("5 : temp. drive       - %c\n", toupper(DR));
	printf("6 : display           - (%s) (%s)\n",
	       (dispmode & 1) ? "distance" : "",
	       (dispmode & 2) ? "gradient" : "");
	printf("7 : grad. threshold   - %f\n", GRAD_THRESHOLD);
	printf("8 : header blocks     - %d\n", header_blocks);
	printf("9 : views             - %s %s %s %s %s %s %s\n",
	       (views & 1) ? "BO" : "",
	       (views & 1<<1) ? "TO" : "",
	       (views & 1<<2) ? "RL" : "",
	       (views & 1<<3) ? "LL" : "",
	       (views & 1<<4) ? "RE" : "",
	       (views & 1<<5) ? "FR" : "",
	       (views == 0) ? "NO" : "");
	printf("10: clipping planes   - RL %d LL %d RE %d FR %d\n",
	       clipx[0], clipx[1], clipy[0], clipy[1]);
	printf("11: image orientation - X %d Y %d\n",
	       image_or[0], image_or[1]);
	printf("12: objct orientation - X %d Y %d Z %d\n",
	       object_or[0], object_or[1], object_or[2]);
	printf("\n Enter number to change any item, or ENTER to start\n");
	if (gets(cmd) == NULL) exit(0);
	if (cmd[0]) {
	  for (i = 0; !isdigit(cmd[i]) && cmd[i] != '\0'; i++) ;
	  if (isdigit(cmd[i])) {
	    sscanf(&cmd[i], "%d", &i);
	    switch (i) {
	      case 1:
		printf("Enter file name : ");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%s", fnamein);
		strcat(fnamein, ".000");
		break;
	      case 2:
		printf("Enter first scan : ");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%d", &FIRSTSLICE);
		printf("Enter last scan : ");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%d", &LASTSLICE);
		break;
	      case 3:
		printf("Enter zoom factor : ");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%f", &ZOOM);
		break;
	      case 4:
		printf("Enter threshold : ");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%d", &THRESHOLD);
		break;
	      case 5:
		printf("Enter temporary drive : ");
		if (gets(cmd) == NULL) exit(0);
		for (i = 0; !isalpha(DR = cmd[i]) && DR != '\0'; i++) ;
		if (!isalpha(DR)) DR = 'd';
		break;
	      case 6:
		printf("1 to turn distance %s, and 2 gradient %s\n",
		       (dispmode & 1) ? "off" : "on",
		       (dispmode & 2) ? "off" : "on");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%d", &i);
		dispmode ^= 1 << --i;
		break;
	      case 7:
		printf("Enter gradient threshold : ");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%f", &GRAD_THRESHOLD);
		break;
	      case 8:
		printf("Enter number of header blocks : ");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%d", &header_blocks);
		break;
	      case 9:
		printf("1 BO %s, 2 TO %s, 3 RL %s, 4 LL %s, 5 RE %s, 6 FR %s, 7 NO %s ",
		       (views & 1) ? "off" : "on",
		       (views & 1<<1) ? "off" : "on",
		       (views & 1<<2) ? "off" : "on",
		       (views & 1<<3) ? "off" : "on",
		       (views & 1<<4) ? "off" : "on",
		       (views & 1<<5) ? "off" : "on",
		       (views == 0) ? "off" : "on");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%d", &i);
		if (i == 7)
		  views == 0;
		else
          views ^= 1 << --i;
		break;
	      case 10:
		printf("Enter for clipping: 1 RL, 2 LL, 3 RE, 4 FR : ");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%d", &i);
		if (i > 0 && i < 5) {
		  printf("Enter clipping : ");
		  if (gets(cmd) == NULL) exit(0);
		  sscanf(cmd, "%d", &n);
		  switch (i) {
		    case 1:
		      clipx[0] = n;
		      break;
		    case 2:
		      clipx[1] = n;
		      break;
		    case 3:
		      clipy[0] = n;
		      break;
		    case 4:
		      clipy[1] = n;
		      break;
		  }
		}
		break;
	      case 11:
		printf("Enter for image orienation: 1 X, 2 Y : ");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%d", &i);
		if (i > 0 && i < 3)
		  image_or[i-1] *= -1;
		break;
	      case 12:
		printf("Enter for object orienation: 1 X, 2 Y, 3 Z : ");
		if (gets(cmd) == NULL) exit(0);
		sscanf(cmd, "%d", &i);
		if (i > 0 && i < 4)
		  object_or[i-1] *= -1;
		break;
	      default:
		printf("Unvalid command!\n");
		break;
	    }
	  }

	}
      }
    }
    THRESHOLD+=1024;

  /* test parameters */
  if (ZOOM > 5.0 || ZOOM < 1.0) {
    printf("ZOOM must not be smaller than 1 or larger than 5!\n");
    exit(1);
  }
  if (FIRSTSLICE < 1 || LASTSLICE <= FIRSTSLICE) {
    printf("FIRSTSLICE and LASTSLICE are not within valid range!\n");
    exit(1);
  }
  for (i = FIRSTSLICE; i <= LASTSLICE; i++) {
    setfilename(i);
    if ((fp = fopen(fnamein, "rb")) == NULL) continue;
    fclose(fp);
    break;
  }
  if (i > LASTSLICE) {
    setfilename(FIRSTSLICE);
    printf("No files with a name between %s and", fnamein);
    setfilename(LASTSLICE);
    printf(" %s!\n", fnamein);
    exit(1);
  }
  else
    FIRSTSLICE = i;
  for (i = LASTSLICE; i >= FIRSTSLICE; i--) {
    setfilename(i);
    if ((fp = fopen(fnamein, "rb")) == NULL) continue;
    fclose(fp);
    break;
  }
  LASTSLICE = i;
  if (LASTSLICE <= FIRSTSLICE) {
    printf("FIRSTSLICE and LASTSLICE are not within valid range!\n");
    exit(1);
  }
  if (GRAD_THRESHOLD < 1.0) {
    printf("GRAD_THRESHOLD is too small!\n");
    exit(1);
  }
  if (!isalpha(DR)) {
    printf("Temporary drive has to be a letter!\n");
    exit(1);
  }
  if (header_blocks < 0) {
    printf("Number of header blocks has to be positive!\n");
    exit(1);
  }
  if (clipx[0] < 0 || clipx[1] > 256 || clipx[1] <= clipx[0]) {
    printf("Lateral clipping planes are not within valid range!\n");
  }
  if (clipy[0] < 0 || clipy[1] > 256 || clipy[1] <= clipy[0]) {
    printf("Frontal clipping planes are not within valid range!\n");
  }

  /* temporary number of lines in output is used in finding
     traversal of slices */
  temp_nlines = ((float)(LASTSLICE-FIRSTSLICE)*ZOOM+.5);

  /* get floating distance values (first pass over data)*/
    if (!views || (views & 1) || (views & 1<<2) || (views & 1<<4)) {
      /* set scan start/end/directions depending on object/image
         orientations */
      scanxdir[0] = clipx[0];
      scanxdir[1] = clipx[1];
      scanxdir[2] = 1;
      scanydir[0] = clipy[0];
      scanydir[1] = clipy[1];
      scanydir[2] = 1;
      scanzdir[0] = FIRSTSLICE;
      scanzdir[1] = LASTSLICE;
      scanzdir[2] = 1;
      imagex[0] = clipy[0];
      imagex[1] = 1;
      imagex[2] = 0;
      imagex[3] = 1;
      imagey[0] = 255 - clipx[0];
      imagey[1] = -1;
      imagey[2] = 0;
      imagey[3] = 1;
      imagez[0] = clipx[0];
      imagez[1] = 1;
      imagez[2] = clipy[0];
      imagez[3] = 1;
      if (object_or[0] == -1) {
        scanxdir[0] = clipx[1]-1;
        scanxdir[1] = clipx[0]-1;
        scanxdir[2] = -1;
        imagey[0] = clipx[1]-1;
        imagez[0] = 256 - clipx[1];
      }
      if (object_or[1] == -1) {
        scanydir[0] = clipy[1]-1;
        scanydir[1] = clipy[0]-1;
        scanydir[2] = -1;
        imagex[0] = 256 - clipy[1];
        imagez[2] = 256 - clipy[1];
      }
      if (object_or[2] == -1) {
        scanzdir[0] = LASTSLICE;
        scanzdir[1] = FIRSTSLICE;
        scanzdir[2] = -1;
      }
      if (image_or[0] == -1) {
        imagex[0] = 255-imagex[0];
        imagex[1] *= -1;
        imagey[0] = 255-imagey[0];
        imagey[1] *= -1;
        imagez[0] = 255-imagez[0];
        imagez[1] *= -1;
      }
      if (image_or[1] == -1) {
        imagex[2] = temp_nlines-imagex[2];
        imagex[3] *= -1;
        imagey[2] = temp_nlines-imagey[2];
        imagey[3] *= -1;
        imagez[2] = 255-imagez[2];
        imagez[3] *= -1;
      }

      printf("starting forward pass on data\n");
      getdistances(scanxdir[0], scanxdir[1], scanxdir[2],
		           scanydir[0], scanydir[1], scanydir[2],
                   scanzdir[0], scanzdir[1], scanzdir[2],
                   imagex[0], imagex[1], imagex[2], imagex[3],
                   imagey[0], imagey[1], imagey[2], imagey[3],
                   imagez[0], imagez[1], imagez[2], imagez[3],
                   1);
      /* create images */
      if (views & 1) {
	printf("doing bottom (Z) view(s)\n");
        sprintf(filename,"%c:zdis%d.dat",DR,1);
        doviews(filename,"gbo.out","dbo.out",256);
      }
      if (views & 1<<2) {
	printf("doing right lateral (X) view(s)\n");
        sprintf(filename,"%c:xdis%d.dat",DR,1);
        doviews(filename,"grl.out","drl.out",NLINES);
      }
      if (views & 1<<4) {
	printf("doing rear (Y) view(s)\n");
        sprintf(filename,"%c:ydis%d.dat",DR,1);
        doviews(filename,"gre.out","dre.out",NLINES);
      }
    }
 /* get floating distance values (second pass over data)*/
    if (!views || (views & 1<<1) || (views & 1<<3) || (views & 1<<5)) {
      /* set scan start/end/directions depending on object/image
         orientations */
      scanxdir[0] = clipx[1]-1;
      scanxdir[1] = clipx[0]-1;
      scanxdir[2] = -1;
      scanydir[0] = clipy[1]-1;
      scanydir[1] = clipy[0]-1;
      scanydir[2] = -1;
      scanzdir[0] = LASTSLICE;
      scanzdir[1] = FIRSTSLICE;
      scanzdir[2] = -1;
      imagex[0] = 256-clipy[1];
      imagex[1] = 1;
      imagex[2] = temp_nlines;
      imagex[3] = -1;
      imagey[0] = clipx[1]-1;
      imagey[1] = -1;
      imagey[2] = temp_nlines;
      imagey[3] = -1;
      imagez[0] = 256-clipx[1];
      imagez[1] = 1;
      imagez[2] = clipy[1]-1;
      imagez[3] = -1;
      if (object_or[0] == -1) {
        scanxdir[0] = clipx[0];
        scanxdir[1] = clipx[1];
        scanxdir[2] = 1;
        imagey[0] = 255 - clipx[0];
        imagez[0] = clipx[0];
      }
      if (object_or[1] == -1) {
        scanydir[0] = clipy[0];
        scanydir[1] = clipy[1];
        scanydir[2] = 1;
        imagex[0] = clipy[0];
        imagez[2] = 255 - clipy[0];
      }
      if (object_or[2] == -1) {
        scanzdir[0] = FIRSTSLICE;
        scanzdir[1] = LASTSLICE;
        scanzdir[2] = 1;
      }
      if (image_or[0] == -1) {
        imagex[0] = 255-imagex[0];
        imagex[1] *= -1;
        imagey[0] = 255-imagey[0];
        imagey[1] *= -1;
        imagez[0] = 255-imagez[0];
        imagez[1] *= -1;
      }
      if (image_or[1] == -1) {
        imagex[2] = temp_nlines-imagex[2];
        imagex[3] *= -1;
        imagey[2] = temp_nlines-imagey[2];
        imagey[3] *= -1;
        imagez[2] = 255-imagez[2];
        imagez[3] *= -1;
      }
      printf("starting backward pass on data\n");
      getdistances(scanxdir[0], scanxdir[1], scanxdir[2],
		           scanydir[0], scanydir[1], scanydir[2],
                   scanzdir[0], scanzdir[1], scanzdir[2],
                   imagex[0], imagex[1], imagex[2], imagex[3],
                   imagey[0], imagey[1], imagey[2], imagey[3],
                   imagez[0], imagez[1], imagez[2], imagez[3],
                   2);
      /* create images */
      if (views & 1<<1) {
	printf("doing top (Z) view(s)\n");
        sprintf(filename,"%c:zdis%d.dat",DR,2);
        doviews(filename,"gto.out","dto.out",256);
      }
      if (views & 1<<3) {
	printf("doing left lateral (X) view(s)\n");
        sprintf(filename,"%c:xdis%d.dat",DR,2);
        doviews(filename,"gll.out","dll.out",NLINES);
      }
      if (views & 1<<5) {
	printf("doing front (Y) view(s)\n");
        sprintf(filename,"%c:ydis%d.dat",DR,2);
        doviews(filename,"gfr.out","dfr.out",NLINES);
      }
    }
    printf("number of lines = %d\n",NLINES);

    /* write temporary drive and number of lines */ 
    par=fopen("param.dat","w");
    fprintf(par,"%c %d\n",DR,NLINES);
    /* write views */
    if (views & 1)
      fprintf(par, "BO ");
    if (views & (1 << 1))
      fprintf(par, "TO ");
    if (views & (1 << 2))
      fprintf(par, "RL ");
    if (views & (1 << 3))
      fprintf(par, "LL ");
    if (views & (1 << 4))
      fprintf(par, "RE ");
    if (views & (1 << 5))
      fprintf(par, "FR ");
    if (!views)
      fprintf(par, "NO ");
    fprintf(par, "\n");
    /* write clipping planes */
    fprintf(par, "RL %d LL %d RE %d FR %d\n", clipx[0], clipx[1],
						clipy[0], clipy[1]);
    /* write image and object orientation */
    fprintf(par, "image: x %2d  y %2d\n", image_or[0], image_or[1]);
    fprintf(par, "object: x %2d  y %2d  z %2d\n",
	    object_or[0], object_or[1], object_or[2]);
    fclose(par);
}


usage()
{
  printf("%s\n", usestr1);
  printf("%s\n", usestr2);
  printf("%s\n", usestr3);
  exit(1);
}
