/*  VERSION 0009  (DATE: 17/05/87)  (TIME: 14:17)  */
/* mstsdr.c -- call xdentals function 1 times and print result*/
#include <STDIO.H>
#include <MATH.H>
main()
{
	double y,arg;
	int i;
	y=0;
	printf("Enter argument: ");
	scanf("%lf\n",&arg);
	printf("Argument = %f\n",arg);
	y=sin(arg);
	printf("sin(%f)=%19.9e\n",arg,y);
	y=cos(arg);
	printf("cos(%f)=%19.9e\n",arg,y);
	y=tan(arg);
	printf("tan(%f)=%19.9e\n",arg,y);
	y=asin(arg);
	printf("asin(%f)=%19.9e\n",arg,y);
	y=acos(arg);
	printf("acos(%f)=%19.9e\n",arg,y);
	y=atan(arg);
	printf("atan(%f)=%19.9e\n",arg,y);
	y=sinh(arg);
	printf("sinh(%f)=%19.9e\n",arg,y);
	y=cosh(arg);
	printf("cosh(%f)=%19.9e\n",arg,y);
	y=tanh(arg);
	printf("tanh(%f)=%19.9e\n",arg,y);
	y=exp(arg);
	printf("exp(%f)=%19.9e\n",arg,y);
	y=log(arg);
	printf("log(%f)=%19.9e\n",arg,y);
	y=log10(arg);
	printf("log10(%f)=%19.9e\n",arg,y);
}
