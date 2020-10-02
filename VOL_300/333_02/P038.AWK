

$4 == "Asia"	    { pop += $3; ++n }

END		    { print "Total population of the", n,	\
			    "Asian countries is", pop, "million."
		    }
