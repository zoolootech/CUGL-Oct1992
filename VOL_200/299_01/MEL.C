/*
---------------------------------------------------------------------------
filename: mel.c
author: g. m. crews
creation date: 28-Jul-1988
date of last revision: 19-Jul-1989

MEL is a comprehensive metalanguage input/output processor for engineering 
analysis programs. its purposes are 1) to save the programmer development time
when providing a user-friendly i/o interface, and 2) to establish a common
protocol for interprogram messaging. these routines can be used by any
analysis program for which a "dictionary" has been defined. see file
"mel.doc" for more information.

note: these functions will normally return 0 if no error was encountered, 
else a number (code) reflecting the type of error.

functions contained in this file:

    module #   name			 global scope?
    --------   -----------------------	 -------------
    1.0        meli_file		 yes
    1.1        end_of_data_in_file	 no
    1.2        meli			 yes
    1.2.1      compact_str		 no
    1.2.2      get_descrip_type 	 no
    1.2.2.1    name_match		 no
    1.2.3      get_param_name		 no
    1.2.4      get_param_values 	 no
    1.2.5      get_param_units		 no
    2.0        meli_descrip_type	 yes
    3.0        meli_num_params		 yes
    4.0        meli_param		 yes
    5.0        meli_data		 yes
    6.0        melo_init		 yes
    7.0        melo_data		 yes
    8.0        melo_file		 yes
    8.1        melo			 yes
    8.1.1      write_units_if_any	 no
---------------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>

/*
---------------------------------------------------------------------------
the following include file declares the dictionary that is to be used for
MEL i/o.  note that this file will be unique for every program that uses
this method of i/o.  (again, see file "mel.doc" for more information on how
to further customize or modify this file.)

it also contains "public" (global) function prototyping and miscellaneous 
data structures.
---------------------------------------------------------------------------
*/

#define MEL_INPUT
#define MEL_OUTPUT
#define MEL_PRIVATE
#define MEL_INIT
#include "mel.h"

/* "private" function prototyping:
   (visible only to other routines in this file) */

static int end_of_data_in_file(int);
static void compact_str(void);
static int get_descrip_type(void);
static int name_match(char *, char *, int);
static int get_param_name(void);
static int get_param_values(void);
static int get_param_units(void);
static void write_units_if_any(int);

/* "private" (local external) storage: */

static char descrip_str[MELI_MAX_DESCRIP_STR_LEN+1];
    /* temp storage for descriptor string.
       (read from file or given by MEL user). */
static char *descrip_str_ptr;
    /* current point of interest in above string. it is used by several 
       routines that "translate" the string into MEL data structure. it points 
       to the next character needing to be "translated". */
static int curr_descrip_index;
    /* what is descriptor's index into meli_descrip[] array? this allows
       other local routines to know which parameters to check for. */
static int datum_param_index;
    /* where does this parameter's data go in meli_datum? */

/* the following variables are also local and deal with parameter handling: */

static int equals_sign_found_flag;
    /* will there be a need to try and read a parameter value? */
static int curr_param_index;
    /* what is parameter's index into meli_descrip.param[] array? */
static int param_name_found_flag;
    /* as a safety measure, note when a parameter name has been found
       for a descriptor (and do not let values without names follow). */
static int left_parenthesis_found_flag;
    /* will there be a need to try and read units? */

/*
---------------------------------------------------------------------------
module 1.0

get a descriptor string from a file and put data into structure for easy
access. that is:
    read a descriptor from a file into "private" storage (descrip_str).
    check for file read errors.
    stop reading after semicolon encountered.
    put a copy into meli_descriptor_string.
    translate this string into MEL data structure (meli_datum).

algorithm synopsis: read characters from input stream until semi-
colon (end-of-descriptor) is encountered, while putting them into
private storage string.
---------------------------------------------------------------------------
*/
int meli_file(
FILE *input_file_handle)
{
    static int curr_line_num = 1;
	/* assume that reading starts with the first line in a text file
	   and continues incrementally (one descriptor at a time). thus,
	   increment this variable everytime a newline character is
	   encountered. (that a descriptor may span several lines.) */

    int i;
	/* loop counter. */

    int ch;
	/* current char from input stream. */

    meli_datum.start_line = curr_line_num;
	/* save the starting line number for this descriptor. */

    /* read from the input stream one character at a time: */

    for (i = 0; i < MELI_MAX_DESCRIP_STR_LEN; i++) {
	ch = fgetc(input_file_handle);

	/* check for a file read error: */
	if (ferror(input_file_handle)) {
	    mel_err.type = mel_read_err;
	    mel_err.start_line = meli_datum.start_line;
	    mel_err.end_line = curr_line_num;
	    strcpy(mel_err.msg, "MEL input file");
	    return mel_read_err;
	};

	/* also error if unexpected end of file before semicolon: */
	if ((ch == EOF) || (feof(input_file_handle))) {

	    /* an exception is for the case of EOF right after a semicolon
	       (not counting any whitespace that might also have been read).
	       this is called the "end-of-data" error (which really isn't an
	       error at all, but a means of letting the caller know that no
	       more descriptors are in the file). */
	    if (end_of_data_in_file(i)) {
		mel_err.type = mel_end_of_data_err;
		mel_err.start_line = meli_datum.start_line;
		mel_err.end_line = curr_line_num;
		strcpy(mel_err.msg, "MEL input file");
		return mel_end_of_data_err;
	    } else {
		mel_err.type = mel_end_of_file_err;
		mel_err.start_line = meli_datum.start_line;
		mel_err.end_line = curr_line_num;
		strcpy(mel_err.msg, "Missing semicolon, MEL input file");
		return mel_end_of_file_err;
	    }
	};

	if (ch == ';') break;
	    /* break the loop if a semicolon is encountered. */

	if (ch == '\n') curr_line_num++;
	    /* increment line number count for a new line */

	/* now add this character to the descriptor string and go get the
	   next character: */
	descrip_str[i] = (char)ch;
    }

    /* the descriptor has now been successfully read into the descriptor
       string. time to tidy-up. */

    descrip_str[i++] = (char)ch;  /* tack on the semicolon */
    descrip_str[i] = '\0';
    meli_datum.end_line = curr_line_num;
    strcpy(meli_descriptor_string, descrip_str);
	/* make copy in case user wants to see it. */

    /* lastly, translate it (put data into meli_datum): */
    return meli();
}

