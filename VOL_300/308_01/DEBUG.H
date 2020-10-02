
/*
	HEADER:         CUG308;
	TITLE:          header for debug module;

	DATE:           5/6/90;
	VERSION:        2.01;
	FILENAME:       DEBUG.H;
	SEE-ALSO:       DEBUG.C;
	AUTHOR:         Michael Kelly
			254 Gold St. Boston Ma. 02127;
	COPYRIGHT:	May be used freely if authorship is acknowledged;
*/


#if !defined(DEBUG_H)
#define DEBUG_H

void err_out(const char *errmsg);
void bug_out(const char *errmsg, const char *filename, int lineno);

#if defined(DEBUG)
#define err_exit(a) bug_out((a),__FILE__,__LINE__)
#else
#define err_exit(a) err_out(a)
#endif
#endif
