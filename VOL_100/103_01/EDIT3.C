/*	This is the editor that everyone (including Ma) loves -> QED
	This is the searching section.
*/

/* 	find line fixed for abort 4/21/81	ns	*/
#include "edit.inc"

init_search()
	{
	used_search = 0;
	}

int place_search(count, line)
	int *count;
	char line[];
	{
	int first, dummy, deflt_count, nxet;
	BOOL at_begin, multiple, negate, force, use_first, use_second;
	char c, delim;
	allocate(&first);
	last_block = &data_search[first].epsilon;
	delim = line[(*count)++];
	if (delim == line[*count])
		{
		++*count;
		line = deflt_line;
		count = &dummy;
		*count = 0;
		delim = EOL;
		}
	deflt_count = 0;
	at_begin = YES;
	multiple = negate = force = start_begin = end_end = NO;
	for (;;)
		{
		c = deflt_line[deflt_count++] = line[(*count)++];
		if (c==delim AND NOT force)
			if (multiple OR negate OR at_begin)
				return(ERROR);
			else
				{
				link(0, NO_SEARCH);
				deflt_line[deflt_count-1] = EOL;
				return(first);
				}
		if (c==EOL)
			return(ERROR);
		if (c==NOT_SYMBOL AND NOT force)
			if (negate)
				return(ERROR);
			else
				negate = YES;
		else if (c==MANY_SYMBOL AND NOT force)
			if (multiple)
				return(ERROR);
			else
				multiple = YES;
		else if (c==FORCE_SYMBOL AND NOT force)
			force = YES;
		else if (c==BEGIN_SYMBOL AND NOT force AND at_begin)
			if (multiple OR negate)
				return(ERROR);
			else
				{
				start_begin = YES;
				at_begin = NO;
				}
		else if (c==END_SYMBOL AND NOT force AND line[*count] == delim)
			if (multiple OR negate)
				return(ERROR);
			else
				{
				at_begin = NO;
				end_end = YES;
				}
		else
			{
			if (allocate(&nxet))
				return(ERROR);
			data_search[nxet].test_char = c;
			if (c==ANY_SYMBOL AND NOT force)
				{
				if (negate)
					return(ERROR);
				use_first = use_second = YES;
				data_search[nxet].if_found =
					&data_search[nxet].if_not;
				}
			else
				if (negate)
					{
					use_first = NO;
					use_second = YES;
					}
				else
					{
					use_first = YES;
					use_second = NO;
					}
			if (multiple)
				{
				if (use_first)
					data_search[nxet].if_found = nxet;
				if (use_second)
					data_search[nxet].if_not = nxet;
				link(&data_search[nxet].epsilon, nxet);
				}
			else
				{
				if (use_first)
					link(&data_search[nxet].if_found, nxet);
				else
					link(&data_search[nxet].if_not, nxet);
				}
			multiple = negate = at_begin = force = NO;
			}
		}
	}

BOOL allocate(put)
	int *put;
	{
	if (used_search == NO_SEARCH)
		return(YES);
	data_search[used_search].epsilon = data_search[used_search].if_found =
		data_search[used_search].if_not = NONE;
	*put = used_search++;
	return(NO);
	}

link(point, block)
	int point, block;
	{
	int t;
	while (last_block != NONE)
		{
		t = *last_block;
		*last_block = block;
		last_block = t;
		}
	last_block = point;
	}

INT find_line(start, up)
	int start;
	BOOL up;
	{
	int stop_at, dummy1, dummy2;
	fix_circular(&start);
	stop_at = start;
	if (dollar == 0)
		return(NO);
	do
		{
		if (kbhit())
			{
			getchar();
			printf("!\007  ");
			return(start);
			}
		if (test_line(start))
			return(start);
		if (up)
			start++;
		else
			start--;
		fix_circular(&start);
		}
	while (stop_at != start);
	return(NO);
	}

BOOL test_line(number)
	int number;
	{
	char line[MAX_LINE+1];
	int dummy1, dummy2;
	read_line(number, line);
	return(check_line(line, &dummy1, &dummy2, 0));
	}

fix_circular(line)
	int *line;
	{
	if (*line == 0)
		*line = dollar;
	if (*line > dollar)
		*line = 1;
	}