/*
---------------------------------------------------------------------------
module 1.1

has all the data been read from the MEL input file?

this routine is called when an end of file has been encountered. we must make
sure that the user has not forgotten to append a semicolon on the last
descriptor.  thus, it is an error condition if anything except whitespace
has been read since the last semicolon (end-of-descriptor character).

return 1 if only whitespace has been read since last semicolon.
return 0 if other (data) characters have been read since last semicolon.
---------------------------------------------------------------------------
*/
static int end_of_data_in_file(
int str_len)
{
    descrip_str[str_len] = '\0';
	/* make string out of characters read since last semicolon. */

    compact_str();    /* remove whitespace from this string */

    if (strlen(descrip_str)) return 0;
    else return 1;
	/* if anything left over after compaction, then user has made
	   an error. */
}

/*
---------------------------------------------------------------------------
module 1.2

interpret a MEL descriptor:

this function scans a descriptor string and interprets it.

the results are put into the mel_data structure, or if an error occurs,
into the mel_err structure.

algorithm synopsis: first translate what kind of descriptor this is,
then loop while translating each parameter.

---------------------------------------------------------------------------
*/
int meli()
{
    int result;

    param_name_found_flag = 0;
	/* allow parameter values to be input, in order, without their
	   names (until a parameter name is encountered).  see
	   function get_param_name() for more info. */

    meli_datum.num_param = 0;
	/* how many parameter have been processed so far? */

    /* first place string into local storage, strip trailing semicolon,
       and "compact" it: */

    strncpy(descrip_str, meli_descriptor_string,
	(size_t) MELI_MAX_DESCRIP_STR_LEN);
    descrip_str[MELI_MAX_DESCRIP_STR_LEN+1] = '\0';
	/* ensure no overflow. */

    compact_str();

    /* next keep evaluating until an error is encountered or the end of
       the string is reached (functions return 0 if no err): */
	
    if (result = get_descrip_type()) return result;
	/* every descriptor must have a type. */

    /* there may be a variable number of parameters and values: */

    while (*descrip_str_ptr != '\0') {
	/* keep going until end of descrip_str reached */

	datum_param_index = meli_datum.num_param;
	    /* where will new parameter data go? */

	/* a parameter usually consists of its name, value, and units: */
	if (result = get_param_name()) return result;
	if (result = get_param_values()) return result;
	if (result = get_param_units()) return result;

	meli_datum.num_param++;
	    /* another parameter has been processed! */
    };

    return 0;
	/* the end of the descriptor string has been reached. */
}

/*
---------------------------------------------------------------------------
module 1.2.1

compact the descrip_str string:
    flush whitespace except blanks within single quotes or curly brackets
	(strings and arrays).
    flush comments (text within double quotes).

algorithm synopsis:  since the string can only shrink while being compacted,
use pointers to keep track of current positions and overwrite in place.
---------------------------------------------------------------------------
*/
static void compact_str()
{
    char *p;    /* pointer to current end of compacted string */
    char *q;    /* pointer to current end of uncompacted string */

    int single_quote_flag = 0;
    int curly_brackets_flag = 0;
	/* all blanks within a descriptor are flushed except those within
	   single quotes or curly brackets. */

    int double_quote_flag = 0;
	/* all text within double quotes are comments and are flushed. */

    p = q = descrip_str;    /* start at the beginning */

    do { /* look at current character, does it need to be flushed? */

	/* if the character is a double quote, flush all chars between it
	   and the next double quote: */
	if (*q == '"') {
	    double_quote_flag = !double_quote_flag;
	    q++;     /* has effect of flushing this character (typical) */
	    continue;   /* go to next character (typical) */
	}

	/* if the character is a newline, flush it */
	else if (*q == '\n') {
	    q++;
	    continue;
	}

	/* flush all chars if we are currently between double quotes: */
	else if (double_quote_flag) {
	    q++;
	    continue;
	}

	/* test for single quote or curly brackets and adjust flag value: */
	else if (*q == '\'') single_quote_flag = !single_quote_flag;
	else if (*q == '{') curly_brackets_flag = 1;
	else if (*q == '}') curly_brackets_flag = 0;

	/* if the character is some other whitespace char, except a blank,
	   also flush it: */
	else if ((*q == '\t') || (*q == '\v') || (*q == '\r') ||
	(*q == '\f')) {
	    q++;
	    continue;
	}

	/* if the character is a blank, flush it unless it is between
	   single quotes or curly brackets: */
	else if ((*q == ' ') && !single_quote_flag && !curly_brackets_flag) {
	    q++;
	    continue;
	}

	/* this character not to be flushed, so copy it in place */
	*p = *q;
	p++; q++;   /* get ready for next character */

    } while (*(q-1) != '\0');
	/* don't continue if last char was null terminator */
}

