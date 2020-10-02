// FULLFLNM.CPP
// make_fullfilename ()
//		routine to catenate directory, filename, and extension
//		and make sure that there are the right number of \, ., etc...
//		(does not cover all possible invalid constructions,
//			but allows things like:  C:\  +  \rootfile.ex1  + .ex2)
//	 
#include "dblib.h"
String *make_fullfilename ( char *dir, char *fn, char *ext )
	{
	String *Sdne = new String;
	
	*Sdne = dir;
	if ( Sdne->len() > 0 ) 
		{
		(*Sdne).trim ( "\\" );
		(*Sdne) += "\\";			// ensures exactly one terminal \
		}

	String Sfn = fn;	
	if ( ext != NULL )
		{
		int ndot = Sfn.find ( "." );
		if ( ndot >= 0 ) 		// if dot is present...
			{
			Sfn.cut ( ndot );		// remove && force to extension given
			}
		if ( ext[0] != '.' ) Sfn += ".";
		Sfn += ext;
		}	
		
	*Sdne += Sfn; 	
	
	return Sdne;		// directory_name_extension ()
	}
	
#if 0
// code for testing above routine.	
main (int argc, char **argv)
	{
	char *d=NULL, *n=NULL, *e=NULL;
	
	if ( argc > 3 ) e=argv[3];
	if ( argc > 2 ) n=argv[2];
	if ( argc > 1 ) d=argv[1];
	
	String *S = make_fullfilename ( d,n,e );
	puts ( *S );
	delete S;
	return 0;	
	}
#endif /* 0 - end of test code */