BOOL check_line(line, from, to, start)
	char line[];
	int *from, *to, start;
	{
	int j,i,t,set;
	BOOL found[NO_SEARCH], next_found[NO_SEARCH];
	*from = 0;
	for (;;)
		{
		*to = NOPE;
		for (j=0; j<used_search; j++)
			found[j] = NO;
		found[start] = YES;
		i = *from - 1;
		for (;;)
			{
			for (j=0; j<used_search; j++)
				if (found[j] AND (t=data_search[j].epsilon))
					if (t==NO_SEARCH)
						*to = i;
					else
						found[t] = YES;
			if (line[i+1] == EOL)
				break;
			for (j=0; j<used_search; j++)
				next_found[j] = NO;
			i++;
			for (j=0; j<used_search; j++)
				if (found[j])
					{
					if (data_search[j].test_char == line[i])
						set = data_search[j].if_found;
					else
						set = data_search[j].if_not;
					if (set == NO_SEARCH)
						*to = i;
					else if (set)
						next_found[set] = YES;
					}
			for (j=0; j<used_search; j++)
				found[j] = next_found[j];
			for (j=0; j<used_search; j++)
				if (found[j])
					goto lets_go_on;
			break;
lets_go_on:;		}
		if (*to != NOPE)
			if (NOT end_end OR line[*to+1]==EOL)
				return(YES);
		if (start_begin OR line[*from]==EOL)
			return(NO);
		++*from;
		}
	}

BOOL get_replace(count, line, replace)
	int *count;
	char line[], replace[];
	{
	int c;
	char delim, k;
	delim = line[*count-1];
	for (c=0; delim != (k=line[(*count)++]); c++)
		{
		if (k==REPLACE_SYMBOL)
			k = REP_SYM;
		if (k==FORCE_SYMBOL)
			k = line[(*count)++];
		if (k == EOL)
			return(NO);
		replace[c] = k;
		}
	replace[c] = EOL;
	return(YES);
	}

BOOL get_options()
	{
	for (;;)
		{
		skip_space(&parse, line);
		switch(tolower(line[parse++]))
			{
			case 'p':
				if (p_mode)
					return(NO);
				p_mode = YES;
				break;
			case 'g':
				if (g_mode)
					return(NO);
				g_mode = YES;
				break;
			case EOL:
				return(YES);
			default:
				return(NO);
			}
		}
	}

substitute(l, start_search, replace, interact)
	int l, start_search;
	char replace[];
	BOOL interact;
	{
	int from, to, new_to;
	char line[MAX_LINE+1], new_line[MAX_LINE+1];
	read_line(l, line);
	if (NOT check_line(line, &from, &to, start_search))
		return;
	if (NOT dosub(line, &from, &to, replace, interact))
		return;
	while (g_mode AND NOT start_begin AND strlen(line)>to AND
		check_line(line+(new_to=to), &from, &to, start_search))
		{
		from += new_to;
		to += new_to;
		if (NOT dosub(line, &from, &to, replace, interact))
			return;
		}
	delete_line(l);
	insert_line(l, line);
	if (p_mode)
		print(l, NO);
	dot = l;
	}

#define OVER_FLOW {printf("Overflow error!\007\n");return(NO);}
BOOL dosub(line, from, to, replace, interact)
	char line[], replace[];
	int *from, *to;
	BOOL interact;
	{
	char new_line[MAX_LINE+1], replace_string[MAX_LINE+1];
	int cnt, i, next_to, j;
	cnt = 0;
	for (i=0; replace[i]; i++)
		if (replace[i] == REP_SYM)
			for (j=*from; j<=*to; j++)
				{
				if (cnt >= MAX_LINE)
					OVER_FLOW;
				replace_string[cnt++] = line[j];
				}
		else
			{
			if (cnt >= MAX_LINE)
				OVER_FLOW;
			replace_string[cnt++] = replace[i];
			}
	replace_string[cnt] = EOL;
	if (interact)
		{
		printf("%s\n", line);
		for (i=0; i<*from; i++)
			if (line[i] == TAB)
				printf("	");
			else
				printf(" ");
		for (i=*from; i<=*to; i++)
			{
			printf("^");
			if (line[i] == TAB)
	   { printf("\010^\010^\010^\010^\010^\010^\010|");
	   printf("\t\010\010^\010^\010^\010^\010^\010^\010^\010|"); }
			}
		printf("\n");
		get_line(new_line,"");
		if (strcmp(new_line, "."))
			{
			++*to;
			return(YES);
			}
		}
	cnt = 0;
	for (i=0; i<*from; i++)
		new_line[cnt++] = line[i];
	for (i=0; replace_string[i]; i++)
		{
		new_line[cnt++] = replace_string[i];
		if (cnt > MAX_LINE)
			OVER_FLOW;
		}
	next_to = cnt;
	i = *to + 1;
	do
		{
		new_line[cnt++] = line[i];
		if (cnt > MAX_LINE)
			OVER_FLOW;
		}
	while (line[i++]);
	*to = next_to;
	for (i=0; new_line[i]; i++)
		line[i] = new_line[i];
	line[i] = EOL;
	return(YES);
	}
ile (line[i++]);
	*to = next_to;
	for (i=0; new_line[i]; i++)
		line[i] = new_line[i];
	line[i] = EOL;
	return(YES);
	}
