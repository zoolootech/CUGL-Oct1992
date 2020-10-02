// STRDEMO.CXX  - demo file for C++ String class.
//
//	member functions are provided to do almost all common string manipulations
//		including: 	assignment, comparison, concatenation,
//					finding substrings, finding any of a series of letters
//					extracting substrings, removing from middle
//					tokenize string in components
//					trimming whitespace in several different ways
//							(ie: from end only, from whole string, 
//								 duplicate whitespace only, etc...)
//					translating upper/lower case or arbitrary translation.
//					find and replace operations.
// 
//	Case sensitivity can be turned on or off for any of the functions.
//	The sizes of the string buffers are taken care of automatically.
//
//
//	To compile and link, construct a project file 
//		that includes 	WTS.LIB  ( windows lib for wmalloc() function ).
//						STRPPS.LIB ( lib with a all string plus plus routines )
//						strdemo.cpp (this module) 
//
//	D Blum 8/90

#include <stdlib.h>
#include <iostream.h>

#include "wtwgpp.h"

#define ON 	1
#define OFF 0
			
#pragma argsused			// allows us to ignore argv without a warning 


int main ( int argc, char **argv)
	{
	winit ('T');		// for memory allocation routines.
	if ( argc > 1 )
		{
		cout<< "String demo run with caseSens flag turned ON\n";
		String::caseSens = ON;
		}
	else
		{
		cout<< 	"String demo run with caseSens flag turned OFF\n"
			<<	"To run with caseSens turned ON, supply any parameter\n"; 
		// OFF is default.	
		}
		
	// constructing and comparing strings.
	// NOTE differences between upper and lower case.	
	// NOTE sometimes referring to Strings and sometimes to char *
	String apples("apples");
	String Apples("Apples");
	String oranges("oranges");
	String Oranges("Oranges");

	if ( apples < Oranges ) 	cout<<"apples are less than Oranges. ";	
	else						cout<<"Oranges are less than apples. ";

	if ( Apples == "apples" ) 	cout<<"Apples and apples are the same\n";
	else						cout<<"Apples and apples are not the same\n";


	// CONCATENATE 3 Strings and assign results to another string.
	// NOTE mixing Strings with traditional 'strings' which are just char*
	//
	String 	MixedFruit = apples + " & " + oranges;
			MixedFruit += "\n";
	cout << " Concatenation: " << MixedFruit ;


	// INSERTION 
	String nowtime("Now time for all good men");
	String forall("is the ");
	nowtime.insert ( forall, 4 );	
	cout << "\nString insertion: " << nowtime << "\n";
	
	// SUBSTRING EXTRACTION.
	//		the source String nowtime is unchanged. a new String is created,
	//		the address of the new String is returned by String::substring()
	//		NOTE: the cout operator << can use either a String or a &String.
	//					so you can use ptrs to Strings or Strings themselves.
	//		NOTE: you must explicitly delete the String created by substring()
	//			  or it will persist till the end of the program.
	String *Sptr;
	Sptr = nowtime.substring( 7,5 );  		// creates a new String. 
	cout << "Substring: (\"the t\") "<<  Sptr <<"\n";
	delete Sptr;							// IMPORTANT! you must delete

	// FIND CHARACTERS or STRINGS.
	//		functions in the String class include find() findAny() findNot()
	// 		similar to ANSI C functions strchr(), strstr(), strpbrk(), etc...
	//		but these are easier to use, and all of them obey String::caseSens
	// 
	String  fs = "Now is the time for all good men";
	String  St = "TIME";

	int  st_pos = fs.find (St);		// could also use:    fs.find("TIME");
	if ( st_pos == -1 )
		{
		cout<< "uppercase TIME not found in text: " << fs << "\n";
		}
	else
		{
		cout << "The position of TIME in: Now is the time... is(11) " 
			 <<  st_pos << "\n";
		}


	// find a vowel using findAny, which returns position of the vowel. 
	//	and use the index operator [] to retrieve the vowel itself 
	st_pos = fs.findAny("AEIOU");
	cout  	<<"The first vowel in \"Now is the time...\" is ";
	if ( st_pos == -1 ) cout << "NO CAPITAL VOWELS FOUND";
	else				cout << fs[st_pos];
		

	// TOKENIZE
	// 		tokenize() allows you to break a string up into parts.
	//		each part is placed in a new String, the original is broken down.
	//		NOTE: 1) the separator chars are lost.
	//			  2) the search for separator chars obeys String::caseSens
	//			  3) the newly created Strings must be explicitly deleted.

	cout <<"\nTokenizing...NOTE + signs replace any tokens.\n";

	while ( fs != NULL )
		{
		Sptr = fs.tokenize ("AEIOU");		// tokenize() returns a &String
		cout<< Sptr <<"+";					//         (NOTE << passes &String
		delete Sptr;						//... which must be deleted	
		}
	cout << "\n";



	// TRIM() and SQUEEZE() remove unwanted chars from string
	//	most useful for removing whitespace.
	//  NOTE both squeeze and trim shorten the string.
	//
	fs = "Many small letters x, y and z at the end: xxxyzzzyyyx";
	cout << fs;
	cout << "\nXYZ removed from end of string: " << fs.trim("XYZ") << "\n";

	cout << "All XYZ removed wherever found: " << fs.squeeze("XYZ") << "\n";
		



	// noExtra() can simplify the distribution of whitespace in a String.
	//		noExtra(str) compresses all multiple occurence of chars from str
	//					 and replaces the first occurence with the first char.
	//
	char *whitespace = " \r\n\t";		//NOTE space is first char provided.
	fs = "String  \twith\nimproved \r     white\t\t\tspace";
	cout << "\nOriginal whitespace: " << fs;
	cout << "\nSimplify whitespace: " << fs.noExtra (whitespace) << "\n";


		
		
	// TRANSLATE() exchanges chars from 1 translate table with another
	//		There are functions toUpper() and toLower() as well.
	//					ie: String X="abcdef";  X.toUpper();
	// 		NOTE caseSens flag controls translation.
	//
	fs = "Old MacDonald Had A Farm";
	cout<< "Original: " << fs << "\n";
	cout<< "Encoded:  " 
		<< fs.translate ("abcdefghijklmnopqrstuvwxyz",
						 "12345678901234567890123456" ) << "\n";		
	
	
	
	
	// REPLACE() find all occurences of char *orig in String,
	//			replace each occurence with char *replace.
	//	NOTE	1) caseSens, also, 
	//			2) lengths of orig and replace don't need to be equal.
	//				(the String length is adjusted for you.)
	//
	fs = "A horse, a horse, my kingdom for a horse.";
	cout << fs.replace ( "a horse", "two giraffes" ) << "\n";
	// with apologies to W.S.	
		
	return 0;	// main()
	}
//------------------- end of STRDEMO.CPP --------------------------//