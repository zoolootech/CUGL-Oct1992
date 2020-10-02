/* SUMGRADR.C   VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:39:39 PM */
/*
cp b:$1 -e -s0
z80asm $1.bb
slrnk b:$1, b:$1/n/e
*/
/* 
Description: 

Sum each student's grades, with weighting according to point value of report, 
	exam, etc.  
Drop any grade entered as negative.  
Calculate average = sum grades / sum possible points.
Output sorted by name, and by average.

For structure of the data file for input to the program, see the usage message.

By J.A. Rupley, Tucson, Arizona
Coded for ECO C compiler, version 3.40
*/

/*
For example:

There are 7 expts, 2 exams, and a notebook grade.
The final is optional, and is optionally included in the input file below.
The low grade for the experiments is dropped and is given as a negative number.
Negative "0" is given as "-999".
Excused missing grades are given as "-999".
Point values of labs, exams, and notebook  =  (maxium credit) * (weight)
                   0     1     2     3     4     5     6     7     8     9
                   lab   lab   lab   lab   lab   exm   lab   lab   nbk   exm
                   100   200   100   100   200   250   100   200   300   250

weight             1     2     1     1     2     2.5   1     2     3     2.5

maxium credit      100   100   100   100   100   100   100   100   100   100

data given for each student are:
name      9 (or 10) grades, one for each lab, etc, on a 100 point scale

******************************************************************************
*/

#include <stdio.h>

#include <ctrlcnst.h>

#define STUDENTS	200		/* maximum class size		*/
#define GRADES		20		/* maximum number of 
						grades per student	*/



					/* data structure "students",
						to contain grade info
						and calculated averages	*/
struct students {
char *name;
int grades[GRADES];
float total;
float possible;
float average;
} ;

					/* data for each student in class */
struct students stu[STUDENTS];

         				/* an array of pointers to
						struct students, later set
						to stu, for use in sorting */
struct students *jarsort[STUDENTS];


					/* title, giving class, year, etc  */
char title[80];

					/* number of students in class and
						number of grades per student */
int num_student, num_grade;


					/* relative weight of each grade */
float weight[GRADES];

					/* full credit, ie maximum possible
						points for each grade	*/
int credit[GRADES];


					/* array to store student names,
						pointed to by name element
						of data structure stu	*/
char name[STUDENTS][24];



				/* main program			*/
main(argc, argv)
int argc; char *argv[];
{
	register int i, j;
	int putchar();
	int strcomp(), fltcomp(), swap();
	void read_data();
	void printf(), bsort(), exit();


					/* read data from file		*/
	read_data(argc, argv);


					/* calculate adjusted average etc */
						/* omit grades flagged "-" */
						/* sum points received
							and points possible */
						/* calculate average grade */
	for (i=0; i < num_student; i++) {
		stu[i].possible = stu[i].total = stu[i].average = 0;
		for (j=0; j < num_grade; j++) {
			if (stu[i].grades[j] < 0 )
				continue;
			else {
				 stu[i].total = stu[i].total 
						+ stu[i].grades[j] * weight[j];
				 stu[i].possible = stu[i].possible 
						+ credit[j] * weight[j];
			}
		}
		stu[i].average = 100 * stu[i].total / stu[i].possible;
	}


					/* initialize array of pointers to 
						structure elements, to be
						used in sorting subroutines,
						swap and comp		*/
	for (i = 0; i < num_student; i++)
		jarsort[i] = &stu[i];



					/* sort students by name
						and print		*/
	bsort(num_student, strcomp, swap);

	putchar(FF);
	printf("\n%-s\n", title);
	printf("Sorted  by student name\n\n");

	for (i = 0; i < num_student; i++) {
		printf("%3d %-23.23s ", (i + 1), jarsort[i]->name);
		printf("%5.0f %5.0f %7.2f\n", jarsort[i]->total, 
				jarsort[i]->possible, jarsort[i]->average);
	}


					/* sort students by grade
						and print		*/
	bsort(num_student, fltcomp, swap);

	putchar(FF);
	printf("\n%-s\n", title);
	printf("Sorted  by grade\n\n");

	for (i = 0; i < num_student; i++) {
		printf("%3d %-23.23s ", (i + 1), jarsort[i]->name);
		printf("%5.0f %5.0f %7.2f\n", jarsort[i]->total, 
				jarsort[i]->possible, jarsort[i]->average);

	}

					/* end with form feed		*/
	putchar(FF);
}				/* END OF MAIN				*/



				/* NOTE-- the swap routine exchanges
					pointers to structure records;
					the comp routines compare members of
					two structure records, which may
					be swapped later; the members checked
					here are the average grade of a 
					student (fltcomp), 
					and his name (strcomp).		*/


				/* swap contents of array elements x and y
					which are pointers to structure
					records tested in comp routines */
int swap(x, y)
int x, y;
{
	int *temp;

	temp = jarsort[x];
	jarsort[x] = jarsort[y];
	jarsort[y] = temp;

	returî (OK);

}				/* END OF SWAP				*/



				/* compare float contents of records x and y
				       for struct member average
					and for a descending sort	*/
