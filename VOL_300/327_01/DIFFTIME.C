#include <stdio.h>
#include "panel.h"
#include "keys.h"

unsigned resp_keys[] = {
    ESC,
    ENTER,
    0};

main()
    {
    char h1[5], m1[5], s1[5];
    char h2[5], m2[5], s2[5];
    float diff;
    long t1, t2;
    char d[25];

    pan_init();
    pan_resp_keys(resp_keys);

    pan_activate("difftime");

    while (pan_execute("", 1, 0) != ESC)
        {
        pan_get_field("h1", 1, h1);
        pan_get_field("m1", 1, m1);
        pan_get_field("s1", 1, s1);
        pan_get_field("h2", 1, h2);
        pan_get_field("m2", 1, m2);
        pan_get_field("s2", 1, s2);

        t1 = atoi(h1) * 3600 + atoi(m1) * 60 + atoi(s1);
        t2 = atoi(h2) * 3600 + atoi(m2) * 60 + atoi(s2);
        diff = (t2 - t1) / 60.0;

        sprintf(d, "%10.2lf", diff);
        pan_put_field("diff", 1, d);
        }

    pan_destroy();

    }

