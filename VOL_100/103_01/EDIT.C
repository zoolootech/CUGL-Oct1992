
/*	Hello!  This program is supposed to be an editor...  in fact, the
	editor just like mom used to make (oops...) I mean, just like the
	QED editor found most anywhere, especially on UNIX
*/

#include "edit.inc"

main(argc,argv)
	int argc;
	char **argv;
	{
	char *exp_string;
	int i;
	init_buffer();
	prompt_enabled = DEF_PROMPT;
	sprintf(deflt_file,"DEFAULT.$$$");
	print_line_number = DEFAULT_PRINT_LINE_NUMBER;
	exp_string = "+-0123456789.$/?";
	modified_file = NO;
	printer_echo = NO;
	push_point = 0;
	user_command = NO;
	user_string[0] = EOL;
	printf("version 12.9999b by scott fluhrer and neal somos");
	putchar('\n');	/* to force local version in */
	if (argc>1)
		if (argc==2)
			{
			read_file(argv[1]);
			}
		else
			syntax_error();
	for (;;)
		{
		if (user_command)
			{
			for (i=0; line[i] = user_string[i]; i++)
				;
			user_command = NO;
			}
		else
			{
			tline[0]='\0';
			if (prompt_enabled)
				{
				if (print_line_number)
					sprintf(tline,"%d>",dot);
				else	sprintf(tline,">");
				}
			get_line(line,tline);
			}
		parse = 0;
		skip_space(&parse, line);
		if (char_in_string(line[parse], exp_string))
			{
			number1 = eval_exp(&parse, line);
			if (number1 == ERROR)
				SYNTAX_ERROR;
			skip_space(&parse, line);
			if (line[parse] == COMMA)
				{
				parse++;
				number2 = eval_exp(&parse, line);
				if (number2 == ERROR)
					SYNTAX_ERROR;
				num_num = 2;
				}
			else
				num_num = 1;
			}
		else
			num_num = 0;
		skip_space(&parse, line);
		global = NO;
		found_line = NO;
		if (do_command())
			continue;
oops:		syntax_error();
		}
	}

