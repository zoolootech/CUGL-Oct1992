
/*
 *  prevs.inc -- include file for LIST.C
 */

static int prevs0(void);
static int prevs1(void);
static int prevs2(void);
static int prevs3(void);

static int (*prevs[MAXLISTS])(void) = { prevs0, prevs1, prevs2, prevs3 };

static int prevs0(void)
{
    list = listptr[0];
    return lprev();
}

static int prevs1(void)
{
    list = listptr[1];
    return lprev();
}

static int prevs2(void)
{
    list = listptr[2];
    return lprev();
}

static int prevs3(void)
{
    list = listptr[3];
    return lprev();
}
