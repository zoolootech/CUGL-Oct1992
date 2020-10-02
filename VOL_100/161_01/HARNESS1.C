/* harness1 - execute repetitive long division */
#define LOOPCNT 10000
main()
	{
	long a, b = 255, c = 255, i;

	for (i = 1; i <= LOOPCNT; ++i)
		a = b / c;
	}
