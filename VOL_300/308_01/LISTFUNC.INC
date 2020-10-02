
/*
 *  adds.inc -- include file for LIST.C
 */

static int add0(void *item, size_t itemsize, enum Place place);
static int add1(void *item, size_t itemsize, enum Place place);
static int add2(void *item, size_t itemsize, enum Place place);
static int add3(void *item, size_t itemsize, enum Place place);

static int (*adds[MAXLISTS])(void *item, size_t itemsize, enum Place place) = {
add0, add1, add2, add3 };

static int add0(void *item, size_t itemsize, enum Place place)
{
    list = listptr[0];
    return ladd(item, itemsize, place);
}

static int add1(void *item, size_t itemsize, enum Place place)
{
    list = listptr[1];
    return ladd(item, itemsize, place);
}

static int add2(void *item, size_t itemsize, enum Place place)
{
    list = listptr[2];
    return ladd(item, itemsize, place);
}

static int add3(void *item, size_t itemsize, enum Place place)
{
    list = listptr[3];
    return ladd(item, itemsize, place);
}


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

/*
 *  cmps.inc -- include file for LIST.C
 */

static int cmp0(void *item1);
static int cmp1(void *item1);
static int cmp2(void *item1);
static int cmp3(void *item1);

static int (*cmps[MAXLISTS])() = { cmp0, cmp1, cmp2, cmp3 };

static int cmp0(void *item1)
{
    list = listptr[0];
    return lcmpitem(item1);
}

static int cmp1(void *item1)
{
    list = listptr[1];
    return lcmpitem(item1);
}

static int cmp2(void *item1)
{
    list = listptr[2];
    return lcmpitem(item1);
}

static int cmp3(void *item1)
{
    list = listptr[3];
    return lcmpitem(item1);
}


/*
 *  compares.inc -- include file for LIST.C
 */

static int comp0(void *item1, void *item2);
static int comp1(void *item1, void *item2);
static int comp2(void *item1, void *item2);
static int comp3(void *item1, void *item2);

static int (*compares[MAXLISTS])(void *item1, void *item2) = {
comp0, comp1, comp2, comp3 };

static int comp0(void *item1, void *item2)
{
    list = listptr[0];
    return compare[0](item1, item2);
}

static int comp1(void *item1, void *item2)
{
    list = listptr[1];
    return compare[1](item1, item2);
}

static int comp2(void *item1, void *item2)
{
    list = listptr[2];
    return compare[2](item1, item2);
}

static int comp3(void *item1, void *item2)
{
    list = listptr[3];
    return compare[3](item1, item2);
}

/*
 *  dels.inc -- include file for LIST.C
 */

static int del0();
static int del1();
static int del2();
static int del3();

static int (*dels[MAXLISTS])() = { del0, del1, del2, del3 };

static int del0()
{
    list = listptr[0];
    return ldelete();
}

static int del1()
{
    list = listptr[1];
    return ldelete();
}

static int del2()
{
    list = listptr[2];
    return ldelete();
}

static int del3()
{
    list = listptr[3];
    return ldelete();
}

/*
 *  destroys.inc -- include file for LIST.C
 */

static int dest0(void);
static int dest1(void);
static int dest2(void);
static int dest3(void);

static int (*destroys[MAXLISTS])(void) = { dest0, dest1, dest2, dest3 };

static int dest0(void)
{
    list = listptr[0];
    return ldestroy();
}

static int dest1(void)
{
    list = listptr[1];
    return ldestroy();
}

static int dest2(void)
{
    list = listptr[2];
    return ldestroy();
}

static int dest3(void)
{
    list = listptr[3];
    return ldestroy();
}


/*
 *  finds.inc -- include file for LIST.C
 */

static int find0(void *item1);
static int find1(void *item1);
static int find2(void *item1);
static int find3(void *item1);

static int (*finds[MAXLISTS])() = { find0, find1, find2, find3 };

static int find0(void *item1)
{
    list = listptr[0];
    return lfinditem(item1);
}

static int find1(void *item1)
{
    list = listptr[1];
    return lfinditem(item1);
}

static int find2(void *item1)
{
    list = listptr[2];
    return lfinditem(item1);
}

static int find3(void *item1)
{
    list = listptr[3];
    return lfinditem(item1);
}

/*
 *  firsts.inc -- include file for LIST.C
 */

static int firsts0(void);
static int firsts1(void);
static int firsts2(void);
static int firsts3(void);

static int (*firsts[MAXLISTS])(void) = { firsts0, firsts1, firsts2, firsts3 };

static int firsts0(void)
{
    list = listptr[0];
    return lfirst();
}

static int firsts1(void)
{
    list = listptr[1];
    return lfirst();
}

static int firsts2(void)
{
    list = listptr[2];
    return lfirst();
}

static int firsts3(void)
{
    list = listptr[3];
    return lfirst();
}

/*
 *  gitems.inc  -- include file for LIST.C
 */

static int gitem0(void *itembuf);
static int gitem1(void *itembuf);
static int gitem2(void *itembuf);
static int gitem3(void *itembuf);

static int (*gitems[MAXLISTS])(void *itembuf) = {
gitem0, gitem1, gitem2, gitem3 };

static int gitem0(void *itembuf)
{
    list = listptr[0];
    return lgetitem(itembuf);
}

static int gitem1(void *itembuf)
{
    list = listptr[1];
    return lgetitem(itembuf);
}

