/* unird - universal read distribution
 */
#include <stdio.h>
main()
	{
	char b[BUFSIZ];
	FILE *fp;

	do {
		gets(b);
		} while (strncmp(b, "###", 3) != 0);
	while (strncmp(b, "###EOF", 6) != 0)
		{
		fp = fopen(&b[3], "w");
		while (gets(b) != NULL && strncmp(b, "###", 3) != 0)
			fprintf(fp, "%s\n", b);
		fclose(fp);
		}
	}
