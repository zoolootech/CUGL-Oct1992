/*	 file	demo1.c */
#include fprintf.h
#define EOF	-1
#define MAXL	40
extern	float	atof();
extern	char	*ftoa();
extern	int	portc,portd;
main() {
	static float fnuma;
	static	int len;
	static char line[MAXL];

	portd = 188;	portc = 189;	/* must be YOUR hardware ports	*/

	while(len = getline(line, MAXL) < MAXL ) {
		fnuma = atof(line);

		printf("%-20.5e\t%-20.5f\t%-20.5g\n",fnuma,fnuma,fnuma);
		printf("%-20.4e\t%-20.4f\t%-20.4g\n",fnuma,fnuma,fnuma);
		printf("%-20.3e\t%-20.3f\t%-20.3g\n",fnuma,fnuma,fnuma);
		printf("%-20.2e\t%-20.2f\t%-20.2g\n",fnuma,fnuma,fnuma);
		printf("%-20.1e\t%-20.1f\t%-20.1g\n",fnuma,fnuma,fnuma);
		printf("\n\n");
	}
}

getline(s,lim)	/* get line into s, return length */
char	s[];
int	lim;
{
	int	c,i;
	i = 0;
	while(--lim > 0 && (c=getchar()) != EOF && c != '\n')
		s[i++] = c;
	if (c == '\n')
		s[i++] = c;
	s[i] ='\0';
	return(i);
}
�������� �_�������	+�! 9~�
�!�~�bʆ!�5>�É6�>
*�	w���`i#�᯾�·�����%,�! ������|�!���