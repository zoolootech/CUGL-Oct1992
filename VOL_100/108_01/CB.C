/*
	Pretty-printer for C Programs

	Swiped from the CIPG's UNIX system and modified to run
		under BDS C by William C. Colley, III

	Mods made July 1980

To use the program, type the following command line:

	A>cb input.fil [output.fil]

where input.fil is the file to be pretty-printed and [output.fil] is an
optional output file.  If no output file is specified, the output is sent
to the console.
*/

int slevel[10];
int clevel;
int spflg[20][10];
int sind[20][10];
int siflev[10];
int sifflg[10];
int iflev;
int ifflg;
int level;
int ind[10];
int eflg;
int paren;
int pflg[10];
char lchar;
char pchar;
int aflg;
int ct;
int stabs[20][10];
int qflg;
char *wif[2];
char *welse[2];
char *wfor[2];
char *wds[3];
int j;
char string[200];
char cc;
int sflg;
int bflg;
int peek;
int tabs;
char lastchar;
char c;

/*  Added disk I/O buffers, etc.:  */

char inpbuf[134], outbuf[134], *outvect;

main(argc,argv)
int argc;
char *argv[];
{
	int k;

	/*  Initialize everything here.  */

	outvect = 1;
	if (argc < 2 || argc > 3){
		printf("Usage:  A>cb input.fil [output.fil\n");
		exit();
	}
	if (fopen(*++argv,inpbuf) == -1){
		printf("File not found\n\n");
		exit();
	}
	if (argc == 3){
		if (fcreat(*++argv,outbuf) == -1){
			printf("Could not create output file\n");
			exit();
		}
		outvect = outbuf;
	}
	clevel = iflev = level = eflg = paren = 0;
	aflg = qflg = j = bflg = tabs = 0;
	ifflg = peek = -1;
	sflg = 1;
	initw(ind,"0,0,0,0,0,0,0,0,0,0");
	initw(pflg,"0,0,0,0,0,0,0,0,0,0");
	wif[0] = "if";
	welse[0] = "else";
	wfor[0] = "for";
	wds[0] = "case";
	wds[1] = "default";
	wif[1] = welse[1] = wfor[1] = wds[2] = 0;

	/*  End of initialization.  */

	while((c = getchr()) != 032){
		switch(c){
		default:
			string[j++] = c;
			if(c != ',')lchar = c;
			break;
		case ' ':
		case '\t':
			if(lookup(welse) == 1){
				gotelse();
				if(sflg == 0 || j > 0)string[j++] = c;
				puts();
				sflg = 0;
				break;
			}
			if(sflg == 0 || j > 0)string[j++] = c;
			break;
		case '\n':
			if(eflg = lookup(welse) == 1)gotelse();
			puts();
			printf("\n");
			sflg = 1;
			if(eflg == 1){
				pflg[level]++;
				tabs++;
			}
			else
				if(pchar == lchar)
					aflg = 1;
			break;
		case '{':
			if(lookup(welse) == 1)gotelse();
			siflev[clevel] = iflev;
			sifflg[clevel] = ifflg;
			iflev = ifflg = 0;
			clevel++;
			if(sflg == 1 && pflg[level] != 0){
				pflg[level]--;
				tabs--;
			}
			string[j++] = c;
			puts();
			getnl();
			puts();
			printf("\n");
			tabs++;
			sflg = 1;
			if(pflg[level] > 0){
				ind[level] = 1;
				level++;
				slevel[level] = clevel;
			}
			break;
		case '}':
			clevel--;
			if((iflev = siflev[clevel]-1) < 0)iflev = 0;
			ifflg = sifflg[clevel];
			puts();
			tabs--;
			ptabs();
			if((peek = getchr()) == ';'){
				printf("%c;",c);
				peek = -1;
			}
			else printf("%c",c);
			getnl();
			puts();
			printf("\n");
			sflg = 1;
			if(clevel < slevel[level])if(level > 0)level--;
			if(ind[level] != 0){
				tabs -= pflg[level];
				pflg[level] = 0;
				ind[level] = 0;
			}
			break;
		case '"':
		case '\'':
			string[j++] = c;
			while((cc = getchr()) != c){
				string[j++] = cc;
				if(cc == '\\'){
					string[j++] = getchr();
				}
				if(cc == '\n'){
					puts();
					sflg = 1;
				}
			}
			string[j++] = cc;
			if(getnl() == 1){
				lchar = cc;
				peek = '\n';
			}
			break;
		case ';':
			string[j++] = c;
			puts();
			if(pflg[level] > 0 && ind[level] == 0){
				tabs -= pflg[level];
				pflg[level] = 0;
			}
			getnl();
			puts();
			printf("\n");
			sflg = 1;
			if(iflev > 0)
				if(ifflg == 1){iflev--;
					ifflg = 0;
				}
				else iflev = 0;
			break;
		case '\\':
			string[j++] = c;
			string[j++] = getchr();
			break;
		case '?':
			qflg = 1;
			string[j++] = c;
			break;
		case ':':
			string[j++] = c;
			if(qflg == 1){
				qflg = 0;
				break;
			}
			if(lookup(wds) == 0){
				sflg = 0;
				puts();
			}
			else{
				tabs--;
				puts();
				tabs++;
			}
			if((peek = getchr()) == ';'){
				printf(";");
				peek = -1;
			}
			getnl();
			puts();
			printf("\n");
			sflg = 1;
			break;
		case '/':
			string[j++] = c;
			if((peek = getchr()) != '*')break;
			string[j++] = peek;
			peek = -1;
			comment();
			break;
		case ')':
			paren--;
			string[j++] = c;
			puts();
			if(getnl() == 1){
				peek = '\n';
				if(paren != 0)aflg = 1;
				else if(tabs > 0){
					pflg[level]++;
					tabs++;
					ind[level] = 0;
				}
			}
			break;
		case '#':
			string[j++] = c;
			while((cc = getchr()) != '\n')string[j++] = cc;
			string[j++] = cc;
			sflg = 0;
			puts();
			sflg = 1;
			break;
		case '(':
			string[j++] = c;
			paren++;
			if(lookup(wfor) == 1){
				while((c = gets()) != ';');
				ct=0;
cont:
				while((c = gets()) != ')'){
					if(c == '(') ct++;
				}
				if(ct != 0){
					ct--;
					goto cont;
				}
				paren--;
				puts();
				if(getnl() == 1){
					peek = '\n';
					pflg[level]++;
					tabs++;
					ind[level] = 0;
				}
				break;
			}
			if(lookup(wif) == 1){
				puts();
				stabs[clevel][iflev] = tabs;
				spflg[clevel][iflev] = pflg[level];
				sind[clevel][iflev] = ind[level];
				iflev++;
				ifflg = 1;
			}
		}
	}
	if (outvect != 1){
		printf("\032");
		fflush(outbuf);
	}
	exit();
}
ptabs(){
	int i;
	for(i=0; i < tabs; i++)printf("\t");
}
getchr(){
	if(peek < 0 && lastchar != ' ' && lastchar != '\t')pchar = lastchar;
	lastchar = (peek<0) ? getc(inpbuf):peek;
	peek = -1;
	return(lastchar == '\r' ? getchr():lastchar);
}
puts(){
	if(j > 0){
		if(sflg != 0){
			ptabs();
			sflg = 0;
			if(aflg == 1){
				aflg = 0;
				if(tabs > 0)printf("    ");
			}
		}
		string[j] = '\0';
		printf("%s",string);
		j = 0;
	}
	else{
		if(sflg != 0){
			sflg = 0;
			aflg = 0;
		}
	}
}
lookup(tab)
char *tab[];
{
	char r;
	int l,kk,k,i;
	if(j < 1)return(0);
	kk=0;
	while(string[kk] == ' ')kk++;
	for(i=0; tab[i] != 0; i++){
		l=0;
		for(k=kk;(r = tab[i][l++]) == string[k] && r != '\0';k++);
		if(r == '\0' && (string[k] < 'a' || string[k] > 'z'))return(1);
	}
	return(0);
}
gets(){
	char ch;
beg:
	if((ch = string[j++] = getchr()) == '\\'){
		string[j++] = getchr();
		goto beg;
	}
	if(ch == '\'' || ch == '"'){
		while((cc = string[j++] = getchr()) != ch)if(cc == '\\')string[j++] = getchr();
		goto beg;
	}
	if(ch == '\n'){
		puts();
		aflg = 1;
		goto beg;
	}
	else return(ch);
}
gotelse(){
	tabs = stabs[clevel][iflev];
	pflg[level] = spflg[clevel][iflev];
	ind[level] = sind[clevel][iflev];
	ifflg = 1;
}
getnl(){
	while((peek = getchr()) == '\t' || peek == ' '){
		string[j++] = peek;
		peek = -1;
	}
	if((peek = getchr()) == '/'){
		peek = -1;
		if((peek = getchr()) == '*'){
			string[j++] = '/';
			string[j++] = '*';
			peek = -1;
			comment();
		}
		else string[j++] = '/';
	}
	if((peek = getchr()) == '\n'){
		peek = -1;
		return(1);
	}
	return(0);
}
comment(){
rep:
	while((c = string[j++] = getchr()) != '*')
		if(c == '\n'){
			puts();
			sflg = 1;
		}
gotstar:
	if((c = string[j++] = getchr()) != '/'){
		if(c == '*')goto gotstar;
		goto rep;
	}
}
printf(format,arg)
char *format;
unsigned arg;
{
	if (fprintf(outvect,format,arg) == -1){
		fprintf(1,"Disk write error\n");
		exit();
	}
}
