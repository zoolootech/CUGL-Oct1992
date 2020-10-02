
/************************************************************************
	Title:		C++ Dynamic String Class;
	Filename:	String.cpp;
	Date:		9/2/90;
	Version:	1.0;
	Requires:	string.hpp;
	
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


#include "string.hpp"

/*
 *  "scratch" String used by some of the String::operator methods.
 *
 *   	( benign neglect recommended )
 */
static String tempstr(2);


//  		----------  String constructors  ----------

String::String(int str_siz)
{
    if(! str_siz)
	str_siz = 2;
    str = new char[str_siz];
    str[0] = '\0';
    siz = str_siz;
    len = 0;
}

String::String(String &string)
{
    siz = string.siz;
    len = string.len;
    str = new char[siz];
    strncpy(str, string.str, len);
    str[len] = '\0';
}

String::String(char *cstr)
{
    len = strlen(cstr);
    siz = len + 1;
    str = new char[siz];
    strcpy(str, cstr);
}


//		----------  String destructor  ----------

String:: ~String(void)
{
    if(str)
	delete str;
    str = NULL;
    siz = len = 0;
}


//		----------  friend String operators  ----------


String& operator + (char *cstr, String &string)
{
    tempstr = cstr;
    tempstr += string;
    return tempstr;
}

int operator <  (char *cstr, String &string)
{
    return(strcmp(cstr, string.str) < 0);
}

int operator <= (char *cstr, String &string)
{
    return(strcmp(cstr, string.str) < 1);
}

int operator >  (char *cstr, String &string)
{
    return(strcmp(cstr, string.str) > 0);
}

int operator >= (char *cstr, String &string)
{
    return(strcmp(cstr, string.str) > -1);
}

int operator == (char *cstr, String &string)
{
    return !(strcmp(cstr, string.str));
}

int operator != (char *cstr, String &string)
{
    return(strcmp(cstr, string.str) != 0);
}


//		----------  String operators  ----------


String& String::operator = (String &string)
{
    char *temp;

    if(siz > string.len)  {
	len = string.len;
	strcpy(str, string.str);
    }
    else if(temp = new char[string.len + 1])  {
	if(str)
	    delete str;
	str = temp;
	strcpy(str, string.str);
	len = string.len;
	siz = len + 1;
    }
    else
	strncpy(str, string.str, len);

    if(&tempstr == &string)  {
	if(tempstr.str)
	    delete tempstr.str;
	tempstr.str = new char[2];
	tempstr.str[0] = '\0';
	tempstr.siz = 2;
	tempstr.len = 0;
    }
    return *this;
}

String& String::operator = (char *cstr)
{
    char *temp;
    int cstr_len = strlen(cstr);

    if(siz > cstr_len)  {
	len = cstr_len;
	strcpy(str, cstr);
    }
    else if(temp = new char[cstr_len + 1])  {
	if(str)
	    delete str;
	str = temp;
	strcpy(str, cstr);
	len = cstr_len;
	siz = len + 1;
    }
    else
	strncpy(str, cstr, len);


    return *this;
}


String& String::operator + (String &string)
{
    String temp(len + string.len + 1);

    temp.len = temp.siz - 1;
    strcpy(temp.str, str);
    strcat(temp.str, string.str);
    tempstr = temp;
    return tempstr;
}

String& String::operator + (char *cstr)
{
    String tmp(strlen(cstr) + 1);

    tmp = cstr;
    return (*this) + tmp;
}


String& String::operator += (String &string)
{
    char *temp;

    if(siz > (len + string.len))  {
	strcat(str, string.str);
	len += string.len;
    }
    else if(temp = new char[len + string.len + 1])  {
	strcpy(temp, str);
	strcat(temp, string.str);
	siz = len + string.len + 1;
	len = siz - 1;
	if(str)
	    delete str;
	str = temp;
    }
    else if(siz > (len + 1))  {
	strncat(str, string.str, siz - (len + 1));
	len = siz - 1;
	str[len] = '\0';
    }

    return *this;
}

String& String::operator += (char *cstr)
{
    String tmp(strlen(cstr) + 1);

    tmp = cstr;
    return (*this) += tmp;
}


//		----------  String member functions  ----------

int String::shrink(int new_size)
{
    if(siz <= new_size)
	return 0;

    if(new_size < 1)
	new_size = 1;
    if(str)
	delete str;
    str = new char[new_size];
    if(! str)
	return len = siz = 0;
    len = 0;
    str[0] = '\0';
    siz = new_size;
    return 1;
}
