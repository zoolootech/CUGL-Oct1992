
/*
 *  gptrs.inc -- include file for LIST.C
 */

static void *gptr0(void);
static void *gptr1(void);
static void *gptr2(void);
static void *gptr3(void);

static void *(*gptrs[MAXLISTS])(void) = { gptr0, gptr1, gptr2, gptr3 };

static void *gptr0(void)
{
    list = listptr[0];
    return lgetptr();
}

static void *gptr1(void)
{
    list = listptr[1];
    return lgetptr();
}

static void *gptr2(void)
{
    list = listptr[2];
    return lgetptr();
}

static void *gptr3(void)
{
    list = listptr[3];
    return lgetptr();
}
