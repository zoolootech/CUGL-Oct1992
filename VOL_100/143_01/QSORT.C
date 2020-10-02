  /* quick sort for BDS-C; similar to K & L sort p116 & exch p117
    here the two routines are rolled into one srting function.
    This function is required for OTHELLO.C written in BDS-C */

qsort( targ, n, length, comp)
char targ[];
int n, length;
int (*comp)();
{
    char temp;
    register int  i, j, e, ind1, ind2, gap;
    for( gap =  n/2; gap > 0; gap /= 2)
	for( i = gap; i < n; i++)
	    for( j = i-gap; j >= 0; j -=gap) {
		ind1 = length * j;
		ind2 = (j + gap) * length;
		if ((*comp) (&targ[ind1], &targ[ind2]) <= 0 )
		    break;
		/* exchange */
		for( e = 0; e < length; e++, ind1++, ind2++) {
		    temp = targ[ind1];
		    targ[ind1] = targ[ind2];
		    targ[ind2] = temp;
		}
	    }
}
