#include "panel.h"

main(argc, argv)
    int argc;
    char *argv[];
    {
    unsigned aid;

    if (argc < 2) {
        printf("Usage:  SHOWPAN <panel name>\a\n");
        exit(1);
        }

    pan_init();

    pan_activate(argv[1]);

    aid = pan_execute("",1,0);

    pan_destroy();

    exit(aid);

    }