/*	This is the general command do-er, returning YES if everything
	went Hunky-Dory
*/
BOOL do_command()
	{
	int l, start_search, i;
	BOOL interact, forced, delete_after_store, print_control;
	char file[MAX_LINE], replace[MAX_LINE+2], *h;
	char *w;
	char string1[MAX_LINE+1], string2[MAX_LINE+1];
	switch( tolower( line[parse] ))
		{
		case 'p':
			print_control = NO;
random_label:		if (check_2_param())
				return(NO);
			while(l=get_next_line())
				{
				print(l, print_control);
				dot = l;
				}
			break;
		case 'l':
			print_control = YES;
			goto random_label;
		case EOL:
			if (check_blank())
				return(NO);
			while (l=get_next_line())
				{
				print(l, NO);
				dot = l;
				}
			break;
		case 'i':
			if (check_1_param())
				return(NO);
			if (number1 == 0)
				return(NO);
			insert(number1);
			break;
		case 'a':
			if (tolower( line[parse+1] ) == 'c')
				{
				parse++;
				if (check_2_param())
					return(NO);
				while (l=get_next_line())
					append_comment(l);
				break;
				}
			if (check_1_param())
				return(NO);
			if (number1 > dollar)
				return(NO);
			insert(number1+1);
			break;
		case 'd':
			if (check_2_param())
				return(NO);
			while(l=get_next_line())
				delete(l);
			break;
		case 'c':
			if (check_2_param())
				return(NO);
			if (global)
				return(NO);
			while (l=get_next_line())
				delete(l);
			insert(number1);
			break;
		case 'm':
			delete_after_store = YES;
move_label:		if (check_3_param())
				return(NO);
			stuffed_file = NO;
			t = temp_buff;
			while (l=get_next_line())
				{
				read_line(l, replace);
				if (delete_after_store)
					delete_line(l);
				for (w=replace; *w; w++)
					if (save_in_buff(*w))
						return (NO);
				if (save_in_buff(EOL))
					return (NO);
				}
			if (stuffed_file)
				seek(fd, 0, 0);
			u = 0;
			while (recall_from_buff(replace))
				{
				insert_line(number3, replace);
				}
			break;
		case 't':
			delete_after_store = NO;
			goto move_label;
		case 'h':
			if (check_0_param())
				return(NO);
			if ((fd=open("edit.hlp", 0)) == ERROR)
				return(NO);
			read(fd, temp_buff, 8);
			close(fd);
			for (h=&temp_buff; *h!=CNTRL_Z; h++)
				printf("%c",*h);
			break;
		case 'u':
			if (line[parse+1] == '=')
				{
				if (check_user())
					return(NO);
				parse += 2;
				if (line[parse] == EOL)
					{
					printf("%s\n", user_string);
					break;
					}
				for (i=0; line[parse];)
					user_string[i++] = line[parse++];
				user_string[i] = EOL;
				break;
				}
			if (check_0_param())
				return(NO);
			user_command = YES;
			break;
		case 'j':
			if (check_1_param())
				return(NO);
			if (number1==0 OR number1==dollar)
				return(NO);
			read_line(number1, string1);
			read_line(number1+1, string2);
			if (strlen(string1) + strlen(string2) > MAX_LINE)
				{
				printf("Line too long\007\n");
				return(YES);
				}
			strcat(string1, string2);
			delete_line(number1+1);
			delete_line(number1);
			insert_line(number1+1, string1);
			dot = number1+1;
			break;
		case 'n':
			if (check_file(file))
				return(NO);
			if (push_point == MAX_FILES)
				return(NO);
			if (fopen(file, &iobuf[push_point]) == ERROR)
				return(NO);
			push_point++;
			break;
		case 'r':
			if (check_file(file))
				return(NO);
			read_file(file);
			break;
		case 'w':
			if (check_file(file))
				return(NO);
			write_file(file);
			break;
		case 'f':
			if (line[parse+1] == '=')
				{
				parse++;
				if (check_file(file))
					return(NO);
				strcpy(deflt_file, file);
				break;
				}
			if (strcmp(deflt_file, "") == 0)
				printf("\007??\n");
			else
				printf("%s\n", deflt_file);
			break;
		case '#':
			if (check_0_param())
				return(NO);
			print_line_number ^= YES;
			break;
		case '>':
			if (check_0_param())
				return(NO);
			prompt_enabled = NO;
			break;
		case '<':
			if (check_0_param())
				return(NO);
			prompt_enabled = YES;
			break;
		case 'q':
			if (line[parse+1] == '!')
				{
				parse++;
				forced = YES;
				}
			else
				forced = NO;
			if (check_0_param())
				return(NO);
			if (modified_file AND NOT forced)
				{
				printf("Write??\007\n");
				modified_file = NO;
				break;
				}
			else
				{
				if (temp_used)
					unlink(TEMP_FILE);
				exit(YES);
				}
		case 's':
			interact = NO;
search_label:		if (check_search())
				return(NO);
			if (NOT global)
				init_search();
			start_search = place_search(&parse, line);
			if (start_search == ERROR)
				return(NO);
			if (NOT get_replace(&parse, line, replace))
				return(NO);
			g_mode = p_mode = NO;
			if (NOT get_options())
				return(NO);
			if (interact)
				{
				if (g_mode)
					return(NO);
				g_mode = YES;
				}
			while (l=get_next_line())
				substitute(l, start_search, replace, interact);
			break;
		case 'x':
			interact = YES;
			goto search_label;
		case 'g':
			search_true = YES;
global_search:		if (check_global())
				return(NO);
			global = YES;
			init_search();
			if (place_search(&parse, line) == ERROR)
				return(NO);
			return(do_command());
		case 'v':
			search_true = NO;
			goto global_search;
		case '=':
			if (check_1_param())
				return(NO);
			printf("%d\n", number1);
			break;
		case '%':
			if (check_1_param())
				return(NO);
			if (dollar == 0 OR number1 == 0 OR number1>dollar)
				return(NO);
			dot = number1;
			for (i=max(1, dot-8); i<dot; i++)
				print(i, NO);
			HIGH_LIGHT;
			print(dot, NO);
			STOP_HIGH_LIGHT;
			for (i=dot+1;i<=min(dot+8, dollar); i++)
				print(i, NO);
			break;
		case ':':
			if (check_1_param())
				return(NO);
			if (number1 > dollar OR number1 == 0)
				return(NO);
			dot = number1;
			for (i=dot; i<=min(dot+16, dollar); i++)
				print(i, NO);
			break;
		case '^':
			if (check_1_param())
				return(NO);
			if (dot == 0)
				return(NO);
			for (;;)
				{
				if (kbhit())
					{
					getchar();
					printf("!\007	%d\n", dot);
					break;
					}
				print(dot, NO);
				if (dot == dollar)
					break;
				dot++;
				}
			break;
		case '&':
			if (check_1_param())
				return(NO);
			if (number1 > dollar)
				return(NO);
			dot = number1;
			for (i=max(dot-16, 1); i<=dot; i++)
				print(i, NO);
			break;
		case QUOTE:
			if (check_1_param())
				return(NO);
			if (number1>dollar)
				return(NO);
			for (i=min(number1+1, dollar); i<=min(number1+17,
				dollar); i++)
				{
				dot = i;
				print(i, NO);
				}
			break;
		case '~':
			if (check_1_param())
				return(NO);
			if (number1>dollar)
				return(NO);
			for (i = dot = max(1, number1-17); i<=number1; i++)
				print(i, NO);
			break;
		case '!':
			for (i=0; parsels[i].type!=EOF; i++)
				{
				printf("%d", i);
				if (parsels[i].type == IN_MEM)
					printf("-in memory   ");
				else if (parsels[i].type == IN_TEMP_FILE)
					printf("-in temp file");
				else
					printf("-Illegal type");
				printf(" at %d", parsels[i].where);
				printf(" line number %d\n", parsels[i].line_no);
				}
			for (i=0; i<NO_MEMORIES; i++)
				if (memory[i].used)
					{
					printf("%d-being used by parsel ", i);
					printf("%d-lru = %d\n", memory[i].parsel_no, memory[i].lru);
					}
				else
					printf("%d-Not being used\n", i);
			break;
		case '@':
			printf("Kathy, don't do that, it tickles\n");
			break;
		case '*':
			printf("Now Kathy, stop fooling around\n");
			break;
		default:
			return(NO);
		}
	return(YES);
	}

