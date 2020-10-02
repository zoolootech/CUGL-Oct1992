#include "panel.h"
#include "keys.h"

main()
    {

    pan_init();

    pan_activate("colors");

    while (pan_execute("", 1, 1) != ESC);

    pan_destroy();

    }

