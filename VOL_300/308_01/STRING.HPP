
/************************************************************************
	Title:		C++ Dynamic String Class Header;
	Filename:	String.hpp;
	Date:		9/2/90;
	Version:	1.0;
	Requires:	string.cpp;
	
	Author:		Michael Kelly
	                254 Gold Street
	                Boston, MA 02127
			Copyright 1990;

	COPYRIGHT:	This code may not be commercially distributed
			without prior arrangement with the author.  It
			may be used by programmers, without royalty, for
			their personal programs and for "one of a kind"
			or "custom" applications, provided that said
			programmers assume all liability concerning
			same.
************************************************************************/


#if !defined(STRING_HPP)
#define STRING_HPP

#include <string.h>

class String  {
    int   len;
    int   siz;
    char  *str;

  public:			// constructors
    String(int str_siz);
    String(String &string);
    String(char *cstr);
				// destructor
    ~String(void);

  friend String& operator + (char *cstr, String &string);

    String& operator = 	(String &string);
    String& operator = 	(char *cstr);
    String& operator + 	(String &string);
    String& operator + 	(char *cstr);
    String& operator += (String &string);
    String& operator += (char *cstr);
    char& operator []    (int index)
	{ 
	    if(index < 0)
		return *str;
	    else if(index >= siz)
		return *(str + (siz - 1));
	    else
		return *(str + index); 
	}
    const char* operator *     (void)
	{ return (const char *)str; }

  friend int operator <  (char *cstr, String &string);
  friend int operator <= (char *cstr, String &string);
  friend int operator >  (char *cstr, String &string);
  friend int operator >= (char *cstr, String &string);
  friend int operator == (char *cstr, String &string);
  friend int operator != (char *cstr, String &string);

    int operator < (String &string)
	{ return(strcmp(str, string.str) < 0); }
    int operator < (char *cstr)
	{ return(strcmp(str, cstr) < 0); }
    int operator <= (String &string)
	{ return(strcmp(str, string.str) < 1); }
    int operator <= (char *cstr)
	{ return(strcmp(str, cstr) < 1); }
    int operator > (String &string)
	{ return(strcmp(str, string.str) > 0); }
    int operator > (char *cstr)
	{ return(strcmp(str, cstr) > 0); }
    int operator >= (String &string)
	{ return(strcmp(str, string.str) > -1); }
    int operator >= (char *cstr)
	{ return(strcmp(str, cstr) > -1); }
    int operator == (String &string)
	{ return !(strcmp(str, string.str)); }
    int operator == (char *cstr)
	{ return !(strcmp(str, cstr)); }
    int operator != (String &string)
	{ return(strcmp(str, string.str) != 0); }
    int operator != (char *cstr)
	{ return(strcmp(str, cstr) != 0); }
    int length(void)
	{ return len; }
    int length(int new_len)
	{ return (new_len < siz) ? len = new_len : len; }
    int size(void)
	{ return siz; }
    int shrink(int new_size);

};

#endif
