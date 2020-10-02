/*
---------------------------------------------------------------------------
filename: mel_test.c

this file contains example code for MEL - a metalanguage data processor.
it may be used as a driver by customizing modules: 1.1, 2.1, 3.0, and 4.0.

functions contained in this file:

module #   name
--------   ------------------------------
0.0	   main
1.0	   initialize
1.1	   perform_global_initializations
2.0	   input_MEL_data
2.1	   translate_input_datum
3.0	   solve_problem
4.0	   output_MEL_data
---------------------------------------------------------------------------
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* MEL interface: */
#define MEL_INPUT
#define MEL_OUTPUT
#include "mel.h"

void main(int, char *[]);
void initialize(void);
void perform_global_initializations(void);
void input_MEL_data(void);
void translate_input_datum(void);
void solve_problem(void);
void output_MEL_data(void);

/* i/o/err file data: */
FILE *input_file_handle;
FILE *output_file_handle;
FILE *error_file_handle;
#define MAX_FILENAME_LEN 80
char input_filename[MAX_FILENAME_LEN+1] = "stdin";
char output_filename[MAX_FILENAME_LEN+1] = "stdout";
char error_filename[MAX_FILENAME_LEN+1] = "stderr";

/* other program global data (specific to this example): */
#define MAX_STRING_LEN 80
char program[MAX_STRING_LEN];
char date[MAX_STRING_LEN+1];
char label[MAX_STRING_LEN+1];
char output_format[MAX_STRING_LEN+1];
int verbose_flag;
int message_code;
char message_text[MAX_STRING_LEN+1];
int label_flag,
    output_format_flag,
    message_code_flag,
    message_text_flag;
    /* has any of the above data been changed with new input data? */

/*
---------------------------------------------------------------------------
module 0.0
---------------------------------------------------------------------------
*/
void main(
int argc,	/* command line args specify file i/o (see below) */
char *argv[])
{

    /* first, see if i/o files are other than the default: */
    if (argc > 1) strncpy(input_filename, argv[1], MAX_FILENAME_LEN);
    if (argc > 2) strncpy(output_filename, argv[2], MAX_FILENAME_LEN);
    if (argc > 3) strncpy(error_filename, argv[3], MAX_FILENAME_LEN);

    initialize();
	/* global program initializations. */

    input_MEL_data();
	/* a standard meta-language input format is being used.
	   get input for problem to be solved. */

    solve_problem();
	/* generate output from input. */

    output_MEL_data();
	/* standard meta-language output is used.  write results in this
	   format to file */
}

/*
---------------------------------------------------------------------------
module 1.0
---------------------------------------------------------------------------
*/
void initialize()
{
    struct tm *newtime;      /* so we can write current time/date */
    time_t aclock;
    char *time_msg;

    time(&aclock);		    /* get time in seconds */
    newtime = localtime(&aclock);   /* convert time to struct tm */
    time_msg = asctime(newtime);    /* pointer to time as ascii string */
    time_msg[24] = '\0';	    /* get rid of \n char */
    strcpy(date, time_msg);

    /* open i/o/err files: */

    if (strcmp(input_filename, "stdin") == 0)
	input_file_handle = stdin;
    else if ((input_file_handle = fopen(input_filename, "r")) == NULL) {
	printf("Can't open file <%s>.\n", input_filename);
	exit(0);
    };
    if (strcmp(output_filename, "stdout") == 0)
	output_file_handle = stdout;
    else if ((output_file_handle = fopen(output_filename, "w")) == NULL) {
	printf("Can't open file <%s>.\n", output_filename);
	exit(0);
    };
    if (strcmp(error_filename, "stderr") == 0)
	error_file_handle = stderr;
    else if ((error_file_handle = fopen(error_filename, "w")) == NULL) {
	printf("Can't open file <%s>.\n", error_filename);
	exit(0);
    };

    perform_global_initializations();
	/* also perform other application specific initializations. */
}

/*
---------------------------------------------------------------------------
module 1.1

perform global data initializations that are application specific.
---------------------------------------------------------------------------
*/
void perform_global_initializations()
{
    strcpy(program, "TEST_MEL - a driver program for debugging MEL");
    label_flag = 0;
    output_format_flag = 0;
    verbose_flag = 1;
    message_code_flag = 0;
    message_text_flag = 0;
}

