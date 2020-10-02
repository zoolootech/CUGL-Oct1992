#include <vbuf.h>


main()
{
    char vbuf[VBUFSIZ],str[256],buf[256];
    int b,c;
    char a;


    puts("\n\nVFILE TEST PROGRAMME\n\n\n");
    puts("Name of file  ");
    gets(str);
    puts("record length: ");
    scanf("%d",&b);
printf("number=%d\n",b);
    if(vopen(str,vbuf,b)==ERROR) 
    {	if (vcreat(str,vbuf,b)==ERROR)
	{puts("cannot open...\n"); exit();}
    }
	while(1)
	{
	  puts("\n#");
	  a=toupper(getchar());
	    if (a=='X')
		 {vclose(vbuf); puts("x"); exit();}

	    if (a=='W')
	    {
		puts("Write string: ");
		gets(str);
		fillb(buf,250,32,1);
		strcpy(buf,str);
		puts("to record #:  ");
		scanf("%d",&b);
printf("number=%d\n",b);
printf("returned %d\n",vwrite(b,vbuf,buf));
	    }
	    else
	    {	
		fillb(str,200,32,1);
		puts("Read record #: ");
		scanf("%d",&c);
printf("number=%d\n",c);
		printf("return %d\n",vread(c,vbuf,str));
		printf("Record %d:\"%s\"\n",c,str);
	    }
	}

    puts("How the hell d'we get here?");
}