int fltcomp(x, y)
int x,y;
{
	if (jarsort[x]->average < jarsort[y]->average)
		return (1);

	else if (jarsort[x]->average > jarsort[y]->average)
		return (-1);

	else if (jarsort[x]->average == jarsort[y]->average)
		return (0);

}				/* END OF FLTCOMP			*/



				/* compare string contents of records x and y
				       for struct member name
					and for an ascending sort	*/
int strcomp(x, y)
int x,y;
{
	int strcmp();

	return (strcmp(jarsort[x]->name, jarsort[y]->name));

}				/* END OF STRCOMP			*/



				/* usage message			*/
void use_mess()
{
	void puts();

puts("\nUsage:");
puts("A>sumgrade  <filename.typ>");
puts("   filename.typ has the following structure:");
puts("     line 0.  one-line title, with no tabs or ctrl characters");
puts("     line 1.  two INTEGER values, num_student  &  num_grade");
puts("     line 2.  num_grade FLOAT values of relative weights of each grade");
puts("     line 3.  num_grade INTEGER values of full credit for each grade");
puts("     line 4.  student name, then num_grade grades for that student");
puts("                only 23 characters of the name are read");
puts("                grades must be given as INTEGERS");
puts("                grades to be dropped are to be given as negatives");
puts("                excused missing or zero grades are given as -999");
puts("\n");
puts("      repeat line 4 for num_student students");
puts("      lines 1 to 4 can be more than one line, ie the input file can be");
puts("              freeform, with the restriction that there must be the");
puts("              proper number and sequence of numeric and alphabetic");
puts("              values; a crlf like a space terminates a value");
puts("      comments at the end of the file are not read by the program");
puts("              and any other information to be stored should be there");
puts("\n");
}				/* END OF USE_MESS			*/



				/* read data from file			*/
void read_data(argc, argv)
int argc; char *argv[];
{
	register int i, j;
	int t, c;
	int fscanf(), ungetc(), getc();
	int isdigit(), isalpha(), iscntrl();
	void printf(), exit(), puts(), use_mess();
	FILE *fptr, *fopen();


					/* open data file specified
						in the command line	*/
						/* print filename if given */
						/* else print usage message */
						/* exit on error	*/
	if (argc < 2) {
		use_mess();
		exit (ERROR);
	}
	else if ((fptr = fopen(argv[1],"r")) == NULL) {
		puts("\nSorry, I cannot open the file.\n\n");
		exit (ERROR);
	}
	printf("\nData from file = %s\n", argv[1]);


					/* read title and print		*/
	for (i = 0; i < 79; i++) {
		if (iscntrl(title[i] = getc(fptr)))
			break;
	}
	title[i] = '\0';
	printf("\n%-s\n", title);


					/* read number of students and
						number of grades per student
						and print     		*/
						/* exit on error	*/
	t = fscanf(fptr, "%d %d", &num_student, &num_grade);
	if (t <= 0) {
		printf("\nError in reading file\n");
		exit(ERROR);
	}
	printf("\nnum_student = %d   num_grade = %d\n", 
			num_student, num_grade);



					/* read relative weights for 
						each grade and print	*/
						/* exit on error	*/
	for (j = 0; j < num_grade; j++) {
		if (j % 10 == NULL)
			printf("\nweights =     ");
		t = fscanf(fptr, "%f", &weight[j]);
		if (t <= 0) {
			printf("\nError in reading file\n");
			exit(ERROR);
		}
		printf("%6.2f",weight[j]);
	}
	printf("\n");


					/* read points given as full credit
						for each grade and print */
						/* exit on error	*/
	for (j = 0; j < num_grade; j++) {
		if (j % 10 == NULL)
			printf("\nfull credit = ");
		t = fscanf(fptr, "%d", &credit[j]);
		if (t <= 0) {
			printf("\nError in reading file\n");
			exit(ERROR);
		}
		printf("%6d",credit[j]);
	}
	printf("\n");


					/* read name and set of grades for
						each student, and print	*/
						/* position pntr at name */
						/* read name to EOL or digit */
						/* read numgrade grades	*/
						/* print data as read	*/
						/* exit on error	*/
	for (i = 0; i < num_student; i++) {
		while(!isalpha(c = getc(fptr)))
			;
		ungetc(c, fptr);

		for (j = 0; j < 23; j++) {
			if (isdigit(name[i][j] = getc(fptr))
					|| iscntrl(name[i][j])) {
				ungetc(name[i][j], fptr);
				break;
			}
		}
		name[i][j] = '\0';
		stu[i].name = name[i];
		printf("\n%3d %-23.23s ", (i + 1), stu[i].name);

		for (j = 0; j < num_grade; j++) {
			t = fscanf(fptr, "%d", &stu[i].grades[j]);
			if (t <= 0) {
				printf("\nError in reading file\n");
				exit(ERROR);
			}
			if (j != NULL && (j % 10) == NULL)
				printf("\n                             ");
			printf("%5d", stu[i].grades[j]);
		}
	}
	printf("\n");

}				/* END OF READ_DATA			*/
