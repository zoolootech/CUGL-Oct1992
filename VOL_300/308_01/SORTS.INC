
/*
 *  sorts.inc -- include file for LIST.C
 */

static int sort0();
static int sort1();
static int sort2();
static int sort3();

static int (*sorts[MAXLISTS])() = { sort0, sort1, sort2, sort3 };

static int sort0()
{
    list = listptr[0];
    return lqsort();
}

static int sort1()
{
    list = listptr[1];
    return lqsort();
}

static int sort2()
{
    list = listptr[2];
    return lqsort();
}

static int sort3()
{
    list = listptr[3];
    return lqsort();
}
