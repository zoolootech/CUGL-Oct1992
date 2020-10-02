

# interest1 - compute compound interest
#   input:	amount rate years
#   output:	compounded value at the end of each year

	{
	    i = 1
	    while (i <= $3)
	    {
		printf("Year %d\t%.2f\n", i, $1 * (1 + $2) ^ i)
		i = i + 1
	    }
	}
