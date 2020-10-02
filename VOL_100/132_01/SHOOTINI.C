/*
	init() initializes the lists
*/
init()
{
int *mlist,*pg0list,*pg1list; /* pointers to master list and page list */
int	counter,*ahead,*objptr,*proc,*prcexp;
int	x,y,flag,class;
 
	counter = 0;
	mlist = master;
	pg0list = lpage0;
	pg1list = lpage1;
	while(counter < NOBJ) {
		proc = NULL;
		class = NULL;
		prcexp = NULL;
		if(counter != (NOBJ - 1)) 
			ahead = mlist + MASLEN;
		else	ahead = NULL;
		if(counter == PLANE) {
			objptr = plane;
			x = PLNX0;
			y = PLNY0;
			flag = NULL;
			proc = pplane;
			class = TARGET;
		}
		else if(counter == GUN) {
			objptr = gun;
			x = CENTER;
			y = 182;
			flag = NULL;
			proc = pgun;
		     }
		else if(counter == BULLET) {
			objptr = bullet;
			x = 190;
			y= 190;
			flag = NULL;
			proc = pbullet ;
		    }
		else if(counter == BOOM) {
			x = 10;
			y = 10;
			flag = BLANKED;
			objptr = boom;
		     }
		else if(counter == OBJ1) {
			x = 10;
			y = 10;
			flag = NULL;
			objptr = obj1;
			proc = pchase;
			class = TARGET;
		}
		else if(counter == OBJ2) {
			x = 10;
			y = 180;
			flag = NULL;
			objptr = obj2;
			proc = pchase;
			class = TARGET;
		}
		else if(counter == (NOBJ -1) ) {
			x = 0;
			y = 0;
			flag = NULL;
			proc = pchase;
			objptr = obj1;
		    }
		flag = flag | UPALL ; /* force update of all pages */
		setmlist(mlist,ahead,counter,x,y,flag,objptr,proc,prcexp,class);
		if(counter != (NOBJ - 1)) {
			copylist(mlist,pg0list,(pg0list+PGLEN));
			copylist(mlist,pg1list,(pg1list+PGLEN));
		}
		else {
			copylist(mlist,pg0list,NULL);
			copylist(mlist,pg1list,NULL);
		}
		pg0list[FLAGS] = pg0list[FLAGS] | BLANKED ;
		pg1list[FLAGS] = pg1list[FLAGS] | BLANKED ;
		mlist = mlist + MASLEN;
		pg1list = pg1list + PGLEN;
		pg0list = pg0list + PGLEN;
		counter++;
	}
	setmem(PAGE0,PGSIZE,NULL);
	setmem(PAGE1,PGSIZE,NULL); /* clear both pages */
	putclk();	/* enable screen clock sync */
	spmod4();	/* set pmode 4 graphics */
}
/*
	setmlist	sets up the master display list where
	setmlist(listpointer,FORWARD,NUMBER,X,Y,FLAGS,OBJPTR,PROC,PROCEXP,CLASS) is
	 the expected argument list.
	Calls - No one
	Returns - nothing
*/
setmlist(plist,for,num,x,y,flags,pobj,proc,prcexp,class)
int	*plist,for,num,x,y,flags,*pobj,*proc,*prcexp,class;
{
	plist[FORWARD]=for;
	plist[NUMBER] = num;
	plist[X]      = x;
	plist[Y]      = y;
	plist[FLAGS]  = flags;
	plist[OBJPTR] = pobj;
	plist[XINCR]	= NULL;
	plist[YINCR]	= NULL;
	plist[PROC]	= proc;
	plist[PROCEXP]  = prcexp;
	plist[CLASS]	= class;
}
/*
	copylist	initializes a page list from a master list
	argument list : (Masterlist pointer,Pagelist pointer,FORWARD)
*/
copylist(mlist,pglist,for)
int	*mlist,*pglist,for;
{
	pglist[FORWARD]	= for;
	pglist[NUMBER]	= mlist[NUMBER];
	pglist[X]	= mlist[X];
	pglist[Y]	= mlist[Y];
	pglist[FLAGS]	= mlist[FLAGS];
	pglist[OBJPTR]	= mlist[OBJPTR];
}
setmem(start,size,value)
char	*start;
int	size;
char	value;
{
	while(size --) {
		*start++ = value;
	}
}
;
}
posx(objnum)
in