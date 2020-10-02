#include <gds.h>
#include <stdarg.h>

void usererror1(), usererror2();

main()
{
    char line[80];

    GRADinit();
    setgraph();
    PRE_ERROR_LEVEL=4;
    PRE_ERROR_FUNC=usererror1;
    POST_ERROR_LEVEL=2;
    POST_ERROR_FUNC=usererror2;
    while (1) {
        CreateFrame(960,792);
        PlotType(1);
        HorzLine(10,280,XLIMIT,25);
        PlotType(0);
        ReadStr(line,80,10,300,2,0x5f,0x5f);
        LoadFont(line);
    }
}

void usererror1(errlevel, errnu, arg_ptr)
int errlevel, errnu;
va_list arg_ptr;
{
    extern char *GRADERRMSG[];

    printf("I am usererror 1:");
    vprintf(GRADERRMSG[errnu],arg_ptr);
    printf("\n");
}

void usererror2(errlevel, errnu, arg_ptr)
int errlevel, errnu;
va_list arg_ptr;
{
    printf("I am usererror2\n");
}


