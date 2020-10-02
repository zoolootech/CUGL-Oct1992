	#INCLUDE	GRAPH.H
	#ASM
	ORG	4000H
	LIST	-L
	#ENDASM
	#INCLUDE	PRELUDE
/*
	allocate list storage
*/
int	master[NMASTER];
int	lpage0[NPAGE],lpage1[NPAGE];
/*
	Debugging pointers
*/
int	*gscan,*gscan1,*gxleft,*gxright,*gyupper,*gylower;
int	*gxm,*gym;
joytest()
{
int	*scrn,n,xl,yl,*object;
int	xr,yr;
int	*mscan;
int	toggle;	/* a toggle for page flipping */
char	*plx,*ply;
char	*prx,*pry;
	plx = JOYLX;
	ply = JOYLY;
	prx = JOYRX;
	pry = JOYRY;
 
*flop = 20000;	/* clock interrupt counter = 2 */
toggle = 0;
xr = 1;
xl =1;
yl = 1;
yr = 1;
init();		/* set up the graphics lists */
while(yl < 60) {
	joystick();	/* get joystick co-ordinates	*/
xl = *plx;
yl = *ply;
xr = *prx;
yr = *pry;
mscan=master;
	mscan[X]=xl;
	mscan[Y]=yl;
	mscan[FLAGS]=NULL;
	mscan=mscan[FORWARD];
	mscan[X]=xr;
	mscan[Y]=yr;
	mscan[FLAGS]=NULL;
	if(toggle) {
		cwriter(master,lpage0,PAGE0);
		toggle = 0;
		setscrn(PAGE0);
	}
	else {
		cwriter(master,lpage1,PAGE1);
		toggle = 1;
		setscrn(PAGE1);
	}
}
}
cwriter(mlist,page,screen)
int	*mlist,*page,*screen;
{
 
 
	pass0(mlist,page,screen);
	pass1(mlist,page,screen);
	pass2(mlist,page,screen);
}
	#INCLUDE	cwrtini.c
	#INCLUDE	pass0.c
	#INCLUDE	pass1.c
	#INCLUDE	pass2.c
	#INCLUDE	LIB
	#INCLUDE	RUN
	#ASM
	LIST	*
	#ENDASM
m;
	plist[X]      = x;
	plist[Y]      = y;
	plist[FLAGS]  = flags;
	plist[OBJ