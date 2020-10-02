/**********************  gx.c  ******************************


	Three Dimensional Reconstruction Of Medical
	Images from Serial Slices - CT, MRI, Ultrasound


   This program processess a set of slices images (scans) for one
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

	This program may be copied and used freely for any
	non-commercial purpose with acknowledgement of the
	developers and inclusion of this notice.

   
*********************************************************/

#include <stdio.h>
#include <math.h>
#define DZ    3.0
#define DX    1.0
#define DY    1.0
#define PI    3.141592653

int outfile,ZMAX,FIRSTSLICE,LASTSLICE,THRESHOLD,
RIGHTMID,LEFTMID,TOPINT,midslice,midline;
int short buffer[3][3][256];

/*            standard 18 output files ( 9 views x 2) */
char *fnamein="ctbild.000",*fgll="gll.out",*fdll="dll.out";
succ(i)
int i;
{
    return(i==2?0:i+1);
}
prev(i)
int i;
{
    return(i==0?2:i-1);
}
setfilename(filenum)
int filenum;
{
    fnamein[7]=filenum/100+'0';
    fnamein[8]=(filenum%100)/10+'0';
    fnamein[9]='0'+filenum%10;
}
readline(filenum,line,bufslice,bufline)
int filenum,line,bufslice,bufline;
{
    FILE *fn;
    setfilename(filenum);
    fn=fopen(fnamein,"rb");
    fseek(fn,(long)512*(line+1),SEEK_SET);
    fread(buffer[bufslice][bufline],1,512,fn);
    fclose(fn);
}
readsection(firstfile,bufslice,bufline,line)
int firstfile,bufslice,bufline,line;
{
    int file;
    for(file=firstfile;file<firstfile+3;file++){
        readline(file,line,bufslice,bufline);
        bufslice=(bufslice==2)?0:bufslice+1;
    }
}
readblock(firstfile)
int firstfile;
{
    int file,i;
    FILE *fn;
    for(file=firstfile,i=0;i<3;i++,file++){
        setfilename(file);
        fn=fopen(fnamein,"rb");
        fseek(fn,(long)512,SEEK_SET);
        fread(buffer[i],1,1536,fn);
        fclose(fn);
    }
}
/******************* VAR1X ****************************/
/*place of change on x axis reference to point (positive search) */
double var1x(x,y,z,zero)
int x,y,z,zero;
{
    int i;
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
{
    double sy[2],sz[2],gx,gy,gz;
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

/**********************************************************/
/**** MAIN ***** MAIN ***** MAIN ***** MAIN ***** MAIN ****/
/**********************************************************/
main()
{
    int x,y,z,i,j,k,r;
    FILE *fg,*fd;
    unsigned char lined[256],lineg[256];
    midslice=1;   
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
    fd=fopen(fdll,"wb");
    fg=fopen(fgll,"wb");
    printf(" opened %d %d \n",fg,fd);
    /* read first 3 scans */
    readblock(FIRSTSLICE);

    /* first pass on scan data (forward) */
    printf("Begining computation of REAR,LEFT,REAR,and LEFT-MID views\n");
    for(z=FIRSTSLICE+1;z<LASTSLICE;z++){                 /*for each slice */
        for(i=0;i<256;i++){
            lineg[i]=0;
            lined[i]=0;
        }
        midline=1;
        for(y=1;y<255;y++){ /*for each line*/
            for(x=1;(x<255)&&(buffer[midslice][midline][x]<THRESHOLD);x++);
            if(x<255){
                lineg[y]=getgradx(0,x,midline,midslice,0);
                lined[y]=255-x;
            }
            if(y<254) readsection(z-1,0,prev(midline),y+2);
            midline=succ(midline);
        }
        fwrite(lineg,1,256,fg);
        fwrite(lined,1,256,fd);
        printf("did slice %d\n",z);
        if(z<LASTSLICE-1) readblock(z);
    }
    fclose(fg);
    fclose(fd);
