/*
	pass1 checks for overlapping objects and calls for rewrites
	where required.
*/
pass1(mptr,pgptr,pscrn)
int	*mptr,*pgptr,*pscrn;
{
int	*pgscan,mflag,pgflag;
 
	pgscan=pgptr;
 
	while(pgscan[FORWARD] != NULL) {
		pgflag=pgscan[FLAGS];
		if(pgflag&WRITE) { /* if a write */
			rewrite(pgscan,pgptr);
			  }
		pgscan=pgscan[FORWARD];
	}
}
/*
	Rewrite	 scans a page list for any object which is not
	BLANKED or already flagged for WRITE. If the two objects
	overlap, then the tested object in the page list is 
	flagged for a write.
 
	Entry	: pointer to current page list entry
		  pointer to head of page list to scan
	Exit	: Returns 1 if at least one overlap acted on.
		  Returns 0 if no overlaps.
	Calls	: Overlap
*/
rewrite(pscan,pscan1)
int	*pscan,*pscan1;
{
int	update;
update = NULL;
	while(pscan1[FORWARD] != NULL) {
		if(pscan1 != pscan)	{ /* avoid same object */
			if(((BLANKED+WRITE)&pscan1[FLAGS])==0) {
				if(overlap(pscan,pscan1)) {
				 pscan1[FLAGS]=pscan1[FLAGS] + WRITE;
				 update++;
				}
			}
		}
		pscan1=pscan1[FORWARD];
	}
return(update);
}
/*
	Overlap tests for two objects in the same space.
	Entry	: A pointer to each scan list entry.
	Exit	: Null if no overlap, 1 if overlap
	Calls	: None
		Linkage to object definition list via scan pointers.
*/
overlap(scan,scan1)
int	scan[],scan1[];
{
int	xleft,xm,yupper,ym,xright,ylower,temp;
int	*obj;
/*
	Ascertain the left-most (x) and upper (y) objects.
*/
	xleft= scan[X];
	xright	= scan1[X];	/* assume this is correct */
	obj	= scan[OBJPTR];
	if(xleft > xright) 	{ /* if assumption invalid,switch */
		temp = xright;
		xright = xleft;
		xleft  = temp;
		obj    = scan1[OBJPTR];
	}
	xm= 255&(PIXBYT*obj[XMAX]);
	yupper	= scan[Y];	/* same procedure for y axis */
	ylower  = scan1[Y];
	obj 	= scan[OBJPTR];
	if(yupper > scan1[Y]) {
		temp	= yupper;
		yupper	= ylower;
		ylower	= temp;
		obj	= scan1[OBJPTR];
	}
	ym	= 255&obj[YMAX];
	if((xleft + xm)>= xright)
		if((yupper + ym)>= ylower)
			return 1;
	return 0;
}
 
n 0;
}
 
rn 0;
}
 
;
	return 0;
}
 
;
	return 0;
}
 
;
	return 0;
}
 

	retu