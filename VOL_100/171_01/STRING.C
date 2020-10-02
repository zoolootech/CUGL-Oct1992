/************************************************************
 *        IBM PC "C" Bulletin Board                         *
 *        Tulsa, OK    918-664-8737                         *
 *        Lynn Long, SYSOP   300/1200 XMODEM                *
 ************************************************************/




char *strcpy(dest, src)
register char *dest, *src;
{
	char *sav;

	sav = dest;
	while (*dest++ = *src++)
		;
	return sav;
}

char *strncpy(dest, src, len)
register char *dest, *src;
{
	char *sav;

	sav = dest;
	while (len--) {
		if ((*dest++ = *src++) == 0) {
			while (len--)
				*dest++ = 0;
			break;
		}
	}
	return sav;
}

char *strcat(dest, src)
register char *dest, *src;
{
	char *sav;

	sav = dest;
	while (*dest)
		++dest;
	while (*dest++ = *src++)
		;
	return sav;
}

char *strncat(dest, src, len)
register char *dest, *src;
{
	char *sav;

	sav = dest;
	while (*dest)
		++dest;
	do {
		if (len-- == 0) {
			*dest = 0;
			break;
		}
	} while (*dest++ = *src++);
	return sav;
}

strcmp(a, b)
register char *a, *b;
{
	int i;

	while ((i = *a - *b++) == 0 && *a++)
		;
	return i;
}

strncmp(a, b, len)
register char *a, *b;
{
	int i = 0;

	while (len-- && (i = *a - *b++) == 0 && *a++)
		;
	return i;
}

strlen(str)
register char *str;
{
	register int len = 0;

	while (*str++)
		++len;
	return len;
}

char *index(str, c)
register char *str; register int c;
{
	while (*str) {
		if (*str == c)
			return str;
		++str;
	}
	return (char *)0;
}

char *rindex(str, c)
register char *str; int c;
{
	register char *cp;

	for (cp = str ; *cp++ ; )
		;
	while (cp > str)
		if (*--cp == c)
			return cp;
	return (char *)0;
}
