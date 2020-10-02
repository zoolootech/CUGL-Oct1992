
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
