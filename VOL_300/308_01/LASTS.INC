
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
