
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
