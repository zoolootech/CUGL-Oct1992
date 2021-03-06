	#ASM
	ORG	4000H
	LIST	-L
	#ENDASM
	#INCLUDE	GRAPH.H
	#INCLUDE	SHOOT.H
	#INCLUDE	PRELUDE
/*
	allocate list storage
*/
int	master[NMASTER];
int	lpage0[NPAGE],lpage1[NPAGE];
int	toggle;	/* a toggle for page flipping */
char	*prx,*pry;	/* right x and y joystick pointers */
int	score;
shoot()
{
int	*scrn,n,xr,yr,*object;
int	*mscan;
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
	if(procedure(master)==EOF) break; /* object procedure update */
	display();
	}
}
/*
	procedure(plist)	scans a linked list of graphics records
the head of which is pointed to by plist. If the object is not blanked
(active) and the procedure pointer is not = NULL, the procedure at the 
address in the list will be executed. Eof is returned if there is an
error in procedure(plist), Null otherwise.
*/
procedure(plist)
int	*plist;
{
	int	flag;
	
	flag  = NULL;
	while(1) {
		if((plist[FLAGS] & BLANKED)==NULL)  { /* if object is active */ 
			if(plist[PROC] !=NULL) { /* and has a procedure defined */
		if((flag = prcall(master,plist,plist[PROC])) == EOF) break;
			}
		}
		plist = plist[FORWARD];
		if(plist == NULL) break;
	}
	return(flag);
}
/*
	prcall(x,y,z,...addr) jumps to the address passed on the stack.
	Multiple parameters may be passed if addr is the last. Return is
	via the called subroutine.
*/
prcall(addr)
int 	*addr;
{
	#ASM
	JMP	[2,S]
	#ENDASM
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
/*
	test procedure for airplane
*/
pplane(mlist,plist,proc)
int	*mlist,*plist,*proc;
{
	plist[X] = plist[X] + 1;
	if(plist[X] > PLNXM) {
		plist[FLAGS] = BLANKED | UPALL ;
	}
	else {
		plist[FLAGS] = plist[FLAGS] | UPALL;
	}
	return(NULL);
}
pgun(mlist,plist,proc)
int	*mlist,*plist,*proc;
{
int	vx,vy,x,y ;
 
	x = plist[X];
	y = plist[Y];
	vx = deadzn(*prx);
	vy = deadzn(*pry);
	y  = y + (vy << 1);
	x  = x + (vx << 1);
	bound(plist,0,0,GUNXMX,GUNYMX);
	if(blanked(BULLET)) 
		if(firel()) 
			release(BULLET,x,y,vx * 3,vy * 3);
	plist[X] = x;
	plist[Y] = y;
	plist[FLAGS] = plist[FLAGS] | UPALL;
	return(NULL);
}
pbullet(mlist,plist,proc)
int	*mlist,*plist,*proc;
{
int	*phit, *ptr;
 
	plist[X] = plist[X] + plist[XINCR];
	plist[Y] = plist[Y] + plist[YINCR];
	if(phit=ovlclas(plist,TARGET)) {
		plist[FLAGS] = BLANKED;
		score++;
		replace(phit[OBJNUM],BOOM);
		if(ptr = locate(mlist,BOOM)) {
			ptr[PROC] = pdelay;
			ptr[COUNT] = 60;
		}
	}
	else if(bound(plist,0,0,BULXMX,BULYMX))
		plist[FLAGS] = BLANKED;
	plist[FLAGS] = plist[FLAGS] | UPALL;
	return(NULL);
}
deadzn(val)
char	val;
{
int	v;
	v = 0;
	if(val < JOYMIN) v = -1;
	else if(val > JOYMAX) v = +1;
	return(v);
}
bound(pobj,xmin,ymin,xmax,ymax)
int	*pobj,xmin,ymin,xmax,ymax;
{
int	retval,x,y;
 
	retval = NULL;
	x =pobj[X];
	y = pobj[Y];
	if(x < xmin) { x = xmin;
			retval = 1;
	}
	else if(x > xmax){
		x = xmax;
		retval = 1;
	}
	if(y < ymin) {
		y = ymin;
		retval = 1;
	}
	else if(y >ymax) {
		y = ymax;
		retval = 1;
	}
	pobj[X] = x;
	pobj[Y] = y;
	return(retval);
}
ovlclas(pobj,mlist,class)
int	*pobj,*mlist,class;
{
int	*ptr;
 
	ptr = NULL;
	while(1) {
		if(mlist != pobj)
			if(mlist[CLASS] == class)
				if(overlap(pobj,mlist)) {
					ptr = mlist;
					break;
				}
		if((mlist=mlist[FORWARD]) == NULL) break;
	}
	return(ptr);
}
pdelay(mlist,plist,proc)
int	*mlist,*plist,*proc;
{
if(--(plist[COUNT]) <= 0)
	plist[FLAGS] = BLANKED | UPALL;
return(NULL);
}
pchase(mlist,plist,proc)
int	*mlist,*plist,*proc;
{
	int	x,y,deltx,delty,*temp ;
 
	x = plist[X];
	y = plist[Y];
	deltx = 0;
	delty = 0;
	if((temp = locate(mlist,GUN)) == NULL) return(EOF);
	if((x - temp[X]) > 0) deltx = -1;
	if((x - temp[X]) < 0) deltx =1;
	if((y - temp[Y]) > 0) delty = -1;
	if((y - temp[Y]) < 0) delty = 1;
	plist[X] = x + deltx;
	plist[Y] = y + delty;
	if(overlap(plist,temp)) {
		temp[PROC] = NULL; /* kill future updates */
		replace(GUN,BOOM);
		plist[FLAGS] = BLANKED;
	}
	plist[FLAGS] = plist[FLAGS] | UPALL ;
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
	ORG	06800H
	#ENDASM
	#INCLUDE	fpass0.c
	#INCLUDE	fpass1.c
	#INCLUDE	fpass2.c
	#INCLUDE	LIB
	#INCLUDE	RUN
	#ASM
	LIST	*
	#ENDASM
� � � � � � � � � � � � � � � � � � � � � � 