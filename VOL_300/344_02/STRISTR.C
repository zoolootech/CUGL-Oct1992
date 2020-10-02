
/*
 *      HEADER:         ;
 *      TITLE:          stristr() - substring search function;
 *      DATE:           10/27/1989;
 *      DESCRIPTION:    "strstr() that ignores case distinctions";
 *      VERSION:        1.0;
 *      FILENAME:       STRISTR.C;
 *      SEE-ALSO:       PB.DOC;
 *      AUTHORS:        Michael Kelly;
 */



#include "stristr.h"



/*
 *  strstr() that ignores case distinctions
 */
char *stristr(const char *string, const char *substr)
{
    char *str1, *str2, *sentinel;
    size_t string_len, substr_len, n;
    int dif;


    if(! *substr)
	return (char *) string;

    string_len = strlen(string);
    substr_len = strlen(substr);

    if(substr_len > string_len)
	return NULL;

    sentinel = (char *) &string[string_len - substr_len];

    do  {
	str1 = (char *) string;
	str2 = (char *) substr;
	n = substr_len;

	while(n--)
	    if(dif = tolower(*str1++) - tolower(*str2++))
		break;

	if(! dif)
	    return (char *) string;

	} while(++string <= sentinel);

	return NULL;

}