/*
---------------------------------------------------------------------------
module 1.2.2

determine current descriptor's index into meli_descrip array.
that is, which type of descriptor has just been read?
---------------------------------------------------------------------------
*/
static int get_descrip_type()
{

    int i;
	/* loop counter. */

    char temp_str[MELI_MAX_DESCRIP_STR_LEN+1];
	/* build up descriptor name here. */

    descrip_str_ptr = descrip_str;
	/* must reset current string position to start of string since
	   now time to start another descriptor. */

    /* create string to next comma or end of descrip string: */
    for (i = 0; i < MELI_MAX_DESCRIP_STR_LEN; i++) {
	if ((*descrip_str_ptr == ',') ||
	    (*descrip_str_ptr == ';') ||
	    (*descrip_str_ptr == '\0')) {
	    descrip_str_ptr++;	  /* get ready for next part */
	    temp_str[i] = '\0';
	    break;
	};

	temp_str[i] = *descrip_str_ptr++;
	/* add this char to temp string. */
    }

    /* find descrip name that matches this built up name: */

    for (i = 0; i < MELI_NUM_DESCRIP_NAMES; i++)
	if (name_match(meli_descrip[i].name, temp_str,
	    meli_descrip[i].min_name_len)) {
	    curr_descrip_index = meli_datum.descrip_type = i;
	    return 0;     /* descriptor type has been found */
	};

    /* error if went through entire list without finding match: */

    mel_err.type = mel_unknown_descrip_name_err;
    mel_err.start_line = meli_datum.start_line;
    mel_err.end_line = meli_datum.end_line;
    strncpy(mel_err.msg, temp_str, MEL_MAX_ERR_MSG_LEN);
    mel_err.msg[MEL_MAX_ERR_MSG_LEN+1] = '\0';
    return mel_unknown_descrip_name_err;
}

/*
---------------------------------------------------------------------------
module 1.2.2.1

determine if the starting characters in a test string start off with all
the characters in a reference string. returns: 0 = no, 1 = yes.
---------------------------------------------------------------------------
*/
static int name_match(
char test_str[],
char ref_str[],
int ref_str_len)
{
    int i;

    for (i = 0; i < ref_str_len; i++)
	if (ref_str[i] != test_str[i]) return 0;

    return 1;
}

/*
---------------------------------------------------------------------------
module 1.2.3

parameters usually are part of a descriptor.  get the next parameter name,
if any, and find out which parameter it is.  note that MEL also allows
parameter values to be input, without their names, assuming a certain order.

algorithm synopsis: copy next parameter starting at current position
in descrip_str.  if unrecognizable, it may be a value (parameter name
assumed) so allow for this possibility.

---------------------------------------------------------------------------
*/
static int get_param_name()
{
    int i;
    char temp_str[MELI_MAX_DESCRIP_STR_LEN+1];
    char *old_descrip_str_ptr;

    /* see if the maximum number of descriptors have already been read: */
    /* (must be able to read another or at end of descriptor string.) */
    /* this test prevents too many parameters from being read. */

    if ((meli_datum.num_param >= meli_descrip[meli_datum.descrip_type].
	max_num_param) && (*descrip_str_ptr != '\0')) {
	mel_err.type = mel_too_many_param_err;
	mel_err.start_line = meli_datum.start_line;
	mel_err.end_line = meli_datum.end_line;
	strcpy(mel_err.msg, descrip_str);
	return mel_too_many_param_err;
    };

    /* create string to next comma, equal's sign, semicolon, or null terminator.
       however, since this may be a value and not a parameter name, remember
       the old string posn just in case: */

    old_descrip_str_ptr = descrip_str_ptr;

    for (i = 0; i < MELI_MAX_DESCRIP_STR_LEN; i++) {
	if ((*descrip_str_ptr == ',') ||
	    (*descrip_str_ptr == ';') ||
	    (*descrip_str_ptr == '\0')) {
	    equals_sign_found_flag = 0;
	    descrip_str_ptr++;
	    temp_str[i] = '\0';
	    break;
	} else if (*descrip_str_ptr == '=') {
	    equals_sign_found_flag = 1;
	    descrip_str_ptr++;
	    temp_str[i] = '\0';
	    break;
	};

	temp_str[i] = *descrip_str_ptr++;
    }

    /* find the parameter name that matches this built up name (assuming
       it is a name): */

    for (i = 0; i < meli_descrip[curr_descrip_index].max_num_param; i++)
	if (name_match(meli_descrip[curr_descrip_index].param[i].name,
	    temp_str, meli_descrip[curr_descrip_index].param[i].min_name_len)) {
	    curr_param_index = i;
	    param_name_found_flag = 1;
	    meli_datum.param[datum_param_index].name_index = i;
	    return 0;   /* the parameter's name has been found! */
	};

    /* if we get here then we went through the entire list without finding
       a match.  however, this may not be an error for the case of
       parameter values being input in order (without their names): */

    /* if an equal's sign was found, then this is definitely an error: */
    if (equals_sign_found_flag) {
	mel_err.type = mel_unknown_param_name_err;
	mel_err.start_line = meli_datum.start_line;
	mel_err.end_line = meli_datum.end_line;
	strcpy(mel_err.msg, "Or misplaced equal's sign");
	return mel_unknown_param_name_err;
    };

    /* in order to avoid confusion, once a parameter has been found, make it
       no longer legal to enter just parameter values alone: */
    if (param_name_found_flag) {
	mel_err.type = mel_missing_param_name_err;
	mel_err.start_line = meli_datum.start_line;
	mel_err.end_line = meli_datum.end_line;
	strcpy(mel_err.msg, "Or unrecognizable parameter name");
	return mel_missing_param_name_err;
    };

    /* prepare for the next routine to read the next parameter value in
       order: */

    curr_param_index = datum_param_index;
    meli_datum.param[datum_param_index].name_index = curr_param_index;
    equals_sign_found_flag = 1;   /* fake it */
    descrip_str_ptr = old_descrip_str_ptr;
	/* restore pointer so that value may be read by next routine. */
    return 0;
}

