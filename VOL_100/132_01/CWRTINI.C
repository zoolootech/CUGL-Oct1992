/*
	init() initializes the lists
*/
init()
{
int *mlist,*pg0list,*pg1list; /* pointers to master list and page list */
int	counter,*ahead,*objptr;
 
/*
	Initialize debug pointers
*/
	gscan = 24576;	/* 6000 hex */
	gscan1	= 24578;
	gxleft	= 24580;
	gxright	= 24582;
	gyupper	= 24584;
	gylower = 24586;
	gxm	= 24588;
	gym	= 24590;
	counter = 0;
	mlist = master;
	pg0list = lpage0;
	pg1list = lpage1;
	while(counter < NOBJ) {
		if(counter != (NOBJ - 1)) 
			ahead = mlist + MASLEN;
		else	ahead = NULL;
		if(counter != NULL)
			objptr = obj3;
		else	objptr = obj1;
		setmlist(mlist,ahead,counter,(counter*8),(counter*8),NULL,objptr);
		if(counter != (NOBJ - 1)) {
			copylist(mlist,pg0list,(pg0list+PGLEN));
			copylist(mlist,pg1list,(pg1list+PGLEN));
		}
		else {
			copylist(mlist,pg0list,NULL);
			copylist(mlist,pg1list,NULL);
		}
		mlist = mlist + MASLEN;
		pg1list = pg1list + PGLEN;
		pg0list = pg0list + PGLEN;
		counter++;
	}
	putclk();	/* enable screen clock sync */
}
/*
	setmlist	sets up the master display list where
	setmlist(listpointer,FORWARD,NUMBER,X,Y,FLAGS,OBJPTR) is
	 the expected argument list.
	Calls - No one
	Returns - nothing
*/
setmlist(plist,for,num,x,y,flags,pobj)
int	*plist,for,num,x,y,flags,*pobj;
{
	plist[FORWARD]=for;
	plist[NUMBER] = num;
	plist[X]      = x;
	plist[Y]      = y;
	plist[FLAGS]  = flags;
	plist[OBJPTR] = pobj;
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
        