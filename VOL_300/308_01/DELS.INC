
/*
 *  dels.inc -- include file for LIST.C
 */

static int del0();
static int del1();
static int del2();
static int del3();

static int (*dels[MAXLISTS])() = { del0, del1, del2, del3 };

static int del0()
{
    list = listptr[0];
    return ldelete();
}

static int del1()
{
    list = listptr[1];
    return ldelete();
}

static int del2()
{
    list = listptr[2];
    return ldelete();
}

static int del3()
{
    list = listptr[3];
    return ldelete();
}
