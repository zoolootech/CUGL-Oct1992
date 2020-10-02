/*
	this function converts ascii characters to an integer.
	most common number bases may be used (except offset
	octal).
*/

ntoi(n,b)
char *n;
int b;
{
	int val,sign;
	char c;

	val=0; sign=1;
	while ((c = *n) == '\t' || c == ' ') ++n;
	if (c == '-') {sign = -1; n++;}
	while (dig(c = *n++)) {
	   if (b == 16 && c >= 'A' && c <= 'F') c -= 7;
	   val = val * b + c - '0';
	}
	return sign*val;
}
