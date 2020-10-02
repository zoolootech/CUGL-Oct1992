/* counties.c, 9/3/88, d.c.oshel */

#include "fafnir.h"

/* Iowa Dept.of Transportion 2-digit county codes range from 1 to 99 */

static char *counties[] = {
    "Out of State",
	"Adair",       "Adams",       "Allamakee",     "Appanoose",  "Audubon",  
	"Benton",      "Black Hawk",  "Boone",         "Bremer",     "Buchanan", 
	"Buena Vista", "Butler",      "Calhoun",       "Carroll",    "Cass",     
	"Cedar",       "Cerro Gordo", "Cherokee",      "Chickasaw",  "Clarke",    
	"Clay",        "Clayton",     "Clinton",       "Crawford",   "Dallas",    
	"Davis",       "Decatur",     "Delaware",      "Des Moines", "Dickinson", 
	"Dubuque",     "Emmet",       "Fayette",       "Floyd",      "Franklin",  
	"Fremont",     "Greene",      "Grundy",        "Guthrie",    "Hamilton",  
	"Hancock",     "Hardin",      "Harrison",      "Henry",      "Howard",    
	"Humboldt",    "Ida",         "Iowa",          "Jackson",    "Jasper",    
	"Jefferson",   "Johnson",     "Jones",         "Keokuk",     "Kossuth",   
	"Lee",         "Linn",        "Louisa",        "Lucas",      "Lyon",      
	"Madison",     "Mahaska",     "Marion",        "Marshall",   "Mills",     
	"Mitchell",    "Monona",      "Monroe",        "Montgomery", "Muscatine", 
	"O'Brien",     "Osceola",     "Page",          "Palo Alto",  "Plymouth",  
	"Pocahontas",  "Polk",        "Pottawattamie", "Poweshiek",  "Ringgold",   
	"Sac",         "Scott",       "Shelby",        "Sioux",      "Story",      
	"Tama",        "Taylor",      "Union",         "Van Buren",  "Wapello",    
	"Warren",      "Washington",  "Wayne",         "Webster",    "Winnebago",  
	"Winneshiek",  "Woodbury",    "Worth",         "Wright"
};


int vcounty( char *p, int x, int y, int len )
{
	char buff[4];
	int code;

    if (*p != '')
    {
	    code = atoin( p, len );
	    if ( code > 0 && code < 100 )
		    return (1);  /* county codes 1 to 99 are valid */
    }
	code = select( x, y, counties, SIZE( counties ) );
	sprintf( buff, "%02d", code );
	memcpy( p, buff, len );
	return (1);
}

