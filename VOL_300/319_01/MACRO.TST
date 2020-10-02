/*
	Tests of SPP's macro handling abilities.

	source: macro.tst
	version: February 17, 1989
*/

/*
	PART I

	Weird and wonderful tests of macro expansion.
	see page 93 of Draft C Standard of January, 1988.
*/
#define x 3
#define f(a) f(x * (a))
#undef x

#define x 2
#define g f
#define z z[0]
#define h g(~
#define m(a) a(w)
#define w 0,1
#define t(a) a

macro_tst()
{

/* subtest 1
f(y+1); should expand to:
f(2 * (y+1));
*/
f(y+1);

/* subtest 2
f(f(z)); should expand to:
f(2 * (f(2 * (z[0]))));
*/
f(f(z));

/* subtest 3
t(t(g)(0) + t)(1); should expand to:
f(2 * (0)) + t(1);
*/
t(t(g)(0) + t)(1);

/* subtest 4
g(x+(3,4)-w) | h 5) & m (f)^m(m);
should result in:
f(2 * (2+(3,4)-0,1)) | f(2 * (~ 5)) & f(2 * (0,1))^m(0,1);
*/
g(x+(3,4)-w) | h 5) & m (f)^m(m);

}


/*
	PART 2
	Tests of string concatenation and token pasting.
*/
#define str(s) # s
#define xstr(s) str(s)
#define debug(s,t) printf("x" # s "= %d, x" # t "= %s", x ## s, x ## t)

stringize()
{

/*
#define str(s) # s
#define xstr(s) str(s)
#define debug(s,t) printf("x" # s "= %d, x" # t "= %s", x ## s, x ## t)
debug(1, 2);

should result in:
printf("x1= %d, x2= %s", x1, x2);
*/
debug(1, 2);

/*
fputs(str(strncmp("abc\0d", "abc", '\4') == 0) str(: @\n), s);
should result in:
fputs("strncmp(\"abc\\0d\", \"abc\", '\\4') == 0: @\n", s);
*/
fputs(str(strncmp("abc\0d", "abc", '\4') == 0) str(: @\n), s);

}

/*
	PART 3
	Tests of detecting duplicate definitions

	These examples do not follow the standard yet.
	Please do not report these as bugs to me.
*/
#define OBJ_LIKE (1-1)
#define OBJ_LIKE /* a */  (1-1) /* b */
#define FTN_LIKE(a) ( a )
#define FTN_LIKE( a ) ( /* a */ \
	a  /* b
	*/ )


#define OBJ_LIKE (0)
#define OBJ_LIKE (1 - 1)
#define FTN_LIKE(b) ( a )
#define FTN_LIKE(b) (b)

/*	PART 4
	Bug regression test:

	Test for white space in argument list in macros.

	Test for more than six arguments.

	Test for recursive definition of keywords
*/

#define char (signed) char
char c;

#pragma this can be anything

#pragma ##who cares##??

/* Test of defined keyword. */
#if defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__)
#define TINY_OR_SMALL_OR_MEDIUM
#endif

/*
	Test line number handling.
	Note: #error will terminate the processing of this test file.
*/
#define FILE_NAME "d:\sherlock\sl.h"
#define LINE2 1000
#define FILE2 "changed_file"
#include FILE_NAME
#line LINE2 FILE2

#define a( b , c , d , e , f , g , h ) b c d e f g h

line_number_test()
{
	int b;

	a(b, = , 1, + , 2, - , 3);

	SL_DISABLE();
}
