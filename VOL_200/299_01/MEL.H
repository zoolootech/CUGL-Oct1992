/*
------------------------------------------------------------------------
filename: mel.h
author: g. m. crews
creation date: 25-Jul-1988
date of last revision: 19-Jul-1989

the following defines the input and output interface (called "dictionaries") 
between MEL (a generic metalanguage processor) and the engineering analysis 
program it is being used for. (see files mel.doc and mel.c for more
information.)

the idea is to use "descriptors" (english-like strings) to make i/o more
readable to an analysis program user or checker. the dictionaries define the
"meanings" of the tokens in the string.

the following data objects form the interface:

    input:

	char meli_descriptor_string[]	input descriptor string
	meli_file(...)			interpret data from text file
	meli(...)			interpret data from above string
	meli_descrip_type(...)		get descriptor type
	meli_num_params(...)		get number parameters read
	meli_param(...) 		get value of particular parameter
	meli_data(...)			get parameter data

    output:

	char melo_descriptor_string[]	output descriptor string
	melo_init(...)			prepare to output descriptor
	melo_data(...)			put parameter data
	melo(...)			translate data to above string
	melo_file(...)			translate and send to text file

    errors:

	struct {...} mel_err		i/o error description


development and testing of any particular set of dictionaries may be 
facilitated with a debugging program called test_mel.  for more information, 
see the file test_mel.c. 

NOTE THAT THIS FILE MUST BE CUSTOMIZED FOR EACH TARGET APPLICATION PROGRAM.  
APPLICATION DEVELOPERS SHOULD SEARCH FOR THE WORD "CUSTOMIZED" FOR THE 
LOCATIONS OF NUMBERS AND OTHER DATA THAT NEED TO CHANGE TO REFLECT 
REQUIREMENTS OF THEIR OWN PARTICULAR APPLICATION PROGRAM.

THE PARTICULAR DICTIONARIES CURRENTLY DEFINED ARE SIMPLY A TEST FOR
ILLUSTRATIVE PURPOSES.	INPUT AND OUTPUT DICTIONARIES ARE IDENTICAL AND LOOK
LIKE:

    program_data,
	program = 'sss',
	date = 'sss',
	input_filename = 'sss',
	output_filename = 'sss',
	errors_filename = 'sss',
	label = 'sss';
    program_options,
	output_format = 'sss';	     "verbose or terse"
    message,
	code = nnn,
	text = 'sss';
    end_of_data;

(NOTE THAT nnn STANDS FOR A NUMBER AND 'sss' STANDS FOR A STRING.)

file content: the "public" interface is listed first, followed by the 
"private" interface. application programmers should treat MEL as a "black box" 
and thus only use the "public" definitions.  (note however that manifest
constants, etc., in the "private" area will have to be customized.)

nomemclature: In the following, "meli" is used to label input terms, "melo"
is used to label output terms, and "mel" is used for terms that may apply
to either input or output.

global conditional complilation flags (see mel.doc for more information on
these flags and when to use them) are used to control data hiding and maintain
object oriented approach used for MEL:

    MEL_INPUT	 -  define this flag when you wish to use the MEL input object
		    for your application programs.

    MEL_OUTPUT	 -  define this flag when you wish to use MEL for output.

    MEL_PRIVATE  -  users normally should not define this flag when including
		    mel.h in their application program files. it is used
		    to define "private" data structures, etc., for internal  
		    MEL routines (see mel.c).                        

    MEL_INIT     -  developers should also not define this flag for the same
		    reasons as above.
------------------------------------------------------------------------
*/

/*
------------------------------------------------------------------------
"public" declarations:
------------------------------------------------------------------------
*/

/* if using MEL for input, then must define the MEL input data object: */
#ifdef MEL_INPUT

/* firstly, define input constants (all must be CUSTOMIZED): */

#define MELI_MAX_DESCRIP_STR_LEN 256
    /* maximum number of characters in any input descriptor string. */
#define MELI_MAX_PARAMS 6
    /* maximum number of parameters for any descriptor (min = 1). */
#define MELI_MAX_PARAM_STR_LEN 80
#define MELI_MAX_PARAM_ARRAY_STR_LEN 1
    /* largest allowable parameter string lengths (min = 1) */
#define MELI_MAX_PARAM_INT_ARRAY_LEN 1
#define MELI_MAX_PARAM_REAL_ARRAY_LEN 1
#define MELI_MAX_PARAM_STR_ARRAY_LEN 1
    /* maximum number of elements in parameter data arrays (min = 1). */
