
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
