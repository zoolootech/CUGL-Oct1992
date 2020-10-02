#include <stdio.h>

void		main(argc, argv)
int		argc;
char		*argv[];
{
short		h_magic;
long		longfile;
short		h_namesize;
register short	i;
char		h_name[1000];
register char	*np;
FILE		*fp;

if((fp = fopen(argv[1], "r")) == NULL)
	{
	perror(argv[1]);
	exit(1);
	}
do
	{
	fscanf(fp, "%6o%*6o%*6o%*6o%*6o%*6o%*6o%*6o%*11lo%6o%11lo",
	      &h_magic, &h_namesize, &longfile);
	np = h_name;
	for(i = h_namesize ; i-- ; )
		*np++ = fgetc(fp);
	*np = '\0';
	printf("Name %s (%d), File size %ld\n", h_name, h_namesize, longfile);
	if(h_magic != 070707)
		{
		fprintf(stderr, "bad magic!\n");
		break;
		}
	/*fseek(fp, longfile, 1l);*/
	while(longfile--)
		fgetc(fp);
	}
while(strcmp(h_name, "TRAILER!!!"));
}