int max(a,b)
	int a,b;
	{
	return(a>b?a:b);
	}

int min(a,b)
	int a,b;
	{
	return(a<b?a:b);
	}

/*	This is the character storer */
BOOL save_in_buff(n)
	char n;
	{
	*t++ = n;
	if (t == (temp_buff+BUFF_SIZE))
		{
		if (NOT stuffed_file)
			{
			fd = creat(MOVE_FILE);
			if (fd == ERROR)
				return(YES);
			close(fd);
			fd = open(MOVE_FILE, 2);
			if (fd == ERROR)
				return(YES);
			}
		stuffed_file++;
		if (write(fd, temp_buff, BUFF_SIZE/CPM_SIZE) != BUFF_SIZE/CPM_SIZE)
			{
			close(fd);
			unlink(MOVE_FILE);
			return(YES);
			}
		t = temp_buff;
		}
	return(NO);
	}

/*	This is the line recaller */
BOOL recall_from_buff(line)
	char *line;
	{
	if (stuffed_file==0 AND u+temp_buff==t)
		return(NO);
	while (*line++ = recall_char())
		;
	return(YES);
	}

char recall_char()
	{
	char c;
	char *testptr;
	if (u==0 AND stuffed_file)
		read(fd, temp2_buff, BUFF_SIZE/CPM_SIZE);
	c = (stuffed_file?temp2_buff:temp_buff)[u++];
	if (u == BUFF_SIZE)
		{
		stuffed_file--;
		u = 0;
		if (stuffed_file == 0)
			{
			close(fd);
			unlink(MOVE_FILE);
			}
		}
	return(c);
	}

