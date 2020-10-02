
/*
 *  gsizes.inc -- include file for LIST.C
 */

static size_t gsize0(void);
static size_t gsize1(void);
static size_t gsize2(void);
static size_t gsize3(void);

static size_t (*gsizes[MAXLISTS])(void) = { gsize0, gsize1, gsize2, gsize3 };

static size_t gsize0(void)
{
    list = listptr[0];
    return lgetsize();
}

static size_t gsize1(void)
{
    list = listptr[1];
    return lgetsize();
}

static size_t gsize2(void)
{
    list = listptr[2];
    return lgetsize();
}

static size_t gsize3(void)
{
    list = listptr[3];
    return lgetsize();
}
