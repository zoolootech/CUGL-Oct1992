#include <ctype.h>
test_numeric(char *str)
{
	while(*str) {
		if(!isdigit(*str++)) return(1);
	}
	return(0);
}
