
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
