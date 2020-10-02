/*
	pass 2 writes out those objects in the page list flagged 
	for WRITE and clears all flags except the BLANKED flag.
*/
pass2(mscan,pgscan,pscrn)
int	*mscan,*pgscan,*pscrn;
{
int	pgflag;
	while(pgscan[FORWARD] !=NULL) {
		pgflag=pgscan[FLAGS];
		if(WRITE&pgflag) {
			writeobj(pscrn,NBYTES,pgscan[X],pgscan[Y],pgscan[OBJPTR]);
		}
		pgscan[FLAGS]=pgflag&BLANKED;
		pgscan=pgscan[FORWARD];
	}
}
FORWARD];
	}
}
continue;
				if(!(WRITE&pgscan1[FLAGS])) { /* no write */
					if(overlap(pg