
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

