/*
	otoi(n)

	this function converts an offset octal number in ASCII
	to an integer. the number is in the format xxx.xxx{a} and
	may be preceeded by white space.
*/

otoi(n)
char *n;
{
	int val, b, i;
	char c;

	val = 0; b = 16384;

	while ((c = *n) == '\t' || c == ' ') ++n;
	for (i = 0; i < 7; i++) {
	   if ((c = *n) == '.') {++n; b = 64;}
	   else {c = *n++; val = val + (b * (c - '0')); b /= 8;}
	}
	return val;
}