#define MELI_UNITS_STR_LEN 80
    /* maximum length of units associated with any param (min = 1) */

/* secondly, define input data structures: */

union meli_param_data {
    int integer;
    double real;
    char string[MELI_MAX_PARAM_STR_LEN+1];
    int integer_array[MELI_MAX_PARAM_INT_ARRAY_LEN];
    double real_array[MELI_MAX_PARAM_REAL_ARRAY_LEN];
    char string_array[MELI_MAX_PARAM_STR_ARRAY_LEN]
		     [MELI_MAX_PARAM_ARRAY_STR_LEN+1];
};
/* this is used for input parameter data. it may either be an integer,
   real, string, array of integers, array of reals, or an array of
   strings. (to save space a union was used.) */

/* thirdly, define input variables: */

char meli_descriptor_string[MELI_MAX_DESCRIP_STR_LEN+1];
    /* global storage for the input descriptor string. */

/* lastly, define input functions (typically they return 0 if no error
   encountered, else some nonzero error code): */

int meli_file(FILE *meli_file_handle);
    /* read a descriptor string from the input stream and call meli().
       also, put copy of string read into meli_descriptor_string. */
int meli(void);
    /* translate meli_descriptor_string and put information into a private
       data structure (meli_datum). */
char *meli_descrip_type(void);
    /* return pointer to name of type of descriptor read by meli(). */
int meli_num_params(void);
    /* return number of parameters read by meli(). */
int meli_param(int param_num, char *param, union meli_param_data *data,
    char *units, int *array_len, int *unknown_flag);
    /* fill arguement list with param_num'th parameter read by meli().
       (start with param_num = 0.) */
int meli_data(char *param, union meli_param_data *data,
    char *units, int *array_len, int *unknown_flag);
    /* see if *param was input. if it was, then fill argument list with
       data from meli_datum. */

#endif /* MEL_INPUT */

/* if using MEL for output, must define the MEL output data object: */
#ifdef MEL_OUTPUT

/* firstly, define output constants (all must be CUSTOMIZED): */

#define MELO_MAX_DESCRIP_STR_LEN 256
    /* how many characters can be in an output descriptor string? */
#define MELO_MAX_PARAMS 6
    /* maximum number of parameters for any descriptor. */
#define MELO_MAX_PARAM_STR_LEN 80
#define MELO_MAX_PARAM_ARRAY_STR_LEN 1
    /* largest allowable parameter string length. */
#define MELO_MAX_PARAM_INT_ARRAY_LEN 1
#define MELO_MAX_PARAM_REAL_ARRAY_LEN 1
#define MELO_MAX_PARAM_STR_ARRAY_LEN 1
    /* maximum number of elements in array of parameter data. */
#define MELO_UNITS_STR_LEN 80
    /* maximum string length of any units associated with a param. */

/* secondly, define output data structures: */

union melo_param_data {
    int integer;
    double real;
    char string[MELO_MAX_PARAM_STR_LEN+1];
    int integer_array[MELO_MAX_PARAM_INT_ARRAY_LEN];
    double real_array[MELO_MAX_PARAM_REAL_ARRAY_LEN];
    char string_array[MELO_MAX_PARAM_STR_ARRAY_LEN]
		     [MELO_MAX_PARAM_ARRAY_STR_LEN+1];
};
/* this is for output parameter data. it may either be an integer, real,
   string, array of integers, array of reals, or an array of
   strings. (to save space a union was used.) */

/* thirdly, define output variables: */

char melo_descriptor_string[MELO_MAX_DESCRIP_STR_LEN+1];
    /* global storage for the output descriptor string. */

/* lastly, define output functions (typically return 0 if no error): */

int melo_init(char *descrip_type);
    /* initialize private data structure (melo_datum) to accept parameter
       data from following functions. output descriptor type will be
       descrip_type. returns 0 if no errors were encountered. */
int melo_data(char *param, union melo_param_data *data, char *units,
    int array_len, int unknown_flag);
    /* put data for parameter *param into the proper place in melo_datum. returns
       zero if no errors were encountered. */
void melo(int melo_verbose_flag);
    /* takes the information in melo_datum and translates it into
       melo_descriptor_string. user must set melo_verbose_flag = 1 to make
       output as readable as possible, set it equal to zero to make output
       as terse as possible (and still remain in MEL format). */
