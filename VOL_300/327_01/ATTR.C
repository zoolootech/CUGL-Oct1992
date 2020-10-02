#include "panel.h"
#include "keys.h"

unsigned keys[] = {
     ENTER,
     ESC,
     0
     };

main()
    {
    char buf[20];

    pan_init();

    pan_resp_keys(keys);

    pan_activate("attr");

    while (pan_execute("", 1, 0) != ESC)
        {
        pan_get_field("attr", 1, buf);
        pan_field_attr("field", 1, buf);
        }

    pan_destroy();
    }