/*	This is the great and wonderfull (well, almost wonderful) line input
	routine.  It will input a line anywhere (well, anywhere you give
	a pointer to) and it will enter up to MAX_LINE characters, beeping
	incessently if the user tries to enter more.  It also handles delete
	in a half-way intelligent manner.
*/
get_line(line,from)
	char *from;
	char *line;
	{
	int position;
	int loc;
	char character;
	position = 0;
	loc=0;
	if (*from!=0) printf("%s",from);
	for (;;)
		{
		if (push_point == 0)
			{
			character = getchar();
			if (printer_echo) bdos(5,character);
			}
		else
			{
			character = ubgetc(&iobuf[push_point-1]);
			if (character != CNTRL_Z)
				printf("%c", character);
			}
		switch(character)
			{
			case CNTRL_Z:
				if (push_point == 0)
					{
					printf("\007");
					break;
					}
				fclose(&iobuf[--push_point]);
				break;
			case LF:
				line[position] = EOL;
				return;
			case DELETE:
			case BS:
				if (position == 0)
					{
					printf("\007");
					break;
					}
				position--;
				if (line[position] == TAB)
					{
					line[position] = EOL;
					printline(line,from);
					}
				else
					{
					printf("\010 \010");
			printf(" \010 \010 \010 \010 \010 \010");
					line[position] = EOL;
					}
				break;
			case CNTRL_X:
				position = 0;
				printf("\\\n");
				printline("", from);
				break;
			case CNTRL_P:
				printer_echo=!printer_echo;
				break;
			case CNTRL_R:
				line[position] = EOL;
				printline(line,from);
				break;
			case TAB:
				goto skip_check;
			default:
				if (character<SPACE OR character>DELETE)
					{
					printf("\007");
					break;
					}
skip_check:			if (position == MAX_LINE)
					{
					printf("\007");
					if (character == TAB)
						{
						line[position] = EOL;
						printline(line,from);
						}
					else
						printf("\010 \010");
					break;
					}
				line[position++] = character;
			}
		}
	}

/* print_line */
printline(line,from)
char *from;
char *line;
{
printf("\n");
if (*from!=0) printf("%s",from);
if (*line!=0) printf("%s",line);
}

/*	This is the main syntax error complainer */
syntax_error()
	{
	printf("?\007\n");
	}

/*	This function will check if the character given lines in the string */
BOOL char_in_string(character, string)
	char character, *string;
	{
	while (*string)
		if (*string++ == character)
			return(YES);
	return(NO);
	}

/*	This function will skip any white space (spaces or tabs) in the line
	given to it */
skip_space(pointer, line)
	int *pointer;
	char line[];
	{
	while (line[*pointer] == SPACE OR line[*pointer] == TAB)
		++*pointer;
	}

/*	This is the wonderful, marvulous(sick(sic)), amazing expression
	evaluator.  An expression is something that specifies a line nuMBER,
	2, 35, +, $, .+3, /abc/, ?x.y?-2, /^*.$/, you get the picture.
*/
int eval_exp(count, line)
	int *count;
	char line[];
