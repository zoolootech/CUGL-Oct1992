	#INCLUDE	GRAPH.H
	#INCLUDE	SHOOT.H
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
int	toggle;	/* a toggle for page flipping */
shoot()
{
int	*scrn,n,xr,yr,*object;
int	*mscan;
int	score;
char	*prx,*pry;
	prx = JOYRX;
	pry = JOYRY;
 
*flop = 2;	/* clock interrupt counter = 2 */
toggle = 0;
score = 0;
xr =1;
yr = 1;
init();		/* set up the graphics lists */
while(1) {
	if(firer())	{
		remclk();
		return;		/* exit */
	}
release(PLANE,PLNX0,PLNY0,VPLNX,0);
while(posx(PLANE) < PLNXM){	/* until the plane runs off the screen */
	xr = (*prx)*3 + GUNOFF;
	release(GUN,xr,GUNY0,0,0);
	move(PLANE);
	if(blanked(BULLET)) { /* a bullet is not active */
		if(firel()) {	/* if left fire button depressed */
			release(BULLET,xr,(GUNY0 - 3),0,VBULY);
		}
	}
	else {	/* a bullet is active */
		move(BULLET);
		if(hit(BULLET,PLANE)) {
			blank(BULLET);
			score++;
			replace(PLANE,BOOM);
			display();
			*flop = 60; /* delay time for boom display */
			while(*flop);
			replace(BOOM,PLANE);
			release(PLANE,PLNX0,PLNY0,VPLNX,0);
		}
		if(posy(BULLET) < PLNY0) blank(BULLET);
	}
	display();
     }
  }
}
display()	/* display the next page */
{
	if(toggle) {
		cwriter(UPPG0,master,lpage0,PAGE0);
		toggle = 0;
		while(*flop != NULL); /* wait for page flip time */
		*flop = NTICKS;
		setscrn(PAGE0);
	}
	else {
		cwriter(UPPG1,master,lpage1,PAGE1);
		toggle = 1;
		while(*flop != NULL);
		*flop = NTICKS;
		setscrn(PAGE1);
	}
}
release(objnum,x,y,vx,vy) /* set object in position with velocity */
int	objnum,x,y,vx,vy;
{
int	*loc;
	if((loc=locate(master,objnum))==NULL) return(-1);/* error */
	loc[X]=x;
	loc[Y]=y;
	loc[XINCR]=vx;
	loc[YINCR]=vy;
	loc[FLAGS] = MOVER | UPALL;
	return(NULL);
}
move(objnum)	/* move the specified object */
int	objnum;
{
int	*loc;
	if((loc=locate(master,objnum))==NULL) return(-1);
	loc[X]= loc[X] + loc[XINCR];
	loc[Y] = loc[Y] + loc[YINCR];
	loc[FLAGS] = loc[FLAGS] | UPALL;
}
firel()	/* test the state of left fire button. Return 1 if on */
{
char	*pstate;
	pstate = SWPORT;
	if(((*pstate)&SWTR) != NULL) return(NULL);
	else	  return(1);
}
firer()
{
char	*pstate;
	pstate = SWPORT;
	if(((*pstate)&SWTL) != NULL) return(NULL);
	else	return(1);
}
blank(objnum)	/* turn off (blank) an object */
int	objnum;
{
int 	*loc;
	if((loc=locate(master,objnum)) == NULL) return(-1);
	loc[FLAGS] = BLANKED | UPALL;
	return(NULL);
}
blanked(objnum)	/* TESTS for object blanked */
int	objnum;
{
int	*loc;
	if((loc=locate(master,objnum)) == NULL)return(1);
	return((loc[FLAGS])&BLANKED);
}
posx(objnum)
int	objnum;
{
int	*loc;
	if((loc=locate(master,objnum)) == NULL)return(-1);
	return(loc[X]);
}
posy(objnum)
int	objnum;
{
int	*loc;
	if((loc=locate(master,objnum)) == NULL) return(-1);
	return(loc[Y]);
}
hit(proj,targ)
int	proj,targ;
{
int	*pproj,*ptarg;
	if((pproj=locate(master,proj)))
		if((ptarg=locate(master,targ))) {
			return(overlap(pproj,ptarg));
		}
	return(NULL);
}
replace(oldobj,newobj)	/* change the displayed object to the selected one */
int	oldobj,newobj;
{
int	*from,*to;
	if((from=locate(master,oldobj)))
		if((to=locate(master,newobj))){
			from[FLAGS]=BLANKED | UPALL ;
			to[FLAGS] = UPALL;
			to[X]=from[X];
			to[Y]=from[Y];
			to[XINCR]=from[XINCR];
			to[YINCR]=from[YINCR];
			return(NULL);
		}
	return(-1);
}
locate(plist,objnum)
int	*plist,objnum;
{
	while(plist[FORWARD] != NULL) {
		if(plist[NUMBER] != objnum) {
			plist = plist[FORWARD];
		}
		else	return(plist);
	}
	return(NULL);
}
cwriter(upflag,mlist,page,screen)
int	upflag,*mlist,*page,*screen;
{
 
 
	pass0(upflag,mlist,page,screen);
	pass2(mlist,page,screen);
}
	#INCLUDE	shootini.c
	#INCLUDE	SHOOT.GPH
	#ASM
	ORG	05C00H
	#ENDASM
	#INCLUDE	fpass0.c
	#INCLUDE	fpass1.c
	#INCLUDE	fpass2.c
	#INCLUDE	LIB
	#INCLUDE	RUN
	#ASM
	LIST	*
	#ENDASM
*
	#ENDASM
ASM
»Õ$Õã$√Ö$>2ä$…>√Å$:ä$∑OÚ°$Ê2Ä$2ä$O
Õ⁄$~