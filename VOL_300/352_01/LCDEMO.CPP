// testcl.cpp
// 		this C++ routine tests and demonstrates the features of class LinkClass
//
#include <stdlib.h>
#include <iostream.h>

#include "wtwgpp.h"

class odd_num
	{
	InheritLinkClass(odd_num)
	public: 
		int t;
		odd_num (int i=0): node(base, this) {t=i;};
		~odd_num() { node.unLink(); };
		friend ostream& operator<<(ostream& ost, odd_num &t_obj) 
				{ return ost<<t_obj.t; };
	};
	
class even_num
	{
	InheritLinkClass(even_num)
	public: 
		int t;
		even_num (int i=0): node(base,this) { t=i; };
		~even_num() 		{ node.unLink(); };
		friend ostream& operator<<(ostream& ost, even_num &t_obj) 
				{ return ost<<t_obj.t; };
	};
	
main (int argc, char **argv)
	{
	for ( int i =0; i<argc; ++i )
			{
			cout <<  argv[i] << ' ';
			}

	odd_num  x1(1), x3(3), x5(5),  *xp1, *d7;

	cout << "\nClass odd_num  isEmpty=="<< odd_num::isEmpty() 
		 << "\nClass even_num isEmpty=="<< even_num::isEmpty()
		 << '\n';

	even_num  x2(2), x4(4), x6(6),  *xp2, *d8;


	d7 = new odd_num(7);
	d8 = new even_num(8);
	


	odd_num  x9(9);
	even_num  x10(10); 

	cout << '\n';

	for ( xp1= x1.first(); xp1; xp1=xp1->next() )
		{	
		cout<<  "  xp1 = " << *xp1 ;
		}	
	cout << '\n';
	
	for ( xp2= x2.first(); xp2; xp2=xp2->next() )
		{	
		cout<<  "\nxp2 = " << *xp2 ;
		}

	//  cout << "\n Now removing elements 7 and 8 from lists";
	puts (  "\n Now remove 7 and 8, swap 1,3 and 2,4" );
	x3.moveBelow(x1);
	x2.moveAbove(x4);

	delete d7;
	delete d8;
	
	cout << '\n';

	for ( xp1= x1.first(); xp1; xp1=xp1->next() )
		{	
		cout<<"   xp1 = " << *xp1 ;
		}	
		
	cout << '\n';
	
	for ( xp2= x2.first(); xp2; xp2=xp2->next() )
		{	
		cout<<  "   xp2 = " << *xp2 ;
		}	
	cout<< "\nThats all folks\n";

	return 0;	// end main()
	}	
	
	