#define NOP 0
#define ADD 1
#define SUB 2
#define INC 3
#define DEC 4
	{
	BOOL operator_expected;
	FLAG oper_seen;
	int defalut, number;
	operator_expected = NO;
	oper_seen = NOP;
	defalut = dot;
	for (;;)
		{
		skip_space(count, line);
		switch (line[*count])
			{
			case '0': case '1': case '2':
			case '3': case '4': case '5':
			case '6': case '7': case '8':
			case '9':
				number = 0;
				while (isdigit(line[*count]))
					number = 10*number + (line[(*count)++] - '0');
got_number:			if (operator_expected)
					return(ERROR);
				if (oper_seen == NOP)
					defalut = number;
				else if (oper_seen == ADD OR oper_seen == INC)
					defalut += number;
				else
					defalut -= number;
				operator_expected = YES;
				break;
			case '$':
				number = dollar;
				++*count;
				goto got_number;
			case '.':
				number = dot;
				++*count;
				goto got_number;
			case '/':
				init_search();
				if (place_search(count, line) == ERROR)
					return(NO);
				number = find_line(dot+1, YES);
check_search:			if (number == 0)
					{
					printf("?");
					return(NO);
					}
				goto got_number;
			case '?':
				init_search();
				if (place_search(count, line) == ERROR)
					return(NO);
				number = find_line(dot-1, NO);
				goto check_search;
			case '+':
				++*count;
				if (operator_expected)
					{
					oper_seen = ADD;
					operator_expected = NO;
					break;
					}
				if (oper_seen == INC)
					{
					defalut++;
					break;
					}
				if (oper_seen == NOP)
					{
					oper_seen = INC;
					break;
					}
				return(ERROR);
			case '-':
				++*count;
				if (operator_expected)
					{
					oper_seen = SUB;
					operator_expected = NO;
					break;
					}
				if (oper_seen == DEC)
					{
					defalut--;
					break;
					}
				if (oper_seen == NOP)
					{
					oper_seen = DEC;
					break;
					}
				return(ERROR);
			default:
				if (operator_expected)
					return(defalut);
				if (oper_seen == INC)
					return(defalut+1);
				if (oper_seen == DEC)
					return(defalut-1);
				return(ERROR);
			}
		}
	}

/*	This procedure will check if a command with up to 2 parameters is
	expressed properly.  If not, it will return YES.  If it is, this will
	alter number1 and number2 to deflt values when there are less than
	2 values.
*/
BOOL check_2_param()
	{
	parse++;
	skip_space(&parse, line);
	if (line[parse] != EOL)
		return(YES);
	if (global)
		return(NO);
	if (num_num == 0)
		number1 = dot;
	if (num_num < 2)
		number2 = number1;
	if (number2>dollar OR number1<1 OR number2<number1)
		return(YES);
	return(NO);
	}

BOOL check_3_param()
	{
	parse++;
	number3 = eval_exp(&parse, line);
	if (number3 == ERROR)
		return(YES);
	if (number3<1 OR number3>dollar+1)
		return(YES);
	skip_space(&parse, line);
	if (line[parse] != EOL)
		return(YES);
	if (global)
		return(NO);
	if (num_num == 0)
		number1 = dot;
	if (num_num < 2)
		number2 = number1;
	if (number2>dollar+1 OR number1<1 OR number2<number1)
		return(YES);
	return(NO);
	}
BOOL check_user()
	{
	if (global)
		return(YES);
	if (num_num != 0)
		return(YES);
	return(NO);
	}
BOOL check_search()
	{
	parse++;
	if (global)
		return(NO);
	if (num_num == 0)
		number1 = dot;
	if (num_num < 2)
		number2 = number1;
	if (number2>dollar OR number1<1 OR number2<number1)
		return(YES);
	return(NO);
	}

/*	This will do about the same thing, but for the null command */
BOOL check_blank()
	{
	if (global)
		return(NO);
	if (num_num == 0)
		number1 = dot + 1;
	if (num_num < 2)
		number2 = number1;
	if (number2>dollar OR number1<1 OR number2<number1)
		return(YES);
	return(NO);
	}

