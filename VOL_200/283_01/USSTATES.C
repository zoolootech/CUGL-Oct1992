/* usstates.c, 9/3/88, d.c.oshel */

#include "fafnir.h"
#include <stdio.h>

/* U.S. Postal Service 2-letter state and territory abbreviations */

static char *states[] = {
    "   Foreign",
	"AL Alabama",
    "AK Alaska",
    "AZ Arizona",
    "AR Arkansas",
    "CA California",
    "CO Colorado",
    "CT Connecticut",
    "DE Delaware",
    "DC Dist. of Columbia",
    "FL Florida",
    "GA Georgia",
    "GU Guam",
    "HI Hawaii",
    "ID Idaho",
    "IL Illinois",
    "IN Indiana",
    "IA Iowa",
    "KS Kansas",
    "KY Kentucky",
    "LA Louisiana",
    "ME Maine",
    "MD Maryland",
    "MA Massachusetts",
    "MI Michigan",
    "MN Minnesota",
    "MS Mississippi",
    "MO Missouri",
    "MT Montana",
    "NE Nebraska",  /* NOT "NB", according to U.S. Post Office! */
    "NV Nevada",
    "NH New Hampshire",
    "NJ New Jersey",
    "NM New Mexico",
    "NY New York",
    "NC North Carolina",
    "ND North Dakota",
    "OH Ohio",
    "OK Oklahoma",
    "OR Oregon",
    "PA Pennsylvania",
    "PR Puerto Rico",
    "RI Rhode Island",
    "SC South Carolina",
    "SD South Dakota",
    "TN Tennesee",
    "TX Texas",
    "UT Utah",
    "VT Vermont",
    "VI Virgin Islands",
    "VA Virginia",
    "WA Washington",
    "DC Washington, D.C.",
    "WV West Virginia",
    "WI Wisconsin",
    "WY Wyoming",
    NULL
};


/* field validater */
int vstate( char *p, int x, int y, int len )
{
    char **q;

    if (*p != '')  /* assistance flag */
    {
    for ( q = states; *q != NULL; q++ )
    {
        if ( memicmp( p, *q, 2 ) == 0 )
            return (1);
    }}
	memcpy( p, states[ select(x,y,states,SIZE(states)-1) ], len );
    return (1);	
}