/*
---------------------------------------------------------------------------
module 1.2.4

a parameter's value follows a parameter's name.  read the value(s), if any,
and store them.
---------------------------------------------------------------------------
*/
static int get_param_values()
{
    int i;
    char temp_str[MELI_MAX_DESCRIP_STR_LEN+1];
    enum mel_data_types data_type;
    int result;
    int curr_temp_posn;
    int curr_array_index = 0;
    int end_of_temp_str;
    int single_quote_found_flag = 0;

    /* create string to next comma, semicolon, or null terminator:
       (an exception is to allow a comma or semicolon within a single-
       quoted substring.) note: string may include array of values in
       {brackets}. */

    for (i = 0; i < MELI_MAX_DESCRIP_STR_LEN; i++) {
	if (*descrip_str_ptr == '\'')
	    single_quote_found_flag = !single_quote_found_flag;

	if (single_quote_found_flag) {
	    if (*descrip_str_ptr == '\0') {
		left_parenthesis_found_flag = 0;
		descrip_str_ptr++;
		temp_str[i] = '\0';
		break;
	    };
	} else if ((*descrip_str_ptr == ',') ||
	    (*descrip_str_ptr == ';') ||
	    (*descrip_str_ptr == '\0')) {
	    left_parenthesis_found_flag = 0;
	    descrip_str_ptr++;
	    temp_str[i] = '\0';
	    break;
	} else if (*descrip_str_ptr == '(') {
	    left_parenthesis_found_flag = 1;
	    descrip_str_ptr++;
	    temp_str[i] = '\0';
	    break;
	};

	temp_str[i] = *descrip_str_ptr++;
    }

    /* first see if the value of the parameter is "unknown": */
    if (!strcmp(temp_str, "unknown")) {
	meli_datum.param[datum_param_index].unknown_flag = 1;
	return 0;
    } else meli_datum.param[datum_param_index].unknown_flag = 0;

    /* scan the string and interpret its value(s) (depending on which kind of
       parameter this value belongs to): */

    data_type = meli_descrip[curr_descrip_index].param[curr_param_index].type;

    switch (data_type) {

	case (mel_bool):   /* read true or false (default is true) */
	    if ((temp_str[0] == '\0') || (temp_str[0] != 'f'))
		meli_datum.param[datum_param_index].data.integer = 1;
	    else
		meli_datum.param[datum_param_index].data.integer = 0;

	    result = 1;  /* successful "reading" of value */
	    break;

	case (mel_int):    /* read an integer */
	    result = sscanf(temp_str, "%d",
	    &meli_datum.param[datum_param_index].data.integer);
	    break;

	case (mel_real):      /* read a real */
	    result = sscanf(temp_str, "%lf",
	    &meli_datum.param[datum_param_index].data.real);
	    break;

	case (mel_str):    /* read a string (may be surrounded by single 
			      quotes*/
	    /* string MUST have single quotes if has blanks: */
	    if (temp_str[0] == '\'')
		result = sscanf(&temp_str[1], "%[^']",
		    meli_datum.param[datum_param_index].data.string);
	    else
		result = sscanf(temp_str, "%s",
		    meli_datum.param[datum_param_index].data.string);
	    break;

	case (mel_int_array):

	    /* get ready to read numbers in array: */
	    end_of_temp_str = strlen(temp_str);

	    /* first make sure first and last characters in string were
	       curly brackets and adjust string length to get rid of them: */
	    if ((temp_str[0] != '{') || (temp_str[end_of_temp_str-1] != '}')) {
		mel_err.type = mel_missing_bracket_err;
		mel_err.start_line = meli_datum.start_line;
		mel_err.end_line = meli_datum.end_line;
		strncpy(mel_err.msg, temp_str, MEL_MAX_ERR_MSG_LEN);
		mel_err.msg[MEL_MAX_ERR_MSG_LEN+1] = '\0';
		return mel_missing_bracket_err;
	    };
	    temp_str[--end_of_temp_str] = '\0';  /* last char was bracket */
	    while (temp_str[end_of_temp_str - 1] == ' ')
		temp_str[--end_of_temp_str] = '\0';  /* flush trailing blanks */
	    curr_temp_posn = 1;  /* first char was curly bracket */

	    /* now read in values: */
	    result = 1;      /* assume a good start */

	    while ((curr_temp_posn < end_of_temp_str) &&
		(result != 0) && (result != EOF)) {

		/* be careful not to overflow array: */
		if (curr_array_index > MELI_MAX_PARAM_INT_ARRAY_LEN)
		    break;

		/* purge till first non-blank character: */
		for (; curr_temp_posn < end_of_temp_str; curr_temp_posn++)
		    if (temp_str[curr_temp_posn] != ' ') break;

		/* read next integer into array: */
		result = sscanf(&temp_str[curr_temp_posn], "%d",
		    &meli_datum.param[datum_param_index].
		    data.integer_array[curr_array_index++]);

		/* purge till first blank character: */
		for (; curr_temp_posn < end_of_temp_str; curr_temp_posn++)
		    if (temp_str[curr_temp_posn] == ' ') break;
	    };

	    break;

	case (mel_real_array):
	    /* get ready to read numbers in array: */
	    end_of_temp_str = strlen(temp_str);

	    /* first make sure first and last characters in string were
	       curly brackets and adjust string length to get rid of them: */
	    if ((temp_str[0] != '{') || (temp_str[end_of_temp_str-1] != '}')) {
		mel_err.type = mel_missing_bracket_err;
		mel_err.start_line = meli_datum.start_line;
		mel_err.end_line = meli_datum.end_line;
		strncpy(mel_err.msg, temp_str, MEL_MAX_ERR_MSG_LEN);
		mel_err.msg[MEL_MAX_ERR_MSG_LEN+1] = '\0';
		return mel_missing_bracket_err;
	    };
	    temp_str[--end_of_temp_str] = '\0';  /* last char was bracket */
	    while (temp_str[end_of_temp_str - 1] == ' ')
		temp_str[--end_of_temp_str] = '\0';  /* flush trailing blanks */
	    curr_temp_posn = 1;  /* first char is curly bracket */
	    result = 1;

	    while ((curr_temp_posn < end_of_temp_str) &&
		(result != 0) && (result != EOF)) {

		/* be careful not to overflow array: */
		if (curr_array_index > MELI_MAX_PARAM_REAL_ARRAY_LEN)
		    break;

		/* purge till first non-blank character: */
		for (; curr_temp_posn < end_of_temp_str; curr_temp_posn++)
		    if (temp_str[curr_temp_posn] != ' ') break;

		/* read next real into array: */
		result = sscanf(&temp_str[curr_temp_posn], "%lf",
		    &meli_datum.param[datum_param_index].
		    data.real_array[curr_array_index++]);

		/* purge till first blank character: */
		for (; curr_temp_posn < end_of_temp_str; curr_temp_posn++)
		    if (temp_str[curr_temp_posn] == ' ') break;
	    };

	    break;

	case (mel_str_array):
	    /* get ready to read string into array: */
	    end_of_temp_str = strlen(temp_str);

	    /* first make sure first and last characters in string were
	       curly brackets and adjust string length to get rid of them: */
	    if ((temp_str[0] != '{') || (temp_str[end_of_temp_str-1] != '}')) {
		mel_err.type = mel_missing_bracket_err;
		mel_err.start_line = meli_datum.start_line;
		mel_err.end_line = meli_datum.end_line;
		strncpy(mel_err.msg, temp_str, MEL_MAX_ERR_MSG_LEN);
		mel_err.msg[MEL_MAX_ERR_MSG_LEN+1] = '\0';
		return mel_missing_bracket_err;
	    };
	    temp_str[--end_of_temp_str] = '\0';  /* last char was bracket */
	    while (temp_str[end_of_temp_str - 1] == ' ')
		temp_str[--end_of_temp_str] = '\0';  /* flush trailing blanks */
	    curr_temp_posn = 1;  /* first char is curly bracket */
	    result = 1;

	    while ((curr_temp_posn < end_of_temp_str) &&
		(result != 0) && (result != EOF)) {

		/* be careful not to overflow array: */
		if (curr_array_index > MELI_MAX_PARAM_STR_ARRAY_LEN)
		    break;

		/* purge till first non-blank character: */
		for (; curr_temp_posn < end_of_temp_str; curr_temp_posn++)
		    if (temp_str[curr_temp_posn] != ' ') break;

		/* read next real into array (remember 1st char may be
		   single quote, so two possibilites to consider): */
		if (temp_str[curr_temp_posn] == '\'') {

		    result = sscanf(&temp_str[++curr_temp_posn], "%[^']",
			meli_datum.param[datum_param_index].
			data.string_array[curr_array_index++]);

		    /* purge till first char after next single quote: */
		    for (; curr_temp_posn < end_of_temp_str; curr_temp_posn++)
			if (temp_str[curr_temp_posn] == '\'') break;
		    curr_temp_posn++;

		} else {

		    result = sscanf(&temp_str[curr_temp_posn], "%s",
			meli_datum.param[datum_param_index].
			data.string_array[curr_array_index++]);

		    /* purge till next blank character: */
		    for (; curr_temp_posn < end_of_temp_str; curr_temp_posn++)
			if (temp_str[curr_temp_posn] == ' ') break;
		}
	    };

	    break;
    }

    meli_datum.param[datum_param_index].array_len = curr_array_index;

    /* check to see if read was successful: */
    if ((result == 0) || (result == EOF)) {
	mel_err.type = mel_param_data_err;
	mel_err.start_line = meli_datum.start_line;
	mel_err.end_line = meli_datum.end_line;
	strncpy(mel_err.msg, temp_str, MEL_MAX_ERR_MSG_LEN);
	mel_err.msg[MEL_MAX_ERR_MSG_LEN+1] = '\0';
	return mel_param_data_err;
    };

    return 0;
}

