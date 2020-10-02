/*
	HEADER:		CUG236;
	TITLE:		Cross Reference Generator Header File;
	DATE:		04/27/1988;
	VERSION:	1.1;
	FILENAME:	XC2.H;
	AUTHORS:	Fred C. Smith, William C. Colley III;
*/

/*global*/  int main(int,char **);
/*global*/  void lst_err(void);
/*global*/  void use_err(void);
/*global*/  int proc_file(FILE *,int);
/*global*/  FILE *get_include_file(FILE *);
/*global*/  FILE *open_include_file(char *,char);
/*global*/  void echo(char);
/*global*/  void echochar(char);
/*global*/  int get_token(FILE *,char *,int *,int *,int);
/*global*/  int fil_chr(FILE *,int *);
/*global*/  int rdchr(FILE *,int *,int);
/*global*/  int chk_token(char *);
/*global*/  void put_token(char *,int);
/*global*/  void chain_alpha(struct id_blk *,char *);
/*global*/  struct id_blk *alloc_id(char *);
/*global*/  struct rf_blk *alloc_rf(int);
/*global*/  struct rf_blk *add_rf(struct rf_blk *,int);
/*global*/  void prnt_tbl(void);
/*global*/  void prt_hdr(void);
/*global*/  void nl(void);
/*global*/  int abs(int);
