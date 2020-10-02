#include <stdlib.h>
#include "panel.h"
#include "keys.h"

unsigned resp_keys[] = {
    ESC,
    ENTER,
    0};

main()
    {
    char freq[10], dur[10];

    pan_init();
    pan_resp_keys(resp_keys);

    pan_activate("sound");

    while (pan_execute("", 1, 0) != ESC)
        {
        pan_get_field("freq", 1, freq);
        pan_get_field("dur", 1, dur);

        if (atoi(freq) != 0)
            pan_sound(atoi(freq),atoi(dur));
        else
            {
            pan_sound(50, 30);
            pan_sound(5, 50);
            }
        }

    pan_destroy();
    }