/*
---------------------------------------------------------------------------
module 1.2.5

sometimes the units associated with parameter data may change:
---------------------------------------------------------------------------
*/
static int get_param_units()
{
    int i;
    char temp_str[MELI_MAX_DESCRIP_STR_LEN+1];

    /* are there any units to get? */
    /* if not then return null string for units. */
    if (!left_parenthesis_found_flag) {
	strcpy(meli_datum.param[datum_param_index].units, "");
	return 0;
    }

    /* create temporary string containing units: */
    for (i = 0; i < MELI_MAX_DESCRIP_STR_LEN; i++) {
	if (*descrip_str_ptr == ')') {
	    descrip_str_ptr++;
	    if (*descrip_str_ptr == ',' ||
		*descrip_str_ptr == ';') descrip_str_ptr++;
		/* normally, a comma or semicolon should follow the units.
		   lets be forgiving in case there isn't and not just flush
		   anything. */
	    temp_str[i] = '\0';
	    break;
	} else if (*descrip_str_ptr == '\0') {
	    mel_err.type = mel_missing_paren_err;
	    mel_err.start_line = meli_datum.start_line;
	    mel_err.end_line = meli_datum.end_line;
	    temp_str[i] = '\0';
	    strncpy(mel_err.msg, temp_str, MEL_MAX_ERR_MSG_LEN);
	    mel_err.msg[MEL_MAX_ERR_MSG_LEN+1] = '\0';
	    return mel_missing_paren_err;
	};
	temp_str[i] = *descrip_str_ptr++;
    }

    /* copy this string to units area: */
    temp_str[MELO_UNITS_STR_LEN+1] = '\0';
    strcpy(meli_datum.param[datum_param_index].units, temp_str);
    return 0;
}

