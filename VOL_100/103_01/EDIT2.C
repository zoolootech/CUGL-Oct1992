/*	Hello!  This is the second half of the editor just like mom used
	to make (oops...) I mean, just like the QED editor.  This is the
	line storage system.
*/

#include "edit.inc"

init_buffer()
	{
	int i;
	parsels[0].type = EOF;
	for (i=0; i<NO_MEMORIES; i++)
		{
		memory[i].used = NO;
		memory[i].lru = i;
		}
	temp_used = NO;
	unlink(TEMP_FILE);
	dot = dollar = 0;
	}

insert_line(n, line)
	int n;
	char line[];
	{
	int b, t, tot_len, w;
	char *p, *q, *x, *v;
	if (parsels[0].type == EOF)
		{
		parsels[1].type = EOF;
		parsels[0].type = IN_MEM;
		parsels[0].where = 0;
		parsels[0].line_no = 1;
		memory[0].used = YES;
		memory[0].parsel_no = 0;
		}
	for (b=1; parsels[b].type != EOF; b++)
		if (parsels[b].line_no >= n)
			break;
	b--;
	t = swap_in(b);
	p = memory[t].buffer;
	tot_len = strlen(p) + strlen(line);
	if (tot_len >= BUFF_SIZE)
		split = tot_len/2;
	else
		split = 0;
	line_split = parsels[b].line_no-1;
	split_at = b+1;
	q = temp3_buff;
	for (w=n-parsels[b].line_no; w; w--)
		for (;;)
			if (save(&q, *p++) == LF)
				break;
	for (x=line; *x; x++)
		save(&q, *x);
	save(&q, LF);
	while (save(&q, *p++))
		;
	x = temp3_buff;
	v = memory[t].buffer;
	while (*v++ = *x++)
		;
	for (w=b+1; parsels[w].type != EOF; w++)
		parsels[w].line_no++;
	dollar++;
	modified_file = YES;
	if (n <= number1)
		number1++;
	if (n <= number2)
		number2++;
	if (n <= number3)
		number3++;
	}

char save(p, n)
	char **p, n;
	{
	int t1, i;
	*(*p)++ = n;
	if (n==LF)
		line_split++;
	if (NOT --split)
		{
		**p = EOL;
		t1 = swap_new();
		for (i=NO_PARSELS-2; i>=split_at; i--)
			{
			parsels[i+1].type = parsels[i].type;
			parsels[i+1].where = parsels[i].where;
			parsels[i+1].line_no = parsels[i].line_no;
			}
		for (i=0; i<NO_MEMORIES; i++)
			if (memory[i].parsel_no >= split_at)
				memory[i].parsel_no++;
		parsels[split_at].type = IN_MEM;
		parsels[split_at].where = t1;
		parsels[split_at].line_no = line_split;
		memory[t1].parsel_no = split_at;
		*p = memory[t1].buffer;
		}
	return(n);
	}

/*	This will delete a line */
delete_line(n)
	int n;
	{
	int b, t, u, c;
	char *p, *q;
	for (b=1; parsels[b].type != EOF; b++)
		if (parsels[b].line_no > n)
			break;
	b--;
	if (b AND parsels[b].line_no == n)
		{
		t = swap_in(b-1);
		p = memory[t].buffer;
		for (u=n-parsels[b-1].line_no; u; u--)
			for (;;)
				if (*p++ == LF)
					break;
		*p = 0;
		}
	t = swap_in(b);
	p = memory[t].buffer;
	for (u=n-parsels[b].line_no; u; u--)
		for (;;)
			if (*p++ == LF)
				break;
	for (q=p; *q++ != LF;)
		;
	while (*p++ = *q++)
		;
	for (c=b+1; parsels[c].type != EOF; c++)
		parsels[c].line_no--;
	if (parsels[b+1].type != EOF)
		combine(b);
	if (b)
		combine(b-1);
	dollar--;
	modified_file = YES;
	if (number1 >= n)
		number1--;
	if (number2 >= n)
		number2--;
	if (number3 >= n)
		number3--;
	}

