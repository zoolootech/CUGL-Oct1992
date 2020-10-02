
#   echo - print command line arguments

BEGIN	    {
		for (i = 0; i < ARGC; ++i)
		    printf "%s ", ARGV[i]
		printf "\n"
		for (i in ARGV)
		    print "[" i "]", ARGV[i]
		exit
	    }
