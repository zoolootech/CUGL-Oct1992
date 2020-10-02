
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
