
#   reverse - print input in reverse order (for statement)

	{ line[NR] = $0 }	    # remember each line

END	{
	    for (i = NR; i > 0; i = i - 1)
		print line[i]
	}
