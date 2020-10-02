
/*
 *  chgcomps.inc -- include file for LIST.C
 */

static int chgcomp0(int (*newcompare)());
static int chgcomp1(int (*newcompare)());
static int chgcomp2(int (*newcompare)());
static int chgcomp3(int (*newcompare)());

static int (*chgcomps[MAXLISTS])(int (*newcompare)()) = {
chgcomp0, chgcomp1, chgcomp2, chgcomp3 };

static int chgcomp0(int (*newcompare)())
{
    if(! newcompare)  {
	lerror = NULL_PTR;
	return 0;
    }
    compare[0] = newcompare;
    return 1;
}

static int chgcomp1(int (*newcompare)())
{
    if(! newcompare)  {
	lerror = NULL_PTR;
	return 0;
    }
    compare[1] = newcompare;
    return 1;
}

static int chgcomp2(int (*newcompare)())
{
    if(! newcompare)  {
	lerror = NULL_PTR;
	return 0;
    }
    compare[2] = newcompare;
    return 1;
}

static int chgcomp3(int (*newcompare)())
{
    if(! newcompare)  {
	lerror = NULL_PTR;
	return 0;
    }
    compare[3] = newcompare;
    return 1;
}
