

#   form2 - format countries by continent, pop. den.

BEGIN	    {
		FS = ":"
		printf("%-15s %-10s %10s %7s %12s\n",
		       "CONTINENT", "COUNTRY", "POPULATION",
		       "AREA", "POP. DEN.")
	    }

	    {
		if ($1 != prev)
		{
		    if ("" != prev)
		    {
			printf("%-25s  %7s %10s %10s\n",
			       "", "-------", "-------", "--------")
			printf("Totals for %-14s  %7d %10d %10.1f\n",
			       prev, subtotpop, subtotarea, subtotden)
			printf("%-15s %-10s %7s %10s %10s\n",
			       "", "", "=======", "=======", "========")
		    }
		    print ""
		    prev = $1
		    subtotpop = subtotarea = subtotden = 0
		}
		else
		    $1 = ""
		totpop	   += $3
		totarea    += $4
		totden	   += $5
		subtotpop  += $3
		subtotarea += $4
		subtotden  += $5
		printf("%-15s %-10s %7d %10d %10.1f\n",
		       $1, $2, $3, $4, $5)
	    }

END	    {
		printf("%-25s  %7s %10s %10s\n",
		       "", "-------", "-------", "--------")
		printf("Totals for %-14s  %7d %10d %10.1f\n",
		       prev, subtotpop, subtotarea, subtotden)
		printf("%-15s %-10s %7s %10s %10s\n\n",
		       "", "", "=======", "=======", "========")
		printf("%-25s  %7d %10d %10.1f\n",
		       "Grand Totals", totpop, totarea, totden)
		printf("%-15s %-10s %7s %10s %10s\n",
		       "", "", "=======", "=======", "========")
	    }
