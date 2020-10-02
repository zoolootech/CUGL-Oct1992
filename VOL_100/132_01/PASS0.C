pass0(mptr,pgptr,pscrn)
int *mptr,*pgptr,*pscrn;
{
int *mscan,*pgscan;
int	mflag,pgflag;
	mscan=mptr;
	pgscan =pgptr;
	while(mscan[FORWARD] != NULL) {
		if(pgscan[FORWARD] == NULL ) break;
/*
	Test for a new object definition in the master list.
	If not blanked, erase and flag for rewrite
	Copy the pointer to the page list.
*/
	if(mscan[OBJPTR] != pgscan[OBJPTR]) {
		new(mscan,pgscan,pscrn,pgptr);
		mscan = mscan[FORWARD];
		pgscan = pgscan[FORWARD];
		continue;
	}
/*
	Test for a move of mlist object relative to pglist
*/
	if(mscan[X] == pgscan[X] ) { 
		if(mscan[Y] == pgscan[Y]) {
/*
	Object has not moved to here
*/
			fixed(mscan,pgscan,pscrn,pgptr);
			mscan = mscan[FORWARD];
			pgscan = pgscan[FORWARD];
			continue;
		}
	}
/*
		object has moved to here
*/
		moved(mscan,pgscan,pscrn,pgptr);
		mscan = mscan[FORWARD];
		pgscan = pgscan[FORWARD];
	}
}
new(mptr,pgptr,screen,ptop)
int	*mptr,*pgptr,*screen,*ptop;
{
int	mflag,pgflag;
	pgflag=pgptr[FLAGS];
	mflag =mptr[FLAGS];
	if((mflag & BLANKED) == 0) { /* mlist object is on */
		if((pgflag & BLANKED) == 0) { /* pglist obj is on */
		  eraseobj(screen,NBYTES,pgptr[X],pgptr[Y],pgptr[OBJPTR]);
			pgflag = pgflag + ERASE;
			rewrite(pgptr,ptop);
		}
		pgflag = pgflag + WRITE ;
	}
	pgptr[FLAGS] = pgflag;
	pgptr[OBJPTR]=mptr[OBJPTR];
}
/*
	fixed	sets the erases and sets for write those objects
	which are both the same and have not moved relative to the
	master list.
*/
fixed(mscan,pgscan,pscrn,pgptr)
int	*mscan,*pgscan,*pscrn,*pgptr;
{
int	mflag,pgflag;
	mflag	= mscan[FLAGS];
	pgflag	= pgscan[FLAGS];
	if((mflag&BLANKED)==0){ /* mlist object is on */
		if(pgflag&BLANKED) {
			pgflag=(pgflag&(~BLANKED))+WRITE;
		}
	}
		/* mlist is blanked, test pglist */
	else 	if((pgflag&BLANKED)==0) {
			pgflag=pgflag+BLANKED+ERASE;
			eraseobj(pscrn,NBYTES,pgscan[X],pgscan[Y],pgscan[OBJPTR]);
			rewrite(pgscan,pgptr);
		}
	pgscan[FLAGS]=pgflag;
}
/*
	moved 	erases and flags for re-write those objects which
	have moved relative to the master list.
*/
moved(mscan,pgscan,pscrn,pgptr)
int	*mscan,*pgscan,*pscrn,*pgptr;
{
int	pgflag,mflag;
	pgflag	=pgscan[FLAGS];
	mflag	= mscan[FLAGS];
	if((mflag&BLANKED) ==0) { /* object is on */
		if(pgflag&BLANKED) { /* list object off */
			pgflag=(pgflag&(~BLANKED))+MOVER+WRITE;
		}
		else {	pgflag=pgflag+MOVER+WRITE+ERASE;
			eraseobj(pscrn,NBYTES,pgscan[X],pgscan[Y],pgscan[OBJPTR]);
			rewrite(pgscan,pgptr);
		     }
	}
		/* mlist object is off */
	else	if((pgflag&BLANKED)==0) { /* pglist is on */
			pgflag=pgflag+ERASE+BLANKED;
			eraseobj(pscrn,NBYTES,pgscan[X],pgscan[Y],pgscan[OBJPTR]);
			rewrite(pgscan,pgptr);
		}
	pgscan[X]=mscan[X];
	pgscan[Y]=mscan[Y];
	pgscan[FLAGS]=pgflag;
}
can,*pgs