/*
---------------------------------------------------------------------------
module 2.0
---------------------------------------------------------------------------
*/
void input_MEL_data()
{
    while (!meli_file(input_file_handle)) {
	/* continue to read descriptors until there are no more. */

	if (!strcmp("end_of_data", meli_descrip_type())) break;
	    /* break if no more data left. */

	translate_input_datum();
	    /* save the information read and go read another descriptor. */
    }

    switch (mel_err.type) {
	/* find out why we left the above loop. */

	case (mel_no_err):
	    /* must have encountered an end_of_data descriptor. */
	case (mel_end_of_data_err):
	    /* this is not really an error, it just means no more data
	       to follow. */
	    break;

	default:
	    /* some sort of read error must have occured.
	       tell user about error and exit program. */

	    fprintf(error_file_handle,
		"\nUnrecoverable error in MEL data input file.");
	    if (mel_err.start_line)
		fprintf(error_file_handle,
		    "\n(Look at lines on or after %d, through %d.)",
		    mel_err.start_line, mel_err.end_line);
	    fprintf(error_file_handle, "\nData = <%s>",
		meli_descriptor_string);
	    fprintf(error_file_handle, "\nError number %d, %s.",
		(int)mel_err.type, mel_err_msg[(int)mel_err.type]);
	    fprintf(error_file_handle, "\nAdd info: <%s>.", mel_err.msg);
	    exit(0);
    }
}

/*
---------------------------------------------------------------------------
module 2.1

transfer input data from meli_datum data structure into global program
variables specific to this application.
---------------------------------------------------------------------------
*/
void translate_input_datum()
{
    union meli_param_data data;        /* data to be translated: */
    char units[MELI_UNITS_STR_LEN+1];
    int array_len;
    int unknown_flag;

    char *descrip_type;    /* pointer to meli_datum.descrip_type */

    descrip_type = meli_descrip_type();    /* what has just been read? */

    if (!strcmp("program_data", descrip_type)) {

	if (label_flag = !meli_data("label", &data, units,
	    &array_len, &unknown_flag))
	    strcpy(label, data.string);

    } else if (!strcmp("program_options", descrip_type)) {

	if (output_format_flag = !meli_data("output_format",
	    &data, units, &array_len, &unknown_flag)) {
	    strcpy(output_format, data.string);
	    if (!strcmp(output_format, "terse")) verbose_flag = 0;
	};

    } else if (!strcmp("message", descrip_type)) {

	if (message_code_flag = !meli_data("code",
	    &data, units, &array_len, &unknown_flag))
	    message_code = data.integer;
	if (message_text_flag = !meli_data("text",
	    &data, units, &array_len, &unknown_flag))
	    strcpy(message_text, data.string);
    };
}

/*
---------------------------------------------------------------------------
module 3.0

solve problem, that is, generate output data from input data.
---------------------------------------------------------------------------
*/
void solve_problem()
{
    /* no code required for this example. */
}

/*
---------------------------------------------------------------------------
module 4.0

transfer application specific output data into melo_datum data structure(s)
and output to file.
---------------------------------------------------------------------------
*/
void output_MEL_data()
{
    union melo_param_data data;        /* data to be translated: */
    char units[MAX_STRING_LEN+1];
    int array_len = 0;
    int unknown_flag = 0;

    units[0] = '\0';  /* no units to output */

    /* first output program_data descriptor: */
    melo_init("program_data");
    strcpy(data.string, program);
    melo_data("program", &data, units, array_len, unknown_flag);
    strcpy(data.string, date);
    melo_data("date", &data, units, array_len, unknown_flag);
    strcpy(data.string, input_filename);
    melo_data("input_filename", &data, units, array_len, unknown_flag);
    strcpy(data.string, output_filename);
    melo_data("output_filename", &data, units, array_len, unknown_flag);
    strcpy(data.string, error_filename);
    melo_data("error_filename", &data, units, array_len, unknown_flag);
    if (label_flag) {
	strcpy(data.string, label);
	melo_data("label", &data, units, array_len, unknown_flag);
    };
    melo_file(output_file_handle, verbose_flag);

    /* next output program_options descriptor if required: */
    if (output_format_flag) {
	melo_init("program_options");
	strcpy(data.string, output_format);
	melo_data("output_format", &data, units, array_len, unknown_flag);
	melo_file(output_file_handle, verbose_flag);
    };

    /* next output message descriptor if required: */
    if (message_code_flag || message_text_flag) {
	melo_init("message");
	if (message_code_flag) {
	    data.integer = message_code;
	    melo_data("code", &data, units, array_len, unknown_flag);
	};
	if (message_text_flag) {
	    strcpy(data.string, message_text);
	    melo_data("text", &data, units, array_len, unknown_flag);
	};
	melo_file(output_file_handle, verbose_flag);
    };

    /* finally, output end_of_data descriptor: */
    melo_init("end_of_data");
    melo_file(output_file_handle, verbose_flag);
}
