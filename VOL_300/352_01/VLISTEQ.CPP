//VLISTEQ.CPP
//	routine for Vlist::operator=() and copy constructor.
#include "dblib.h"

Vlist& Vlist::operator=( Vlist& a )
	{
	this->clear();				// remove current contents of Vlist. 
	this->push ( a );			// 
	return *this;
	}
	
Vlist::Vlist (Vlist& a)	// copy constructor
	{
	this-> clear();
	this-> push(a);	
	}
	
void Vlist::push ( Vlist& a )
	{
	int an = a.n;
	void **al=a.list;
	for ( int i=0; i<an; ++i )
		{
		this-> push ( al[i] );
		} 
	}	
	
//--------------------- end VLISTEQ.CPP -------------------------------------