// STRPPREP.CPP  contains String::replace()
//
//	routine to replace all occurences of char *original with char *replace.
//	lengths of two strings do not have to be equal.
//	detection of char *ori obeys String::caseSens
//
//  METHOD: This is a table driven routine.
//			The table is a list of all positions in *this where ori occurs.
//			The table is allocated dynamically, 
//				large enough to hold the maximum number of occurences = this->n
//			The table is filled in by repeated calls to String::findstr()
//				which looks for successive occurences of ori.
//			Computing size following replacements
//			A new string is allocated
//			The first bytes of *this are moved in up to the first occurence.
//			LOOP for each occurence:
//				move in the replacement string.
//				move in bytes from the *this string after current occurence
//													and before next occurence.
//			swap strings. free workareas.		
//	D Blum 8/90
#include <stdlib.h>
#include <string.h>
#include "wtwg.h"
#include "dblib.h"

static char NULLstring =0;

String& String::replace (char *ori, char *rep)
	{
	int sn = n;
	char *ss = s;
	if ( ori == NULL ) ori = &NULLstring;
	int lo = strlen (ori);
	if ( rep==NULL ) rep = &NULLstring;
	int lr = strlen (rep);
	int lresult;				// number of bytes in result string.
	int k=0;					// count occurences of ori in *this.
	char *stmp;					// the new string to build up
	char *stmp_p, *ss_p;		// position ptr in string stmp and ss;
	int this_pos,  next_pos, interval;					
	
	// make sure that original string and *this string are not NULL
	//		(see end of routine)
	//
	if ( lo == 0 || sn == 0 ) return *this;


	// position array - one int for each occurence of ori in *this.
	// 		maximum possible number of occurences is #bytes in *this.
	int *pos;
	pos = (int*) wmalloc ( sizeof(int)*sn, "String::replace" );
	// fill in array with all occurences.
	this_pos =-1;
	do 	{
		// NOTE somewhat complicated indexing
		//		guarantees stepping past last occurence detected.
		// 		this_pos starts at -1 so that this_pos+1 starts at 0.
		//
		interval = findstr(ss+this_pos+1, sn-this_pos-1, ori);

		if ( interval >= 0 )
			{
			this_pos += 1+interval;		// compensate for the +1 in findstr()
			pos[k++] = this_pos;
			}
		else	break;
		}
	while ( k< sn );
	
	if ( k== 0 ) return *this;		// none found.
		
	//  allocate a new string large enough to hold result of replacements.
	//	
	lresult = sn + k*(lr-lo);					// old len + #bytes change.
	stmp = (char*)wmalloc ( lresult+1, "String::replace" );

	// copy all of *this that precedes first occurrence of *ori.
	//	NOTE use next_pos here
	//		so that loop that follows can move next_pos into this_pos
	next_pos = pos[0];
	if ( next_pos > 0 )
		{
		memcpy ( stmp, ss, next_pos );	
		}
	stmp_p = stmp + next_pos;		// move ptrs in result string 
	ss_p   = ss   + next_pos;		// and source string by amount just moved.
	int i=0;
	do 	{
		this_pos = next_pos;
		
		// move in rep string, incr two ptrs at different rates.
		memcpy ( stmp_p, rep, lr );
		ss_p += lo;			// source ptr moves by size of original (bypassed) 
		stmp_p += lr;		// result ptr moves by size of replacement.
		
		++i;
			
		if ( i<k )		// there's another occurence coming up 
			{
			next_pos = pos[i];
			
			// test for intervening bytes in source between the two occurences 
			interval = next_pos - this_pos - lo;
			if ( interval > 0 )
				{
				memcpy ( stmp_p, ss_p,  interval);
				ss_p += interval; 	
				stmp_p += interval;
				}
			}
		else
			// all occurences have been moved.
			{
			// move any possible remainder of the source string
			//		that follows the last occurence.
			interval = sn -this_pos -lo;
			if  ( interval > 0 )
				{
				memcpy ( stmp_p, ss_p, interval );
				// dont need to move ptrs - at end of loop
				}
			break;
			}
		}
	while ( 1 );		// NOTE break statement above.
	
	stmp [ lresult ] = 0;		
	
	free (s);			// NOTE we already know s is not NULL.
	free ( pos );
	
	s = stmp;			// setup 'this' to point to result.
	n = lresult;
	
	return *this;		
	}		// end of String::replace() 
