
#   prep1 - prepare countries by continent and pop. den.

BEGIN       { FS = "\t+"; tmpname = "tmp.$$$" }

	    {
		printf("%s:%s:%d:%d:%.1f\n",
		       $4, $1, $3, $2, 1000 * $3 / $2) > tmpname
	    }

END	    {
		close(tmpname)
		system(sprintf("sort < %s", tmpname))
		system(sprintf("del %s", tmpname))
	    }
