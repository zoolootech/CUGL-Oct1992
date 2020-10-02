
/*
 *  adds.inc -- include file for LIST.C
 */

static int add0(void *item, size_t itemsize, enum Place place);
static int add1(void *item, size_t itemsize, enum Place place);
static int add2(void *item, size_t itemsize, enum Place place);
static int add3(void *item, size_t itemsize, enum Place place);

static int (*adds[MAXLISTS])(void *item, size_t itemsize, enum Place place) = {
add0, add1, add2, add3 };

static int add0(void *item, size_t itemsize, enum Place place)
{
    list = listptr[0];
    return ladd(item, itemsize, place);
}

static int add1(void *item, size_t itemsize, enum Place place)
{
    list = listptr[1];
    return ladd(item, itemsize, place);
}

static int add2(void *item, size_t itemsize, enum Place place)
{
    list = listptr[2];
    return ladd(item, itemsize, place);
}

static int add3(void *item, size_t itemsize, enum Place place)
{
    list = listptr[3];
    return ladd(item, itemsize, place);
}

