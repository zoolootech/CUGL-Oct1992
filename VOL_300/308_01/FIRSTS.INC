
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
