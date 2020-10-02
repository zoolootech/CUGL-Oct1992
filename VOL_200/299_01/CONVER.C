/*
---------------------------------------------------------------------------
filename: conver.c
author: g. m. crews
creation date: 28-Jul-1988
date of last revision: 28-Jul-1989

this function calculates and returns the conversion factor required for
going from units represented by an input string of characters, to units
represented by an output string of characters.
   
to choose the proper strings for various units, see the conversion table
array of structures defined below.  this table gives each unit's key,
conversion factor, string, and base unit's key.  the conversion factor is
the number by which to multiply in order to convert to the given base unit.
   
notice that all compatible units have the same base key.  thus, to
convert from units given by an input_key to units given by an output
key, the function simply returns the input conversion factor divided by
the output conversion factor.
   
if the input_key or output_key are not found in the conversion_table,
or the keys do not have the same base, 0. is returned.

conversion factors were obtained from ASTM E380-82 Standard for Metric
Practice.

example, to go from feet to inches:

    conversion_factor = calc_conversion_factor("ft", "in");
---------------------------------------------------------------------------
*/
#include <string.h>

double calc_conversion_factor(char input_str[],char output_str[]);

#define NUM_CFACTORS_IN_TBL 160
#define MAX_LEN_UNITS 14

double calc_conversion_factor(
char input_str[],  /* string describing input units (see table below) */
char output_str[]) /* string describing output units */
{
    int i,j;
    double factor1, factor2;
    int base1, base2;

    static struct conversion_factors {
	int    key;	     /* code number for this units */
	double factor;	     /* multiplier to obtain base (reference) units */
	int    base;	     /* code number for base units */
	char   units[MAX_LEN_UNITS+1];
			     /* string describing units */
    } conversion_table[NUM_CFACTORS_IN_TBL] = {

    /* This is the array containing conversion factors.  Note that an
       askerisk (*) in the comment describing a unit means the conversion
       factor is exact. */

    /* LENGTH: */
    {  100, 1.000000E+00,  100, "m"		}, /* *(meter) */
    {  101, 1.000000E-10,  100, "angstrom"	},
    {  102, 1.495979E+11,  100, "ast-unit"	}, /* astronomical unit */
    {  103, 2.011684E+01,  100, "chain" 	}, /* US survey ft */
    {  104, 1.828804E+00,  100, "fathom"	}, /* US survey ft */
    {  105, 1.000000E-15,  100, "fermi" 	},
    {  106, 3.048000E-01,  100, "ft"		}, /* *(foot) */
    {  107, 3.048006E-01,  100, "survey-ft"	}, /* US survey ft */
    {  108, 2.540000E-02,  100, "in"		}, /* *(inch) */
    {  109, 9.460550E+15,  100, "light-year"	},
    {  110, 2.540000E-08,  100, "micro-in"	}, /* *microinch */
    {  111, 1.000000E-06,  100, "micron"	}, /* *micron */
    {  112, 2.540000E-05,  100, "mil"		}, /* *mil */
    {  113, 1.852000E+03,  100, "naut-mi"	}, /* nautical mile */
    {  114, 1.609347E+03,  100, "mi"		}, /* statute mile
						      (US survey foot) */
    {  115, 3.085678E+16,  100, "parsec"	},
    {  116, 4.217518E-03,  100, "pica"		}, /* printer's */
    {  117, 3.514598E-04,  100, "point" 	}, /* *(printer's) */
    {  118, 5.029210E+00,  100, "rod"		}, /* rod (US survey foot) */
    {  119, 9.144000E-01,  100, "yd"		}, /* *yard */
    /* ACCELERATION: */
    {  200, 1.000000E+00,  200, "m/s2"		}, /* meter/sec2 */
    {  201, 3.048000E-01,  200, "ft/s2" 	},
    {  202, 9.806650E+00,  200, "g"		}, /* *standard free-fall */
    {  203, 2.540000E-02,  200, "in/s2" 	}, /* *in/s2 */
    /* ANGLE: */
    {  300, 1.000000E+00,  300, "rad"		}, /* *rad */
    {  301, 1.745329E-02,  300, "deg"		}, /* deg */
    {  302, 2.908882E-04,  300, "min"		}, /* minute */
    {  303, 4.848137E-06,  300, "sec"		}, /* second */
    {  304, 1.570796E-02,  300, "grad"		}, /* grad */
    /* AREA: */
    {  400, 1.000000E+00,  400, "m2"		}, /* m2 */
    {  401, 4.046873E+03,  400, "acre"		}, /* acre (US survey foot) */
    {  402, 9.290304E-02,  400, "ft2"		}, /* *ft2 */
    {  403, 1.000000E+04,  400, "ht"		}, /* hectare */
    {  404, 6.451600E-04,  400, "in2"		}, /* *in2 */
    {  405, 2.589998E+06,  400, "mi2"		}, /* US statute */
    {  406, 8.361274E-01,  400, "yd2"		},
    /* TORQUE: */
    {  500, 1.000000E+00,  500, "N*m"		}, /* *(newton meter) */
    {  501, 1.000000E-07,  500, "dyne*cm"	}, /* *dyne*cm */
    {  502, 9.806650E+00,  500, "kgf*m" 	}, /* *kgf*m */
    {  503, 7.061552E-03,  500, "ozf*in"	},
    {  504, 1.129848E-01,  500, "lbf*in"	},
    {  505, 1.355818E+00,  500, "lbf*ft"	},
    /* DENSITY: */
    {  600, 1.000000E+00,  600, "kg/m3" 	}, /* *kg/m3 */
    {  601, 1.000000E+03,  600, "g/cm3" 	}, /* *g/cm3 */
    {  602, 7.489152E+00,  600, "ozm/gal"	}, /* US liquid */
    {  603, 1.729994E+03,  600, "ozm/in3"	},
    {  604, 1.601846E+01,  600, "lbm/ft3"	},
    {  605, 2.767990E+04,  600, "lbm/in3"	},
    {  606, 1.198264E+02,  600, "lbm/gal"	},
    {  607, 5.153788E+02,  600, "slug/ft3"	},
    /* ENERGY & WORK: */
    {  700, 1.000000E+00,  700, "J"		}, /* *(joule) */
    {  701, 1.055056E+03,  700, "Btu"		}, /* Btu (Inter. Table) */
    {  702, 4.184000E+00,  700, "cal"		}, /* *(thermochemical) */
    {  703, 1.602190E-19,  700, "ev"		}, /* electronvolt */
    {  704, 1.000000E-07,  700, "erg"		}, /* *erg */
    {  705, 1.355818E+00,  700, "ft*lbf"	},
    {  706, 4.214011E-02,  700, "ft*poundal"	},
    {  707, 3.600000E+06,  700, "kW*h"		},
    {  708, 1.054804E+08,  700, "therm" 	},
    {  709, 4.184000E+09,  700, "ton-TNT"	}, /* nuclear equiv. TNT */
    {  710, 3.600000E+03,  700, "W*h"		}, /* *W*h */
    /* POWER PER UNIT AREA: */
    {  800, 1.000000E+00,  800, "W/m2"		}, /* *W/m2 */
    {  801, 1.135653E+04,  800, "Btu/ft2/s"	},
    {  802, 3.154591E+00,  800, "Btu/ft2/h"	},
    {  803, 4.184000E+04,  800, "cal/cm2/s"	}, /* *(thermochemical) */
    {  804, 1.000000E+04,  800, "W/cm2" 	}, /* *W/cm2 */
    {  805, 1.550003E+03,  800, "W/in2" 	},
    /* FORCE: */
    {  900, 1.000000E+00,  900, "N"		}, /* *(newton) */
    {  901, 1.000000E-05,  900, "dyne"		}, /* *dyne */
    {  902, 9.806650E+00,  900, "kgf"		}, /* kilogram-force */
    {  903, 4.448222E+03,  900, "kip"		}, /* kip (1000 lbf) */
    {  904, 2.780139E-01,  900, "ozf"		}, /* ounce-force */
    {  905, 4.448222E+00,  900, "lbf"		}, /* lbf */
    {  906, 1.382550E-01,  900, "poundal"	},
    /* THERMAL CONDUCTIVITY: */
    { 1000, 1.000000E+00, 1000, "W/m/K" 	}, /* *W/m/K */
    { 1001, 1.730735E+00, 1000, "Btu/h/ft/F"	},
    { 1002, 1.442279E-01, 1000, "Btu*in/h/ft2/F"},
    { 1003, 5.192204E+02, 1000, "Btu*in/s/ft2/F"},
    /* HEAT CAPACITY: */
    { 1100, 1.000000E+00, 1100, "J/kg/K"	}, /* *J/kg/K */
    { 1101, 4.186800E+03, 1100, "Btu/lbm/F"	},
    /* MASS: */
    { 1200, 1.000000E+00, 1200, "kg"		}, /* *kg */
    { 1201, 6.479891E-05, 1200, "gr"		}, /* *grain */
    { 1202, 1.000000E-03, 1200, "gm"		}, /* gram */
    { 1203, 9.806650E+00, 1200, "kgf*s2/m"	},
    { 1204, 2.834952E-02, 1200, "ozm"		}, /* ounce-mass */
    { 1205, 4.535924E-01, 1200, "lbm"		}, /* pound-mass */
    { 1206, 1.459390E+01, 1200, "slug"		},
    /* MASS PER UNIT LENGTH: */
    { 1300, 1.000000E+00, 1300, "kg/m"		}, /* *kg/m */
    { 1301, 1.111111E-07, 1300, "denier"	},
    { 1302, 1.488164E+00, 1300, "lbm/ft"	},
    { 1303, 1.785797E+01, 1300, "lbm/in"	},
    { 1304, 1.000000E-06, 1300, "tex"		}, /* *tex */
    /* MASS PER UNIT TIME: */
    { 1400, 1.000000E+00, 1400, "kg/s"		}, /* *kg/s */
    { 1401, 1.259979E-04, 1400, "lbm/h" 	},
    { 1402, 7.559873E-03, 1400, "lbm/min"	},
    { 1403, 4.535924E-01, 1400, "lbm/s" 	},
    /* POWER: */
    { 1500, 1.000000E+00, 1500, "W"		}, /* *(watt) */
    { 1501, 2.930711E-01, 1500, "Btu/h" 	},
    { 1502, 1.055056E+03, 1500, "Btu/s" 	},
    { 1503, 4.184000E+00, 1500, "cal/s" 	},
    { 1504, 1.000000E+00, 1500, "erg/s" 	}, /* *erg/s */
    { 1505, 3.766161E-04, 1500, "ft*lbf/h"	},
    { 1506, 2.259697E-02, 1500, "ft*lbf/min"	},
    { 1507, 1.355818E+00, 1500, "ft*lbf/s"	},
    { 1508, 7.456999E+02, 1500, "hp"		}, /* *550 ft*lbf/s) */
    { 1509, 9.809500E+03, 1500, "boiler-hp"	}, /* boiler */
    { 1510, 7.460000E+02, 1500, "elec-hp"	}, /* *(electric) */
    { 1511, 3.517000E+03, 1500, "ton-refrig"	}, /* (12000 btu/hr) */
    /* PRESSURE OR STRESS */
    { 1600, 1.000000E+00, 1600, "Pa"		}, /* *(pascal) */
    { 1601, 1.013250E+05, 1600, "std-atm"	}, /* *standard atmosphere */
    { 1602, 1.000000E+05, 1600, "bar"		},
    { 1603, 2.988980E+03, 1600, "ftH2O" 	}, /* ft of water 39.2F */
    { 1604, 3.386380E+03, 1600, "inHg"		}, /* in of mercury 32F */
    { 1605, 2.490820E+02, 1600, "inH2O" 	}, /* in of water 39.2F */
    { 1606, 9.806650E+00, 1600, "kgf/m2"	}, /* *kgf/m2 */
    { 1607, 6.894757E+06, 1600, "kip/in2"	},
    { 1608, 1.000000E+02, 1600, "millibar"	}, /* *millibar */
    { 1609, 1.333220E+02, 1600, "torr"		}, /* mmHg 0C */
    { 1610, 1.488164E+00, 1600, "poundal/ft2"	},
    { 1611, 4.788026E+01, 1600, "lbf/ft2"	},
    { 1612, 6.894757E+03, 1600, "psi"		}, /* lbf per sq. in */
    /* VELOCITY */  
    { 1700, 1.000000E+00, 1700, "m/s"		}, /* *(meter/second) */
    { 1701, 8.466667E-05, 1700, "ft/h"		},
    { 1702, 5.080000E-03, 1700, "ft/min"	}, /* *ft/min */
    { 1703, 3.048000E-01, 1700, "ft/s"		}, /* *ft/s */
    { 1704, 2.540000E-02, 1700, "in/s"		}, /* *in/s */
    { 1705, 2.777778E-01, 1700, "km/h"		},
    { 1706, 5.044444E-01, 1700, "knot"		}, /* knot (international) */
    { 1707, 4.470400E-01, 1700, "mi/h"		}, /* mi/h (international) */
    /* DYNAMIC VISCOSITY: */
    { 1800, 1.000000E+00, 1800, "Pa*s"		}, /* *(pascal*second) */
    { 1801, 1.000000E-03, 1800, "cp"		}, /* *centipoise */
    { 1802, 1.488164E+00, 1800, "poundal*s/ft2" },
    { 1803, 4.133789E-04, 1800, "lbm/ft/h"	},
    { 1804, 1.488164E+00, 1800, "lbm/ft/s"	},
    { 1805, 4.788026E+01, 1800, "lbf*s/ft2"	},
    { 1806, 6.894757E+03, 1800, "lbf*s/in2"	},
    { 1807, 4.788026E+01, 1800, "slug/ft/s"	},
    /* KINEMATIC VISCOSITY: */
    { 1900, 1.000000E+00, 1900, "m2/s"		}, /* *(meter sq. per sec) */
    { 1901, 1.000000E-06, 1900, "cs"		}, /* *centistokes */
    { 1902, 9.290304E-02, 1900, "ft2/s" 	},
    /* VOLUME: */
    { 2000, 1.000000E+00, 2000, "m3"		}, /* *(meter cubed) */
    { 2001, 1.589873E-01, 2000, "bbl"		}, /* barrel (oil, 42 gal) */
    { 2002, 2.359737E-03, 2000, "board-ft"	}, /* board foot */
    { 2003, 3.523907E-02, 2000, "bushel"	}, /* US */
    { 2004, 2.365882E-04, 2000, "cup"		},
    { 2005, 2.957353E-05, 2000, "oz"		}, /* fluid ounce (US) */
    { 2006, 2.831685E-02, 2000, "ft3"		},
    { 2007, 3.785412E-03, 2000, "gal"		}, /* US */
    { 2008, 1.638706E-05, 2000, "in3"		},
    { 2009, 1.000000E-03, 2000, "l"		}, /* *liter */
    { 2010, 4.731765E-04, 2000, "pint"		}, /* US */
    { 2011, 9.463529E-04, 2000, "qt"		}, /* quart (US) */
    { 2012, 1.478676E-05, 2000, "tbl"		}, /* tablespoon */
    { 2013, 4.928922E-06, 2000, "tsp"		}, /* teaspoon */
    { 2014, 7.645546E-01, 2000, "yd3"		},
    /* VOLUMETRIC FLOW: */
    { 2100, 1.000000E+00, 2100, "m3/s"		}, /* *(cubic meter per sec) */
    { 2101, 4.719474E-04,21000, "cfm"		},
    { 2102, 2.831685E-02,21000, "cfs"		},
    { 2103, 2.731177E-07,21000, "in3/min"	},
    { 2104, 6.309020E-05,21000, "gpm"		}}; /* gal per min */

    /* Is input_str in the table? */
    for (i = 0; i < NUM_CFACTORS_IN_TBL; i++) {
    if (strcmp(conversion_table[i].units, input_str)) continue;

    /* yes, it is. */
    factor1 = conversion_table[i].factor;
    base1 = conversion_table[i].base;

    /* Is output_str in the table? */
    for (j = 0; j < NUM_CFACTORS_IN_TBL; j++) {
	if (strcmp(conversion_table[j].units, output_str)) continue;

        /* yes, it is. */
        factor2 = conversion_table[j].factor;
        base2 = conversion_table[j].base;

        /* Are the units compatible? */
	if (base1 != base2) return 0.;	/* no, they are incompatible. */
        return (factor1 / factor2);  /* yes, they are. Return conversion */
    }				     /* factor for going from input units */
				     /* to output units. */

    /* Gets here iff output_str not found. */
    return 0.;
    }

    /* Gets here if input_str not found or other error. */
    return 0.;
}
