/*	This is the standard include file for the edit function */
#define YES 1
#define NO 0
#define NONE 0
#define NOPE -2
#define MAX_FILES 4
#define BOOL int
#define FLAG int
#define DEF_PROMPT YES
#define DEFAULT_PRINT_LINE_NUMBER YES
#define MAX_LINE 132
#define SIZE_LINE 60
#define ERROR -1
#define EOL '\0'
#define DELETE '\177'
#define LF '\n'
#define CR '\015'
#define COMMA ','
#define TAB '\011'
#define BS '\010'
#define QUOTE 0x22
#define CNTRL_Z '\032'
#define CNTRL_X '\030'
#define CNTRL_R '\022'
#define CNTRL_P '\020'
#define SPACE ' '
#define ESCAPE '\033'
#define NOT_SYMBOL '~'
#define MANY_SYMBOL '*'
#define BEGIN_SYMBOL '^'
#define END_SYMBOL '$'
#define ANY_SYMBOL '.'
#define REPLACE_SYMBOL '&'
#define FORCE_SYMBOL '\\'
#define REP_SYM '\200'
#define XOR ^
#define AND &&
#define OR ||
#define NOT !
#define SYNTAX_ERROR {goto oops;}
/*	In any reasonable C, I could say {int i;for (i=0;i<78;i++)
	printf("-");printf("\n");}, but not this C */
#define HIGH_LIGHT printf("------------------------------------------------------------------------------\n");
#define STOP_HIGH_LIGHT HIGH_LIGHT
#define BUFF_SIZE 512
#define CPM_SIZE 128
#define READ_SIZE (BUFF_SIZE/CPM_SIZE)
#define MOVE_FILE "edit2.tmp"
#define NO_PARSELS 400	/* these point to sets of lines  */
#define NO_MEMORIES 37	/* these are pointed at and hold lines */
#define EOF 3276
#define IN_MEM 5373
#define IN_TEMP_FILE -4836
#define TEMP_FILE "edit.tmp"
#define NO_SEARCH 30

int number1, number2, number3, dot, dollar;
BOOL modified_file;
char deflt_file[MAX_LINE];
int num_num, parse;
int push_point;
struct {char random[200];}
	iobuf[MAX_FILES];
BOOL user_command;
BOOL global, found_line, search_true;
BOOL print_line_number, prompt_enabled;
BOOL g_mode, p_mode;
char line[MAX_LINE+2];
char user_string[MAX_LINE];
char temp_buff[2*BUFF_SIZE];
#define temp2_buff (temp_buff+BUFF_SIZE)
char  *t;
BOOL stuffed_file;
int fd, u, filed;
char temp3_buff[BUFF_SIZE+1], temp4_buff[CPM_SIZE];
BOOL temp_used;
int split, line_split, split_at;
int used_search;
int *last_block;
char deflt_line[MAX_LINE+1];
BOOL start_begin, end_end;

struct {
	char test_char;
	int epsilon, if_found, if_not;
	} data_search[NO_SEARCH];
struct george {
	FLAG type;
	int where;
	int line_no;
	} parsels[NO_PARSELS];

struct kathy {
	BOOL used;
	int parsel_no;
	int lru;
	char buffer[BUFF_SIZE+1];
	} memory[NO_MEMORIES];

char tline[MAX_LINE];
int printer_echo;

	int parsel_no;
	int lru;
	cha