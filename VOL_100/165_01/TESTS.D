###atopi.in
     source expect  description

	     ''    -1   empty_string,_never_enters_loop
	     0      0   normal_case,_low_end
	 32767  32767   normal_case,_high_end
	 32768     -1   out-of-bounds,_low_end
	 99999     -1   fails_INT_MAX/10_test
###st_main.in
x
?
-
+
a
+
b
-
-
0
+
c
0
-
###t_main.in
x
-
>
1
a
=
>
3
c
=
>
3
d
=
>
2
b
=
-
=
-
=
0
=
>
3
c
0
-
###run_cars.in
n
n
###gen_menu.tst
/* add_menu - ADD A PART RECORD */
#include "local.h"
#include "menu.h"
#include "part.h"
extern bool isn_part();
static STRING_VAL Spart_no =
	{'a', (part1).part_no, sizeof((part1).part_no), NULL};
static STRING_VAL Slead_time =
	{'n', (part1).lead_time, sizeof((part1).lead_time), NULL};
static char *Tunit_meas[]=
	{
	"each", "lb", "box",
	};
static CHOICE_VAL Cunit_meas =
	{DIM(Tunit_meas), Tunit_meas, (part1).unit_meas};
static STRING_VAL Sunit_cost =
	{'a', (part1).unit_cost, sizeof((part1).unit_cost), NULL};
static STRING_VAL Scost_qty =
	{'n', (part1).cost_qty, sizeof((part1).cost_qty), NULL};
static FIELD Fadd_menu[] =
	{
	{2, 0, "Part number", 'S', &Spart_no, NULL, NULL, isn_part},
	{4, 0, "Lead time (in weeks)", 's', &Slead_time, NULL, NULL, 0},
	{6, 0, "Unit of measure", 'c', NULL, &Cunit_meas, NULL, 0},
	{8, 0, "Cost per unit", 's', &Sunit_cost, NULL, NULL, 0},
	{10, 0, "Qty required for price", 's', &Scost_qty, NULL, NULL, 0},
	};
MENU add_menu = {"ADD A PART RECORD", Fadd_menu, DIM(Fadd_menu), 0};
###st_main.tst
Type + to push, - to pop, = to print, 0 to reset:
?: x
unknown command: x
Type + to push, - to pop, = to print, 0 to reset:
?: ?
Type + to push, - to pop, = to print, 0 to reset:
?: -
EMPTY STACK
?: +
name: a
?: +
name: b
?: -
name= b
?: -
name= a
?: 0
?: +
name: c
?: 0
?: -
EMPTY STACK
?: 
###t_main.tst
Type > to insert, - to pop, = to print, 0 to reset:
?: x
unknown command: x
Type > to insert, - to pop, = to print, 0 to reset:
?: -
EMPTY QUEUE
?: >
start: 1
desc: a
?: =
start=    1 desc=a
?: >
start: 3
desc: c
?: =
start=    1 desc=a
start=    3 desc=c
?: >
start: 3
desc: d
?: =
start=    1 desc=a
start=    3 desc=c
start=    3 desc=d
?: >
start: 2
desc: b
?: =
start=    1 desc=a
start=    2 desc=b
start=    3 desc=c
start=    3 desc=d
?: -
start=    1 desc=a
?: =
start=    2 desc=b
start=    3 desc=c
start=    3 desc=d
?: -
start=    2 desc=b
?: =
start=    3 desc=c
start=    3 desc=d
?: 0
?: =
EMPTY QUEUE
?: >
start: 3
desc: c
?: 0
?: -
EMPTY QUEUE
?: 
###tr_main.tst
d
q=<d>
parent=<NULL>
after deletion
after re-inserting
d
q=<d>

.b
d
q=<b>
parent=<d>
after deletion
d
after re-inserting
.b
d
q=<b>

.b
..c
d
q=<c>
parent=<b>
after deletion
.b
d
after re-inserting
.b
..c
d
q=<c>

..a
.b
..c
d
q=<a>
parent=<b>
after deletion
.b
..c
d
after re-inserting
..a
.b
..c
d
q=<a>

..a
.b
..c
d
.e
q=<e>
parent=<d>
after deletion
..a
.b
..c
d
after re-inserting
..a
.b
..c
d
.e
q=<e>

after dup:
..a
.b
..c
d
.e
LFIND(missing) is properly NULL
LPFIND(missing) is properly NULL
LPFIND(root) is properly NULL
LNEXT(d) == <e>
LNEXT(a) == <b>
LNEXT(e) == <NULL>
LFIRST(e) == <e>

After deleting 'd':
..a
.b
..c
e

After deleting 'b':
..a
.c
e

After deleting 'c':
.a
e

After deleting 'a':
e

After deleting 'e':
###EOF
