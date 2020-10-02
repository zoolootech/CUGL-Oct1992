
#       Count the number of lines in one or more files

                  { flines[FILENAME]++ }


END               {
                      print ""
                      for (fname in flines)
                      {
			  ++files
			  total += flines[fname]
			  printf("File: %-16s Lines: %5d\n",
                                 fname, flines[fname])
                      }
		      printf("\n%d total lines in %d file%s\n",
			     total, files, files == 1 ? "" : "s")
                  }
