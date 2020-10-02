#include "timer1.h"

char buf[BUFSIZ];
static char buf100[] = "1   2   3   4   5   6   7   8   9   10  \
11  12  13  14  15  16  17  18  19  20  21  22  23  24  25 \n";
int i;

buf[0] = '\0';
DO_STMT("strcat-0 chars")		strcat(buf, "")			OD

DO_STMT("strcat-100 chars")
	{
	buf[0] = '\0';
	strcat(buf, buf100);
	}
OD


buf[0] = 'x';
DO_STMT("strchr-0 chars")		strchr(buf, 'x')		OD

DO_STMT("strchr-100 chars")		strchr(buf100, '\n')	OD

DO_STMT("strcmp-0 chars")		strcmp("", "")			OD

DO_STMT("strcmp-100 chars")		strcmp(buf100, buf100)	OD

DO_STMT("strcpy-0 chars")		strcpy(buf, "")			OD

DO_STMT("strcpy-100 chars")		strcpy(buf, buf100)		OD

DO_STMT("strlen-0 chars")		strlen("")				OD

DO_STMT("strlen-100 chars")		strlen(buf100)			OD

buf[0] = '\0';
DO_STMT("strncat-0 chars")		strncat(buf, "", 1)		OD

DO_STMT("strncat-100 chars")
	{
	buf[0] = '\0';
	strncat(buf, buf100, 101);
	}
OD

DO_STMT("strncmp-0 chars")		strncmp("", "", 1)			OD

DO_STMT("strncmp-100 chars")	strncmp(buf100, buf100, 101)	OD

DO_STMT("strncpy-0 chars")		strncpy(buf, "", 1)			OD

DO_STMT("strncpy-100 chars")	strncpy(buf, buf100, 101)		OD

}
