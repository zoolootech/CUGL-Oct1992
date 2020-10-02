
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