/*
---------------------------------------------------------------------------
module 2.0

extract the type of descriptor that has been input.
see mel.doc for more information on using this procedure.
---------------------------------------------------------------------------
*/
char *meli_descrip_type()
{
    return meli_descrip[meli_datum.descrip_type].name;
}

/*
---------------------------------------------------------------------------
module 3.0

extract the number of parameters that has been input.
see mel.doc for more information on using this procedure.
---------------------------------------------------------------------------
*/
int meli_num_params()
{
    return meli_datum.num_param;
}

/*
---------------------------------------------------------------------------
module 4.0

extract information associated with the param_num'th parameter that has
been input.
---------------------------------------------------------------------------
*/
int meli_param(
int param_num,		      /* which parameter are we interested in? */
char *param,		      /* data associated with this param: */
union meli_param_data *data,
char *units,
int *array_len,
int *unknown_flag)
{

	/* is param_num within range of number of params input? */
	if (param_num >= 0 && param_num < meli_datum.num_param) {
	    /* yes it is! */
	    strcpy(param, meli_descrip[meli_datum.descrip_type].
		param[meli_datum.param[param_num].name_index].name);
	    *data = meli_datum.param[param_num].data;
	    strcpy(units, meli_datum.param[param_num].units);
	    *array_len = meli_datum.param[param_num].array_len;
	    *unknown_flag = meli_datum.param[param_num].unknown_flag;
	    return 0;	/* no errors (match found) */
	} else
	    /* index out of bounds, return an error condition: */
	    return 1;
}

/*
---------------------------------------------------------------------------
module 5.0

extract a particular parameter's value that has been input.
see mel.doc for more information on using this procedure.

algorithm synopsis: see if param string was one of the parameters input with
current descriptor.  if it is, then return with all the data about this
parameter that was input.
---------------------------------------------------------------------------
*/
int meli_data(
char *param,		      /* which parameter are we interested in? */
union meli_param_data *data,  /* return with data about this parameter: */
char *units,
int *array_len,
int *unknown_flag)
{
    int i;    /* loop counter */

    /* compare param with each parameter that was input: */
    for (i = 0; i < meli_datum.num_param; i++)

	/* does it match? (if not, continue looking.) */
	if (name_match(param, meli_descrip[meli_datum.descrip_type].
	    param[meli_datum.param[i].name_index].name, meli_descrip[
	    meli_datum.descrip_type].param[meli_datum.param[i].name_index].
	    min_name_len)) {

	    /* yes it does! */
	    *data = meli_datum.param[i].data;
	    strcpy(units, meli_datum.param[i].units);
	    *array_len = meli_datum.param[i].array_len;
	    *unknown_flag = meli_datum.param[i].unknown_flag;
	    return 0;	/* no errors (match found) */
	};

    /* no match was ever found, return an error condition: */
    return 1;
}

