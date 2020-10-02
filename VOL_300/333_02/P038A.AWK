

$3 > maxpop	{ maxpop = $3; country = $1 }

END		{ print "\nCountry with the largest population:",     \
			country, maxpop
		}