/*	This will combine two parsels into one */
combine(a)
	int a;
	{
	int t1, t2, c1, c2, i;
	char *p, *q;
	t1 = swap_in(a);
	t2 = swap_in(a+1);
	c1 = strlen(memory[t1].buffer);
	c2 = strlen(memory[t2].buffer);
	if (c1+c2 > BUFF_SIZE)
		return;
	p = memory[t1].buffer + c1;
	q = memory[t2].buffer;
	while (*p++ = *q++)
		;
	for (i=a+1; parsels[i-1].type != EOF; i++)
		{
		parsels[i].type = parsels[i+1].type;
		parsels[i].where = parsels[i+1].where;
		parsels[i].line_no = parsels[i+1].line_no;
		}
	for (i=0; i<NO_MEMORIES; i++)
		if (memory[i].parsel_no > a+1)
			memory[i].parsel_no--;
	memory[t2].used = NO;
	}

/*	This will read a line */
read_line(n, line)
	int n;
	char line[];
	{
	int b, c, u, t;
	char *p;
	for (b=1; parsels[b].type!=EOF; b++)
		if (parsels[b].line_no > n)
			break;
	b--;
	c = 0;
	if (b AND parsels[b].line_no==n)
		{
		t = swap_in(b-1);
		p = memory[t].buffer;
		for (u=n-parsels[b-1].line_no; u; u--)
			for (;;)
				if (*p++ == LF)
					break;
		for (; *p; p++)
			line[c++] = *p;
		}
	t = swap_in(b);
	p = memory[t].buffer;
	for (u = n-parsels[b].line_no; u; u--)
		for (;;)
			if (*p++ == LF)
				break;
	for (; *p != LF; p++)
		line[c++] = *p;
	line[c] = EOL;
	}

/*	This function will garentee a parsel in swapped in */
int swap_in(par)
	int par;
	{
	int t, n;
	if (parsels[par].type == IN_MEM)
		{
		t = parsels[par].where;
		do_lru(t);
		return t;
		}
	t = swap_new();
	do_lru(t);
	memory[t].parsel_no = par;
	seek(temp_used, READ_SIZE*parsels[par].where, 0);
	read(temp_used, memory[t].buffer, READ_SIZE);
	parsels[par].type = IN_MEM;
	parsels[par].where = t;
	return t;
	}

/*	This is create a new what_ever */
int swap_new()
	{
	int i;
	for (i=0; i<NO_MEMORIES; i++)
		if (memory[i].used == NO)
			goto out_of_loop;
	i = swap_out();
out_of_loop:
	memory[i].used = YES;
	do_lru(i);
	return(i);
	}

int swap_out()
	{
	int t, i, n, j;
	if (NOT temp_used)
		if ((temp_used = temp_open()) == ERROR)
			{
			temp_used = NO;
			printf("Temp file error...\007\n");
			exit(NO);
			}
	t = 0;
	for (i=1; i<NO_MEMORIES; i++)
		if (memory[i].lru > memory[t].lru)
			t = i;
	n = memory[t].parsel_no;
	for (i=0;; i++)
		{
		for (j=0; parsels[j].type != EOF; j++)
			if (parsels[j].type == IN_TEMP_FILE AND parsels[j].where == i)
				goto comp_exit;
		break;
comp_exit:;
		}
	seek(temp_used, READ_SIZE*i, 0);
	if (write(temp_used, memory[t].buffer, READ_SIZE) != READ_SIZE)
		{
		printf("Temp file error\007\n");
		exit(NO);
		}
	memory[t].used = NO;
	parsels[n].type = IN_TEMP_FILE;
	parsels[n].where = i;
	return(t);
	}

/*	This will make n the most recently used memory block */
do_lru(n)
	int n;
	{
	int i;
	for (i=0; i<NO_MEMORIES; i++)
		if (memory[i].lru < memory[n].lru)
			memory[i].lru++;
	memory[n].lru = 0;
	}