/*
---------------------------------------------------------------------------
module 6.0

initialize melo_datum for a descriptor of type *descrip_type.
---------------------------------------------------------------------------
*/
int melo_init(
char *descrip_type)
{
    int i;    /* loop counter */

    /* look for descriptor of given type: */
    for (i = 0; i < MELO_NUM_DESCRIP_NAMES; i++)

	/* does it match with any item in list of descriptor names? */
	if (!strcmp(descrip_type, melo_descrip[i].name)) {

	    /* yes, it does! */
	    melo_datum.descrip_type = i;
	    melo_datum.num_param = 0;
	    return 0;	 /* no errors */
	};

    /* no match was ever found, return an error condition: */
    return 1;
}

/*
---------------------------------------------------------------------------
module 7.0

for parameter *param, put data in rest of arguement list into melo_datum.
---------------------------------------------------------------------------
*/
int melo_data(
char *param,		      /* which parameter are we interested in? */
union melo_param_data *data,  /* data about this parameter: */
char units[],
int array_len,
int unknown_flag)
{
    int i;    /* loop counter */

    /* compare param with each parameter type: */
    for (i = 0; i < MELO_MAX_PARAMS; i++)

	/* does it match? (if not, continue looking.) */
	if (name_match(param, melo_descrip[melo_datum.descrip_type].
	    param[i].name, melo_descrip[melo_datum.descrip_type].
	    param[i].min_name_len)) {

	    /* yes it does! put it in the proper place: */
	    melo_datum.param[melo_datum.num_param].name_index = i;
	    melo_datum.param[melo_datum.num_param].data = *data;
	    strcpy(melo_datum.param[melo_datum.num_param].units, units);
	    melo_datum.param[melo_datum.num_param].array_len = array_len;
	    melo_datum.param[melo_datum.num_param].unknown_flag = unknown_flag;
	    melo_datum.num_param++;
	    return 0;	/* no errors (match found) */
	};

    /* no match was ever found, return an error condition: */
    return 1;
}

/*
---------------------------------------------------------------------------
module 8.0

write MEL data to the output file.
---------------------------------------------------------------------------
*/
int melo_file(
FILE *output_file_handle,
int verbose_flag)
{
    mel_err.type = mel_no_err;	/* assume no error will occur */

    melo(verbose_flag);  /* translate data structure to string */

    /* now output the string to file: */

    fprintf(output_file_handle, "%s", descrip_str);
    if (ferror(output_file_handle)) {
	mel_err.type = mel_write_err;
	strcpy(mel_err.msg, "MEL output file");
	return mel_write_err;
    };

    return 0;

}

