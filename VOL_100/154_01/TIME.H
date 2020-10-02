/* <time.h> -

		functions to return time and date..

*/

struct
{
	unsigned _ax,
			 _bx,
			 _cx,
			 _dx,
			 _si,
			 _di,
			 _ds,
			 _es,
			 _flags;
} r;

time(hr,min,sec)
unsigned *hr, *min, *sec;
{
	r._ax = 0x2C00;
	intcall(&r,&r,0x21);
	*min = r._cx & 0x00ff;
	*hr = r._cx >> 8;
	*sec = r._dx >> 8;
}

date(yr,mon,day,weekday)
unsigned *yr, *mon, *day;
char *weekday;
{
	static char *days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	int d;

	r._ax = 0x2a00;
	intcall(&r,&r,0x21);
	*yr = r._cx;
	*day = r._dx & 0x00ff;
	*mon = r._dx >> 8;
	d = r._ax & 0x00ff;
	strcpy(weekday,days[d]);
}
