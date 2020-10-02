
/*   FUNC_CA.C   */
struct func *main_loop(FILE *fp, FILE *rptfp, char *module);
static void make_function_token(char *token_ptr, char *function_name);
static void print_userdefs(FILE *rptfp, char *module);
struct func *fill_func_struct(char *token_ptr, int lineno, char *module);
static struct func *syntax(char *p, char * big_buf, char *module,
					 struct func *top,  FILE *rptfp);
static void ini_Globals(void);
int func_def(char *p, char *module, struct func **top,
				   char *big_buf, FILE *rptfp);


/*   LINKL_CA.C   */
void print_ll (struct func *top, FILE *rpt);
struct func *store_linked_list( struct func *i, struct func *top);
struct globals_list *store_ll_globals( struct globals_list *i,
						struct globals_list *top);
void print_calls_from(struct func *func_start, FILE *rptfp);
void print_fnptrs(struct func *func_start, FILE *rptfp);
static void prn_calls__fnptrs(struct func *func_start, int which, FILE *rptfp);


/*   PARSE_CA.C   */
void parse(FILE *fp, FILE *rpt, unsigned end_func, unsigned next_function,
						    unsigned *current_line);
int get_token(void);
int push_fnptr(char *p, char *buffer, char *func_name);
static struct GLoc_fnptr_stack pop_fn_name(char *token);
int iswhite(char c);
int isliteral(char *pgmline);
int isdelim(char c);
int is_in(char c, char *s);
void store_calls_from(struct func *function);
int key_word(char *token);
static void process_calls_from( struct func * fn, FILE *rpt,
					char *token, int current_line);
static int ini_GLstack(void);
void Globfnptrpush(char *p, char *token);
void prn_all__fnptrs(FILE *rptfp);


/*   XRF_CA.C   */
void xrf(struct func *func_start, struct filespec *c_file_start, FILE *rptfp);
void link_by_module(struct func *function);
static int files_match(char *path, char *module);
static FILE *ini_cfile(char *path, FILE *rptfp, unsigned func_line,
							unsigned *count);
int binary_search(char **item, int count, char *key);
static char **create_bsearch_array(struct func *top);
struct func *skip_fnptrs(struct func *func_top);
int infunclist(char *token_string);


/*   GLOB_CA.C   */
struct func * record_variables(FILE *fp, FILE *rptfp,
		    struct func *start, struct func **func_alphatop);
static int xt_struct_union(char *p, int __type);
static int process_vars(char *p, int token_pos, struct func *funcptr,
		    int __type, int line_num, char storage, FILE *rptfp);
static int storage_class(char *p, struct func *func_top, int line_num,
					    int *__type,  FILE *rptfp);
struct globals_list *fill_globals_struct(char *token, struct func *funcptr,
   int mods[_MAX_MODS], int __type, int line_num, char storage, FILE * rptfp);
void print_alphab_globals(struct globals_list *top, FILE *rptfp);
static size_t isolate_strun(char *ptr, int __type);
int is_nextstatement(char **p);
int is_includeF(char *p, FILE * hold_fp);
static int sign_or_ld(char *p , int t_pos, int *__type);
void print_modifiers(int alphab, int mods[_MAX_MODS], int __type, FILE *rptfp);
int vol_const(char *p, struct func *func_top, int line_num,
					int *__type, FILE *rptfp);
static int make_complex_decl(char *p);
static char *is_comment(char *p);
static char * is_closed_comment(char * p);
static char *comment_error(struct func *func_top, unsigned lineno, char *p);
static void glob_ca_msgs(int Line, char * Mod, struct func *func_top );


/*   TDEF_CA.C   */
int get_typedef(char *test);
int tdeftest(char *test, int tdef_open);
int check_types(char *token_ptr, int *typenum);
int intypeslist(char *token);
int get_defines(char *test);
int insert_type(char *token);


/*   INTF_CA.C   */
void main(int argc, char **argv);
struct filespec *get_modules(void);
struct filespec  *extract_file_names(char *file_string);
void instructions (void);
static int reallocate(char *file_string, size_t strL);
void loop_through_modules (struct filespec *top, struct func **func_start,
								FILE *rptfp);
int paginate(FILE *rptfp);
void formfeed(int line_count, FILE *rptfp);
static char *argv_cat(int argc, char **argv);
FILE *make_rptfile(char *path);
FILE *open_c_file(char *filepath, FILE *rptfp);
void get_new_path(struct filespec *top);
static void crt_grc(int *row, int *col);
void relocate(void);
void handle_switches(char ***cmdline_switch, int *argc);
char *parse_switch(char ***cmdline_switch, int *argc, char *p);
char *readmakefile(char *file_string);
char *makepathvar(char *inpath, char *pathvar);
static FILE * open_reportfile(int rpt_number);


/*   UTIL_CA.C   */
char * get_envpath(int service, char *env_var, char *fname);
FILE *setincludefile(char *includebuf);
FILE *open_includef(char *filespec);
FILE *get_redfp(void);
int is_include(char *token_ptr, FILE *holdfp);
int close_include(FILE *hold_fp, FILE **fp, FILE *rptfp, char *module);
void date_time(FILE *rptfp );
void heading(FILE *rptfp, char *fname);
int countbrackets(char *buffer);
void print_headings(int head_no, FILE *rptfp );
char * isolate_token(char * p, char **end_token);
int test_token( char *token, char *type);


/*   UTLG_CA.C   */
int is_prototype(char *p);
int get_dimensions(char *ptr, int count);
char * white_out(char *token);
int is_variable(char *token);
int check_pointers(char *p, int *ptr_count);
char *get_modifiers(int final_check, char *ptr, int mods[_MAX_MODS]);
int push_usertype(char * user_type);
char *pop_usertype(int type_number);
int is_complex_decl(char *p);
void ini_characterset(void);
int complex_value(char *p);
int is_identifier(char **p);


/*   IFDEF_CA.C   */
int is_else_if(char *p, int in_define);
static int def_value(char *p, int if_type);
static IF_DEF get_definition(char *p, int if_type);
static int ifdef_true(IF_DEF ifdefs);
static void push_ifdef(char *p, unsigned int status);
static int pop_ifdef_status(char *p);
void ini_if_def_stack(void);
void free_if_def_stack(void);
int skipifdefn(char *p, int skipdef_status);


/*   EXIT_CA.C   */
void exit_ca(int exit_code, char *exit_str);
static void prn_exitmsg(FILE *output, char const *function, char const *module,
			char const *xit_msg, char *exit_str, int exit_code);
void nullptrxit(char *msg, char *filename, unsigned lineno);


/*   TYPES_CA.C   */
int data_type_ (char *test, int *__type);
int _modifier (char *test, int t_pos);