int melo_file(FILE *melo_file_handle, int melo_verbose_flag);
    /* take the information in melo_datum, translate it into
       melo_descriptor_string, and output it to file. */

#endif /* MEL_OUTPUT

/* now define data objects common to both input and output: */

/* if an error occurs, MEL will try and tell you what happened. so define 
   required error handling information: */

#define MEL_MAX_ERR_MSG_LEN 80

struct mel_errors {
    enum {   /* which error occured? */
	mel_no_err,
	mel_read_err,
	mel_write_err,
	mel_end_of_file_err,
	mel_end_of_data_err,
	mel_syntax_err,
	mel_unknown_descrip_name_err,
	mel_unknown_param_name_err,
	mel_missing_param_name_err,
	mel_param_data_err,
	mel_missing_paren_err,
	mel_too_many_param_err,
	mel_missing_bracket_err,
    } type;
    int start_line;   /* on which lines did err occur? */
    int end_line;     /* (meaningful for input only.) */
    char msg[MEL_MAX_ERR_MSG_LEN+1];   /* additional info describing err */
} mel_err;			       /* (not same as messages below). */

#define MEL_MAX_NUM_ERR_MESSAGES 13

#ifdef MEL_INIT

/* the following describes each type of enumerated error: */
char mel_err_msg[MEL_MAX_NUM_ERR_MESSAGES][MEL_MAX_ERR_MSG_LEN+1]
    ={"No errors encountered",
      "Can't read file",
      "Can't write file",
      "Unexpected end of file encountered",
      "End of input data encountered",
      "Descriptor/parameter syntax error",
      "Unknown descriptor name",
      "Unknown parameter name",
      "A (or another) parameter name was expected but is missing",
      "Unable to read parameter value(s) for this descriptor",
      "Missing right parenthesis while reading units",
      "Too many (or duplicate) parameters given for this descriptor",
      "Missing brackets around array data"};

#else

extern char mel_err_msg[MEL_MAX_NUM_ERR_MESSAGES][MEL_MAX_ERR_MSG_LEN+1];

#endif /* MEL_INIT */

/*
------------------------------------------------------------------------
"private" declarations:
------------------------------------------------------------------------
*/

/* hide private declarations from those routines that don't need it: */
#ifdef MEL_PRIVATE

/* the possible different types of data that can be associated with a given 
   descriptor's (i.e., input string's) parameters is important and is common 
   to both input and output structures: */ 
enum mel_data_types { 
    mel_none,	       /* default value */
    mel_bool,	       /* true or false */
    mel_int,           /* integer value */
    mel_real,          /* double floating point */
    mel_str,           /* character string */
    mel_int_array,     /* array of mel_int */
    mel_real_array,    /* array of mel_real */
    mel_str_array,     /* array of mel_str */
};

#ifdef MEL_INPUT

/* the following structure (meli_datum) is used to store information from
   the input descriptor string until the application program calls for it
   via MEL. */

struct {
    int start_line;
    int end_line;
	/* what is this descriptor's position in the input stream? 
	   (useful in telling user about where errors took place.) */
    int descrip_type;
	/* for the descriptor that was input, this is its index into
	   meli_descrip[] array of structures (see below). in other words,
	   which descriptor is it? */
    int num_param;
	/* number of parameters that were input. */
    struct {   /* data for each parameter input */
	int name_index;
	    /* this is the parameter's index into the meli_descrip.param[]
	       array, that is, which parameter is this? (see below) */
	union meli_param_data data;
	    /* data associated with this parameter. */
	char units[MELI_UNITS_STR_LEN+1];
	    /* what units are associated with the data? */
	int array_len;
	    /* if the data was an array, how long was it? */
	int unknown_flag;
	    /* is the value of this parameter "unknown"? */
    } param[MELI_MAX_PARAMS];
} meli_datum;

/* input dictionary manifest constants (all must be CUSTOMIZED): */

#define MELI_NUM_DESCRIP_NAMES 4
    /* how many descriptors there are. */
#define MELI_MAX_DESCRIP_NAME_LEN 15
    /* how many characters are in the longest descriptor name? */
#define MELI_MAX_PARAM_NAME_LEN 15
    /* how many characters in the longest parameter name? */

/* input dictionary declaration: */

