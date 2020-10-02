struct	_sum	*s;
{
	int	ct, i, siz;

	ct = siz = 0;
	for ( i = 1; i< MAXMSGS; i++)
	{
		if ( (ct = nreadsum(fd,ct,s)) == ERROR)
		{
			mndx[i] = mndx[i-1] + siz;
			mno[i] = 0;
			return (i-1);
		}
		if (i)
			mndx[i] = mndx[i-1] + siz;
		else
			mndx[0] = 0;
		siz = (s->lct+3)/2;
		if (s->mstat)
			mno[i] = s->msgno;
		else
			mno[i] = 0;
	}
}
/***************************************************************/
nreadsum(fd,recno,s)
int	fd,recno;
struct	_sum	*s;
{
	int	blks;

	if ( readrec(fd,recno,tmpstr,1) == ERROR)
		return(ERROR);

	/* SSCANF CONSTANT INFORMATION */
	sscanf(tmpstr,"%d ",&s->msgno);		/* MSG:	*/
	sscanf(tmpstr+LNS,"%d ",&s->lct);	/* LNS:	*/
	sscanf(tmpstr+STAT,"%d ",&s->mstat);	/* STAT:*/
	sscanf(tmpstr+DT,"%s",s->date);		/* DT:	*/
	sscanf(tmpstr+FM,"%s",s->fnm);		/* FM:	*/
	sscanf(tmpstr+TO,"%s",s->tnm);		/* TO:	*/
	sscanf(tmpstr+SU,"%s",s->subj);		/* SU:	*/
	blks = (s->lct+3)/2;
	return(recno+blks);
}
/****************************************************************/
nt	ct, i, siz;

	ct = siz = 0;
	for ( i = 1; i< MAXMSGS; i++)
	{
		if ( (ct = nreadsum(fd,ct,s)) == E		/* MSG:	*/
	sscanf(tmpstr+LNS,"%d ",&s->lct);	/* LNS:	*/
	sscanf(tmpstr+STAT,"%d ",&s->mstat);	/* STAT:*/
	sscanf(tmpstr+DT,"%s",s->date);		/* DT:	*/
	sscanf(tmpstr+FM,"%s",s->fnm);		/* FM:	*/
	sscanf(tmpstr+TO,"%s",s->tnm);		/* TO:	*/
	sscanf(tmps