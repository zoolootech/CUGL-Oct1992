#include <ctype.h>
#include "timer1.h"

char c = 'x';

DO_IEXPR("isalnum(c)")	isalnum(c)	OD
DO_IEXPR("isalpha(c)")	isalpha(c)	OD
DO_IEXPR("isascii(c)")	isascii(c)	OD
DO_IEXPR("iscntrl(c)")	iscntrl(c)	OD
DO_IEXPR("isdigit(c)")	isdigit(c)	OD
DO_IEXPR("islower(c)")	islower(c)	OD
DO_IEXPR("isprint(c)")	isprint(c)	OD
DO_IEXPR("ispunct(c)")	ispunct(c)	OD
DO_IEXPR("isspace(c)")	isspace(c)	OD
DO_IEXPR("isupper(c)")	isupper(c)	OD
c = 'M';
DO_IEXPR("tolower(c)")	tolower(c)	OD
c = 'm';
DO_IEXPR("toupper(c)")	toupper(c)	OD
}