static int gitem2(void *itembuf)
{
    list = listptr[2];
    return lgetitem(itembuf);
}

static int gitem3(void *itembuf)
{
    list = listptr[3];
    return lgetitem(itembuf);
}

/*
 *  gptrs.inc -- include file for LIST.C
 */

static void *gptr0(void);
static void *gptr1(void);
static void *gptr2(void);
static void *gptr3(void);

static void *(*gptrs[MAXLISTS])(void) = { gptr0, gptr1, gptr2, gptr3 };

static void *gptr0(void)
{
    list = listptr[0];
    return lgetptr();
}

static void *gptr1(void)
{
    list = listptr[1];
    return lgetptr();
}

static void *gptr2(void)
{
    list = listptr[2];
    return lgetptr();
}

static void *gptr3(void)
{
    list = listptr[3];
    return lgetptr();
}

/*
 *  gsizes.inc -- include file for LIST.C
 */

static size_t gsize0(void);
static size_t gsize1(void);
static size_t gsize2(void);
static size_t gsize3(void);

static size_t (*gsizes[MAXLISTS])(void) = { gsize0, gsize1, gsize2, gsize3 };

static size_t gsize0(void)
{
    list = listptr[0];
    return lgetsize();
}

static size_t gsize1(void)
{
    list = listptr[1];
    return lgetsize();
}

static size_t gsize2(void)
{
    list = listptr[2];
    return lgetsize();
}

static size_t gsize3(void)
{
    list = listptr[3];
    return lgetsize();
}

/*
 *  lasts.inc -- include file for LIST.C
 */

static int lasts0(void);
static int lasts1(void);
static int lasts2(void);
static int lasts3(void);

static int (*lasts[MAXLISTS])(void) = { lasts0, lasts1, lasts2, lasts3 };

static int lasts0(void)
{
    list = listptr[0];
    return llast();
}

static int lasts1(void)
{
    list = listptr[1];
    return llast();
}

static int lasts2(void)
{
    list = listptr[2];
    return llast();
}

static int lasts3(void)
{
    list = listptr[3];
    return llast();
}

/*
 *  nexts.inc -- include file for LIST.C
 */

static int nexts0(void);
static int nexts1(void);
static int nexts2(void);
static int nexts3(void);

static int (*nexts[MAXLISTS])(void) = { nexts0, nexts1, nexts2, nexts3 };

static int nexts0(void)
{
    list = listptr[0];
    return lnext();
}

static int nexts1(void)
{
    list = listptr[1];
    return lnext();
}

static int nexts2(void)
{
    list = listptr[2];
    return lnext();
}

static int nexts3(void)
{
    list = listptr[3];
    return lnext();
}

/*
 *  prevs.inc -- include file for LIST.C
 */

static int prevs0(void);
static int prevs1(void);
static int prevs2(void);
static int prevs3(void);

static int (*prevs[MAXLISTS])(void) = { prevs0, prevs1, prevs2, prevs3 };

static int prevs0(void)
{
    list = listptr[0];
    return lprev();
}

static int prevs1(void)
{
    list = listptr[1];
    return lprev();
}

static int prevs2(void)
{
    list = listptr[2];
    return lprev();
}

static int prevs3(void)
{
    list = listptr[3];
    return lprev();
}

/*
 *  rems.inc -- include file for LIST.C
 */

static int rem0(void *itembuf);
static int rem1(void *itembuf);
static int rem2(void *itembuf);
static int rem3(void *itembuf);

static int (*rems[MAXLISTS])(void *itembuf) = { rem0, rem1, rem2, rem3 };

static int rem0(void *itembuf)
{
    list = listptr[0];
    return lremitem(itembuf);
}

static int rem1(void *itembuf)
{
    list = listptr[1];
    return lremitem(itembuf);
}

static int rem2(void *itembuf)
{
    list = listptr[2];
    return lremitem(itembuf);
}

static int rem3(void *itembuf)
{
    list = listptr[3];
    return lremitem(itembuf);
}

/*
 *  replace.inc -- include file for LIST.C
 */

static int replace0(void *newitem, int newsize);
static int replace1(void *newitem, int newsize);
static int replace2(void *newitem, int newsize);
static int replace3(void *newitem, int newsize);

static int (*replace[MAXLISTS])() = { replace0, replace1, replace2, replace3 };

static int replace0(void *newitem, int newsize)
{
    list = listptr[0];
    return lreplitem(newitem, newsize);
}

static int replace1(void *newitem, int newsize)
{
    list = listptr[1];
    return lreplitem(newitem, newsize);
}

static int replace2(void *newitem, int newsize)
{
    list = listptr[2];
    return lreplitem(newitem, newsize);
}

static int replace3(void *newitem, int newsize)
{
    list = listptr[3];
    return lreplitem(newitem, newsize);
}

/*
 *  sorts.inc -- include file for LIST.C
 */

static int sort0();
static int sort1();
static int sort2();
static int sort3();

static int (*sorts[MAXLISTS])() = { sort0, sort1, sort2, sort3 };

static int sort0()
{
    list = listptr[0];
    return lqsort();
}

static int sort1()
{
    list = listptr[1];
    return lqsort();
}

static int sort2()
{
    list = listptr[2];
    return lqsort();
}

static int sort3()
{
    list = listptr[3];
    return lqsort();
}