/*
---------------------------------------------------------------------------
module 8.1

write MEL data to the descriptor string. that is, using the output dictionary,
translate data from the melo_datum data structure into a string of data. see
the file mel.doc for more information.
---------------------------------------------------------------------------
*/
void melo(
int verbose_flag)
{
    int i, j;   /* loop counters */
    enum mel_data_types type;   /* temp storage */
    char temp_str[MELO_MAX_DESCRIP_STR_LEN+1];	/* temp storage */

    mel_err.type = mel_no_err;	/* assume no error will occur */

    /* will the "output" string be verbose (human readable) or terse 
       (compressed for machine efficiency/readability)? */

    if (verbose_flag) {
	/* first write descriptor type: */
	strcpy(descrip_str, melo_descrip[melo_datum.descrip_type].name);

	/* next, write its parameters and parameter values: */
	for (i = 0; i < melo_datum.num_param; i++) {

	    /* which parameter is it? */
	    sprintf(temp_str, ",\n    %s =",
		melo_descrip[melo_datum.descrip_type].
		param[melo_datum.param[i].name_index].name);
	    strcat(descrip_str, temp_str);

	    /* is its value known? (if not, write "unknown" and
	       proceed to next parameter.) */
	    if (melo_datum.param[i].unknown_flag) {
		strcat(descrip_str, " unknown");
		continue;
	    };

	    /* what type of value is it (int, array, etc.)? */

	    type = melo_descrip[melo_datum.descrip_type].
		param[melo_datum.param[i].name_index].type;

	    switch (type) {

		case (mel_bool):

		    if (melo_datum.param[i].data.integer)
			strcat(descrip_str, " true");
		    else
			strcat(descrip_str, " false");
		    break;

		case (mel_int):

		    sprintf(temp_str, " %d", melo_datum.param[i].data.integer);
		    strcat(descrip_str, temp_str);
		    write_units_if_any(i);
		    break;

		case (mel_real):

		    sprintf(temp_str, " %lg", melo_datum.param[i].data.real);
		    strcat(descrip_str, temp_str);
		    write_units_if_any(i);
		    break;

		case (mel_str):

		    sprintf(temp_str, " '%s'", melo_datum.param[i].data.string);
		    strcat(descrip_str, temp_str);
		    write_units_if_any(i);
		    break;

		case(mel_int_array):

		    strcat(descrip_str, "{");

		    for (j = 0; j < melo_datum.param[i].array_len; j++) {
			if (j != 0) strcat(descrip_str, " ");
			if (!(j % 5)) strcat(descrip_str, "\n        ");
			sprintf(temp_str, "%d",
			    melo_datum.param[i].data.integer_array[j]);
			strcat(descrip_str, temp_str);
		    }

		    strcat(descrip_str, "\n    }");
		    write_units_if_any(i);
		    break;

		case(mel_real_array):

		    strcat(descrip_str, "{");

		    for (j = 0; j < melo_datum.param[i].array_len; j++) {
			if (j != 0) strcat(descrip_str, " ");
			if (!(j % 3)) strcat(descrip_str, "\n        ");
			sprintf(temp_str, "%lg",
			    melo_datum.param[i].data.real_array[j]);
			strcat(descrip_str, temp_str);
		    }

		    strcat(descrip_str, "\n    }");
		    write_units_if_any(i);
		    break;

		case(mel_str_array):

		    strcat(descrip_str, "{");

		    for (j = 0; j < melo_datum.param[i].array_len; j++) {
			strcat(descrip_str, "\n        ");
			sprintf(temp_str, "'%s'",
			    melo_datum.param[i].data.string_array[j]);
			strcat(descrip_str, temp_str);
		    }

		    strcat(descrip_str, "\n    }");
		    write_units_if_any(i);
		    break;
	    }
	}

	strcat(descrip_str, ";\n");

    } else {   /* terse output */

	/* first write descriptor type: */
	strncpy(descrip_str, melo_descrip[melo_datum.descrip_type].name,
	    melo_descrip[melo_datum.descrip_type].min_name_len);
	descrip_str[melo_descrip[melo_datum.descrip_type].min_name_len] = '\0';

	/* next, write its parameters and parameter values: */
	for (i = 0; i < melo_datum.num_param; i++) {

	    /* which parameter is it? */
	    strcat(descrip_str,",");
	    strncpy(temp_str, melo_descrip[melo_datum.descrip_type].
		param[melo_datum.param[i].name_index].name, melo_descrip[
		melo_datum.descrip_type].param[melo_datum.param[i].
		name_index].min_name_len);
	    temp_str[melo_descrip[melo_datum.descrip_type].param[melo_datum.
	    param[i].name_index].min_name_len] = '\0';
	    strcat(descrip_str, temp_str);
	    strcat(descrip_str, "=");

	    /* is its value known? (if not, write "unknown" and
	       proceed to next parameter.) */
	    if (melo_datum.param[i].unknown_flag) {
		strcat(descrip_str, "unknown");
		continue;
	    };

	    /* what type of value is it (int, array, etc.)? */

	    type = melo_descrip[melo_datum.descrip_type].
		param[melo_datum.param[i].name_index].type;

	    switch (type) {

		case (mel_bool):

		    if (melo_datum.param[i].data.integer)
			strcat(descrip_str, "t");
		    else
			strcat(descrip_str, "f");
		    break;

		case (mel_int):

		    sprintf(temp_str, "%d", melo_datum.param[i].data.integer);
		    strcat(descrip_str, temp_str);
		    write_units_if_any(i);
		    break;

		case (mel_real):

		    sprintf(temp_str, "%lg", melo_datum.param[i].data.real);
		    strcat(descrip_str, temp_str);
		    write_units_if_any(i);
		    break;

		case (mel_str):

		    sprintf(temp_str, "'%s'", melo_datum.param[i].data.string);
		    strcat(descrip_str, temp_str);
		    write_units_if_any(i);
		    break;

		case(mel_int_array):

		    strcat(descrip_str, "{");

		    for (j = 0; j < melo_datum.param[i].array_len; j++) {
			if (j != 0) strcat(descrip_str, " ");
			sprintf(temp_str, "%d",
			    melo_datum.param[i].data.integer_array[j]);
			strcat(descrip_str, temp_str);
		    }

		    strcat(descrip_str, "}");
		    write_units_if_any(i);
		    break;

		case(mel_real_array):

		    strcat(descrip_str, "{");

		    for (j = 0; j < melo_datum.param[i].array_len; j++) {
			if (j != 0) strcat(descrip_str, " ");
			sprintf(temp_str, "%lg",
			    melo_datum.param[i].data.real_array[j]);
			strcat(descrip_str, temp_str);
		    }

		    strcat(descrip_str, "}");
		    write_units_if_any(i);
		    break;

		case(mel_str_array):

		    strcat(descrip_str, "{");

		    for (j = 0; j < melo_datum.param[i].array_len; j++) {
			if (j != 0) strcat(descrip_str, " ");
			sprintf(temp_str, "'%s'",
			    melo_datum.param[i].data.string_array[j]);
			strcat(descrip_str, temp_str);
		    }

		    strcat(descrip_str, "}");
		    write_units_if_any(i);
		    break;
	    }
	}

	strcat(descrip_str, ";");
    }
}

/*
---------------------------------------------------------------------------
module 8.1.1

write the units associated with a parameter to the end of the descriptor
string descrip_str. don't bother if no units associated with this parameter.
---------------------------------------------------------------------------
*/
static void write_units_if_any(
int param_num)
{
    char temp_str[MELO_MAX_DESCRIP_STR_LEN+1];

    if (!strlen(melo_datum.param[param_num].units)) return;

    sprintf(temp_str, " (%s)", melo_datum.param[param_num].units);
    strcat(descrip_str, temp_str);
}