/*	This will (try to) open the temp file */
int temp_open()
	{
	int fd;
	fd = creat(TEMP_FILE);
	if (fd == ERROR)
		return(fd);
	close(fd);
	fd = open(TEMP_FILE, 2);
	return(fd);
	}

/*	This will read a file */
read_file(file_name)
	char file_name[];
	{
	int fd, lines, t, p, i, j;
	char *q;
	BOOL end_file;
	modified_file = NO;
	strcpy(deflt_file, file_name);
	if ((fd = open(file_name, 0)) == ERROR)
		{
		printf("\007??\n");
		return;
		}
	if (temp_used)
		close(temp_used);
	init_buffer();
	lines = 1;
	end_file = NO;
	for (p=0;; p++)
		{
		if (p < NO_MEMORIES)
			{
			q = memory[p].buffer;
			memory[p].buffer[BUFF_SIZE] = EOL;
			memory[p].used = YES;
			memory[p].parsel_no = p;
			parsels[p].type = IN_MEM;
			parsels[p].where = p;
			}
		else
			{
			q = temp3_buff;
			parsels[p].type = IN_TEMP_FILE;
			parsels[p].where = p-NO_MEMORIES;
			}
		q[BUFF_SIZE] = EOL;
		parsels[p].line_no = lines;
		i = read(fd, q, READ_SIZE);
		if (i < READ_SIZE)
			q[i*CPM_SIZE] = CNTRL_Z;
		for (i=j=0; j<=BUFF_SIZE; j++)
			if (q[j] == CNTRL_Z)
				{
				q[i++] = EOL;
				end_file = YES;
				}
			else if (q[j] != CR)
				q[i++] = q[j];
		if (p==NO_MEMORIES)
			{
			temp_used = temp_open();
			if (temp_used == ERROR)
				{
				printf("Error with temp file\007\n");
				init_buffer();
				return;
				}
			}
		if (p>=NO_MEMORIES)
			{
			if (write(temp_used, temp3_buff, READ_SIZE)!=READ_SIZE)
				{
				printf("Error with temp file\007\n");
				init_buffer();
				return;
				}
			}
		for (t=0; t<BUFF_SIZE; t++)
			{
			if (q[t] == LF)
				{
				lines++;
				dot = 1;
				dollar++;
				}
			else if (q[t] == EOL)
				if (end_file)
					goto end_of_file;
				else
					break;
			}
		}
end_of_file: close(fd);
		parsels[p+1].type = EOF;
	}

/*	This will write the file */
#define OOPS {printf("Write error...\007\n"); return;}
write_file(file_name)
	char *file_name;
	{
	int count, k, l, i, t;
	char c;
	printf("%s\n", file_name);
	modified_file = NO;
	strcpy(deflt_file, file_name);
	if ((filed = creat(file_name)) == ERROR)
		{
		printf("\007??\n");
		return;
		}
	count = 1;
	k = 1;
	l = 0;
	if (dollar)
		t = swap_in(0);
	i = 0;
	while (count <= dollar)
		{
		c = memory[t].buffer[l++];
		if (c == EOL)
			{
			t = swap_in(k++);
			l = 0;
			continue;
			}
		if (c == LF)
			{
			if (write_save(CR, &i))
				OOPS;
			count++;
			}
		if (write_save(c, &i))
			OOPS;
		}
	if (write_save(CNTRL_Z, &i))
		OOPS;
	if (write_flush(i))
		OOPS;
	close(filed);
	}

BOOL write_save(c, i)
	char c;
	int *i;
	{
	temp4_buff[(*i)++] = c;
	if (*i == CPM_SIZE)
		{
		if (write(filed, temp4_buff, 1) != 1)
			return(YES);
		*i = 0;
		}
	return(NO);
	}

BOOL write_flush(i)
	int i;
	{
	if (write(filed, temp4_buff, 1) != 1)
		return(YES);
	return(NO);
	}
return(NO);
	}

BOOL write_flush(i)
	int i;
	{
	if (write(filed, temp4_buff, 1) != 1)