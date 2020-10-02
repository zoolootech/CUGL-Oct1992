
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

