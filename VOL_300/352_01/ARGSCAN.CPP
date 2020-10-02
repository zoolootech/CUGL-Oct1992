// ARGSCAN.CPP
//		unified approach to command-line parms.
//		ARGTBL: NULL-terminated list of strings to match and addresses of data
//								also max length of string (incl term. 0)
//								if len==1, data is treated like single char.
//								if len==0, data value is converted to int.
//		argscan () - interpret argc/argv according to argtbl
//				RETURNS:  number of unmatched args.
//				MODIFIES: argv points to list of unmatched args, terminated by 0
//					(array of ptrs to strings, with NULL terminator.)
//					return value of 0 means all args were matched.
//				NOTE that argv[0] was program name, becomes 1st unmatched arg.
//				
//		example entries: invoke program.exe  -A- -c23 name=GEORGE 
//					char name[10], switchA='+', int count=1;  ...defaults
//					ARGTBL args[]= {"name=",name, sizeof(name),
//									"-A",   switchA, sizeof(switchA),
//									"-C",   count, 0 }; 
//		NOTE: uses case-insenstive match.
//		
#include "dblib.h"

int argscan ( int argc, char **argv, ARGTBL *argtbl )
	{
	int remaining = 0;
	int len;
		
	for ( int n=1; n<argc; ++n )		// process args, skip prog name.
		{
		for ( ARGTBL *eptr=argtbl; (eptr->arg_id) != NULL; ++eptr )
			{
			ARGTBL entry = *eptr;			// working copy. 
			len = strlen ( entry.arg_id );
			if ( memicmp ( entry.arg_id, argv[n], len )==0 )
				{
				// match to table
				if (entry.arg_len == 0) 
					*((int*)entry.arg_ptr) = atoi ( argv[n] + len );
				else
				if (entry.arg_len == 1) 
					*( (char*) (entry.arg_ptr) ) = argv[n][len];
				else
					{
					memcpy ( entry.arg_ptr, argv[n]+len, entry.arg_len );
					*(  (char*) (entry.arg_ptr) + entry.arg_len -1)=0;
					}
				break;	// break from if...
				}		// end if ... match
			}
			
		if  (eptr->arg_id == NULL ) // NON-MATCH
			{
			argv[remaining] = argv[n];
			++remaining;
			}		
		} 	
	argv[remaining] = NULL;
	
	return ( remaining );		// argscan()
	}
//----------------- end of argscan ----------------------

#ifdef ARGSCAN
char S[10] ="DEFAULT", C ='D';
int  I = 0;

ARGTBL argtbl[] = 
	{
	{"S=", &S, sizeof (S) },
	{"I=", &I, 0 },
	{"C=", &C, 1 },
	{NULL, NULL, 0 }
	};


main ( int argc, char **argv )
	{
	argc =argscan ( argc, argv, argtbl );
	printf ( "S=%s, C=%c, I=%i. Remaining =%i\n", S, C, I, argc );
	while ( *argv )
		{
		puts ( *(argv++) );
		}
	return (0);	
	}
#endif		// ARGSCAN	
