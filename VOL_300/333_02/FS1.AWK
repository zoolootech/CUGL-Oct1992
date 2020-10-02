
#  This sample demonstrates the use of the enhanced FS variable to split
#  records on more complicated input files.


BEGIN	    {
		OFS = "\t"
	    }

	    # $1 is empty for each record
	    {
		printf "%-20.20s %10s %16s\n", $2, $3, $4
	    }