/* define the data structure used to describe MEL input data: */
struct meli_descrips {   /* for each descriptor: */
    char name[MELI_MAX_DESCRIP_NAME_LEN+1];   
	/* descriptor's name */
    int min_name_len;   
	/* number of characters required to uniquely identify a 
	   descriptor from all others. */
    int max_num_param;  
	/* total number of parameters associated with this descriptor. */
    struct {   /* for each parameter: */
	char name[MELI_MAX_PARAM_NAME_LEN+1];   
	    /* parameter's name */
	int min_name_len;  
	    /* number of characters required to uniquely this parameter 
	       from all others for this particular descriptor. */
	enum mel_data_types type;
	    /* is it int, real, etc? */
    } param[MELI_MAX_PARAMS];
};

/* note that initialization will be required only in mel.c: */
#ifdef MEL_INIT

struct meli_descrips meli_descrip[MELI_NUM_DESCRIP_NAMES] = { /* CUSTOMIZED */
    {"program_data", 9, 6,
       {{"program", 1, mel_str},
	{"date", 1, mel_str},
	{"input_filename", 1, mel_str},
	{"output_filename", 1, mel_str},
	{"errors_filename", 1, mel_str},
	{"label", 1, mel_str}}},
    {"program_options", 9, 1,
       {{"output_format", 1, mel_str},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none}}},
    {"message", 1, 2,
       {{"code", 1, mel_int},
	{"text", 1, mel_str},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none}}},
    {"end_of_data", 1, 0,
       {{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none}}}};

#else

extern struct meli_descrips meli_descrip[MELI_NUM_DESCRIP_NAMES];

#endif /* MEL_INIT */

#endif /* MEL_INPUT */

#ifdef MEL_OUTPUT

/* the following structure (melo_datum) is used to store information from the
   application program until output as a descriptor using MEL. */

struct {
    int descrip_type;
	/* for the descriptor to be output, this is its index into
	   melo_descrip[] array of structures (see below). in other words,
	   which descriptor is it? */
    int num_param;
	/* number of parameters to be output. */
    struct {   /* data for each parameter to be output */
	int name_index;
	    /* this is the parameter's index into the melo_descrip.param[]
	       array, that is, which parameter is this? (see below) */
	union melo_param_data data;
	    /* data associated with this parameter. */
	char units[MELO_UNITS_STR_LEN+1];
	    /* what units are associated with the data? */
	int array_len;
	    /* if the data was an array, how long was it? */
	int unknown_flag;
	    /* is the value of this parameter "unknown"? */
    } param[MELO_MAX_PARAMS];
} melo_datum;

/* output dictionary manifest constants (all must be CUSTOMIZED):*/

#define MELO_NUM_DESCRIP_NAMES 4
    /* how many descriptors there are: */
#define MELO_MIN_DESCRIP_NAME_LEN 9
    /* how many characters does it take to uniquely define a descriptor name? */
#define MELO_MAX_DESCRIP_NAME_LEN 15
    /* how many characters in the longest descriptor name? */
#define MELO_MIN_PARAM_NAME_LEN 1
    /* how many chars to uniquely define a parameter name? */
#define MELO_MAX_PARAM_NAME_LEN 15
    /* how may characters in the longest parameter name? */
#define MELO_MAX_DESCRIP_STR_LEN 256
    /* how long may the output string be? */

/* define the data structure used to describe MEL output data: */
struct melo_descrips {
    char name[MELO_MAX_DESCRIP_NAME_LEN+1];
    int min_name_len;
    struct {
	char name[MELO_MAX_PARAM_NAME_LEN+1];
	int min_name_len;
	enum mel_data_types type;
    } param[MELO_MAX_PARAMS];
};

#ifdef MEL_INIT

struct melo_descrips melo_descrip[MELO_NUM_DESCRIP_NAMES] = { /* CUSTOMIZED */
    {"program_data", 9,
       {{"program", 1, mel_str},
	{"date", 1, mel_str},
	{"input_filename", 1, mel_str},
	{"output_filename", 1, mel_str},
	{"errors_filename", 1, mel_str},
	{"label", 1, mel_str}}},
    {"program_options", 9,
       {{"output_format", 1, mel_str},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none}}},
    {"message", 1,
       {{"code", 1, mel_int},
	{"text", 1, mel_str},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none}}},
    {"end_of_data", 1,
       {{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none},
	{"", 0, mel_none}}}};

#else

extern struct melo_descrips melo_descrip[MELO_NUM_DESCRIP_NAMES];

#endif /* MEL_INIT */

#endif /* MEL_OUTPUT */

#endif /* MEL_PRIVATE */