/*	This will do just about the same thing, but with only 1 parameter
*/
BOOL check_1_param()
	{
	parse++;
	skip_space(&parse, line);
	if (line[parse] != EOL)
		return(YES);
	if (global)
		return(YES);
	if (num_num == 0)
		number1 = dot;
	if (num_num == 2)
		return(YES);
	if (number1<0 OR number1>dollar+1)
		return(YES);
	return(NO);
	}

/*	This will do the same thing, but without any parameters */
BOOL check_0_param()
	{
	parse++;
	skip_space(&parse, line);
	if (line[parse] != EOL)
		return(YES);
	if (global)
		return(YES);
	if (num_num != 0)
		return(YES);
	return(NO);
	}

/*	This will do the same thing, but with a file name at the end */
BOOL check_file(file_name)
	char file_name[];
	{
	parse++;
	if (global)
		return(YES);
	if (num_num != 0)
		return(YES);
	skip_space(&parse, line);
	if (line[parse] == EOL)
		if (deflt_file[0] == EOL)
			return(YES);
		else
			strcpy(file_name, deflt_file);
	else
		strcpy(file_name, line + parse);
	return(NO);
	}

/*	This will check a global command */
check_global()
	{
	parse++;
	if (global)
		return(YES);
	if (num_num == 0)
		{
		number1 = 1;
		number2 = dollar;
		if (dollar < 1)
			return(YES);
		}
	if (num_num == 1)
		number2 = number1;
	if (number1 < 1 OR number2 > dollar OR number1 > number2)
		return(YES);
	return(NO);
	}

/*	This will get the next line (if any) */
int get_next_line()
	{
	while (global AND number1<=number2 AND NOT kbhit() AND
		(search_true XOR test_line(number1)))
		number1++;
	if (kbhit())
		{
		getchar();
		printf("!\007  %d\n",number1);
		return(NO);
		}
	if (number1 > number2)
		{
		if (NOT found_line)
			printf("??\007\n");
		return(NO);
		}
	found_line = YES;
	return(number1++);
	}

/*	This procedure will print a whole bunch of lines */
print(this, print_control)
	int this;
	BOOL print_control;
	{
	char line[MAX_LINE+2];
	int i;
	read_line(this, line);
	if (print_line_number)
		printf("%d\t", this);
	if (print_control)
		{
		for (i=0; line[i]; i++)
			if (line[i]<32)
				printf("^%c", line[i]+'@');
			else if (line[i]>127)
				printf("@%c", line[i] & 0x7f);
			else printf("%c", line[i]);
		printf("\n");
		}
	else
		printf("%s\n", line);
	}

/*	This procedure will delete a whole bunch of lines */
delete(this)
	int this;
	{
	delete_line(this);
	if (dollar<this)
		dot = dollar;
	else
		dot = this;
	}

/*	This procedure will insert a whole bunch of lines */
insert(from)
	int from;
	{
	char line[MAX_LINE+2];
	for (;;)
		{
		if (print_line_number)
			sprintf(tline,"%-8.8d",from);
		else	tline[0]='\0';
		get_line(line,tline);
		if (strcmp(line, ".") == 0)
			return;
		dot = from;
		insert_line(from++, line);
		}
	}

/*	This will do the appending of comments */
append_comment(l)
	int l;
	{
	char line[MAX_LINE+1], comments[MAX_LINE+1];
	read_line(l, line);
	sprintf(tline,"%s\t/\* ", line);
	get_line(comments,tline);
	if (NOT *comments)
		return;
	if (strlen(line)+strlen(comments)+7 > MAX_LINE)
		{
		printf("Line too long\007\n");
		return;
		}
	strcat(line, "\t/\* ");
	strcat(line, comments);
	strcat(line, " */");
	delete_line(l);
	insert_line(l, line);
	}
/* local version of putchar to not interrogate console */
putchar(c)
char c;
{
putch(c);
if (printer_echo)
	bdos(5,c);
}
/* thats it */
f putchar to not interrogate console */
putchar(c)
char c;
{
putch(c);