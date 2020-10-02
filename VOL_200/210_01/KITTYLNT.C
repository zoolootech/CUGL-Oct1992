/*lintoff*/
/* Saturday, July 12, 1986  3:41 PM                                  */
/*                                                                   */
/*     KITTENSOFT                                                    */
/*                                                                   */
/*     Dan Schechter                                                 */
/*     Route 1 Box 19                                                */
/*     Amenia, North Dakota 58004                                    */
/*     (701) 967-8371                                                */
/*                                                                   */
/*     Do what you like with this program. Please let me know if     */
/*     you find bugs. Exactly as is it compiles under Aztec C II     */
/*     for CP/M. I have no idea how portable it is because I do      */
/*     not know how compatible the Manx library functions are with   */
/*     those of other compilers.                                     */
/*                                                                   */
/*     This is not intended to be a real LINT program. That would    */
/*     be far beyond my programming ability. This is just a sort     */
/*     of delimiter checker. I have used it only for a short time    */
/*     but I have found it useful. I hope you will also.             */
/*                                                                   */
/*     Following are some general comments about this program:       */
/*
When checking for proper line termination it returns <good> on
end-of-comment marker. This might not be the best choice as it could mask
an error on the same line (when a short comment is on the same line as
a line of code).

It expects if, for, and while loops to be terminated by closing parenthesis
(no semicolon) if and only if the loop declaration has no following statement.

	Example 1:	if (a==b)
				x=y;

	Example 2:	if (a==b) x=y;

Both the above are correct and will not cause error messages.

	Example 3:	while(a[i++]!=0);

In the above, the programmer wants to increment i until a[i] is 0,
thus the semicolon is correct, but the lint program expects any kind of
loop to control a statement or block of statements, and so generates
the error message "Check line termination".

Sometimes, line labels will generate spurious error messages.

It will check printf(), fprintf(), sprintf and scanf() to see that the number
of parameters matches the number of <%> conversion characters. However it
does NOT check for type consistency, or anything about parameters 
passed to functions. It will check to make sure that a for() loop has
exactly two semicolons inside the parentheses. A spurious error message
will be generated if a function name ends in "for" as the program thinks
it is a for() loop.

It will check for the balance of () parentheses, [] brackets, "" quote marks, 
and {} braces. Spurious error messgaes may be generated if any of these are
used as text characters. Parentheses, braces, and quotes, but not brackets,
may be balanced on following lines if the LAST character on a line is a
backslash. Even white space following a backslash is not permitted. This is
consistent with Aztec C. This program will flag white space following the 
last backslash on a line.

It will flag a line that appears to be a function declaration with a
semicolon. This may generate some spurious error messages.

It will flag nested or unbalanced comment markers.

It will not check for undeclared variables.

It does not recognize the comma that terminates lines in initializing
statements as permissable. Thus initializers will generate spurious error
messages. I suggest toggling lint off for these. 

Most of the time it will find the syntax mistakes I make most often:
unbalanced parentheses, brackets, braces, and quotes; missing semicolons,
semicolons after function declarations, nested comments, parameters
left out of printf() statements, and too many or two few semicolons
inside for() statements.

Checking is toggled on and off with the words                         */
/*linton*/
/*lintoff*/                                                           /*
which must appear starting at column 1, between comment markers,
without spaces. They may not be nested.
                                                                      */
/*linton*/

#include <stdio.h>
#define BELL 7
int errno;
main(n,arg)
int n;
char **arg;
{
	int i,op=0,cp=0,ob=0,cb=0,os=0,cs=0,cmt=0,sq=0,quo=0,quot=0,l=0,conline=0;
	char disab=0,q,string[300],fn[20],*fgets(),*r=1,cmtb=0;
	FILE *fp;

/* If there is a file name on the command line it will use it. Otherwise */
/* it will ask for a file name                                           */

	if (n==2) strcpy(fn,arg[1]);
	else getfn(fn);

o:	if ((fp=fopen(fn,"r"))==0){
		printf("Can't find %s  errno=%d",fn,errno);
		errno=0;
		getfn(fn);
		goto o;
	}
	
	while(r){
		l++;

/* The following initializes string to all newline characters. Without this, */
/* under rare circumstances, the last line can get spurious error flags as   */
/* the string winds up with part of the next-to-last line hanging on.        */

		for (i=0;i<255;i++) string[i]='\n';

/* The following gets one line of source code */

		r=fgets(string,250,fp);

/* It ignores lines beginning with # because they usually require no */
/* special line termination:                                         */

		if (string[0]=='#') continue;
		if ((i=strncmp(string,"/*lintoff*/",11))==0) {
			disab=1;

/* conline keeps track of how many lines have been printed, so the */
/* program can pause for you to read the error messages.           */

			conline++;
			printf("\
%s line: %d --> LINT TURNED OFF.\n\n",fn,l);
			if (conline>9) pause(&conline);
		}
		if ((i=strncmp(string,"/*linton*/",10))==0) {
			disab=0;
			conline++;
			printf("\
%s line: %d --> LINT TURNED BACK ON.\n\n",fn,l);
			if (conline>9) pause(&conline);
		}
		if (disab) continue;
		q=cktsp(string);
		if (q=='b'){
			conline++;
			printf("\
%s line: %d --> White space after backslash:\n%s",fn,l,string);
			q='r';
			if (conline>9) pause(&conline);
		}
		diddle(string);

/* This is to skip blank lines:  */ 

		if (string[0]=='\n') continue;
		q=endcheck(string);    
		if (  (q=='f') && ((op!=cp)||(ob!=cb))  ) {
			conline++;
			printf("\
%s line: %d --> New function declaration. --> %s\
Unbalanced parentheses<%d> or wavy brackets<%d> in the preceding function.\n",\
fn,l,string,(op-cp),(ob-cb));
			if (conline>9) pause(&conline);
			op=0;cp=0;ob=0;cb=0;os=0;cs=0;
		}
		if (q=='b'){
			q=endexcpt(string);
			if (q=='b'){                     
				conline++;
				printf("\
%s line: %d --> check line termination:\n%s",fn,l,string);
				if (conline>9) pause(&conline);
			}                                
			q='r';
		}
		if (q=='g'){
			q=loopwscn(string);
			if (q=='b'){                     
				conline++;
				printf("\
%s line: %d --> check line termination:\n%s",fn,l,string);
				if (conline>9) pause(&conline);
			}                                
			q='r';
		}

/* These keep track of brackets: */

		for (i=0;string[i]!='\n';i++){
			if (string[i]=='(') op++;
			if (string[i]==')') cp++;
			if (string[i]=='{') ob++;
			if (string[i]=='}') cb++;
			if (string[i]=='[') os++;
			if (string[i]==']') cs++;

/* This keeps track of comment markers: */

			if (string[i]=='*'){
				if (string[i-1]=='/') cmt++;
				if (string[i+1]=='/') cmt--;
				if ((cmt>1)||(cmt<0)) cmtb=1; 
			}

/* These keep track of apostrophes and quotes: */

			if (string[i]=='\'') sq^=1;
			if (string[i]=='\"') {
				quo^=1;
				quot++;
			}
		}
		if (os!=cs) {
			conline++;
			printf("\
%s line: %d --> unbalanced square brackets:\n%s",fn,l,string);
			if (conline>9) pause(&conline);
		}
		os=0;
		cs=0;
		if (op!=cp) {
			q=querybks(string);
			if (q=='b') {
				conline++;
				printf("\
%s line: %d --> unbalanced parentheses:\n%s",fn,l,string);
				op=0;cp=0;
				if (conline>9) pause(&conline);
			}
		}
		if (cmtb){
			conline++;
			printf("\
%s line: %d --> Nested comments or close comment first:\n%s",fn,l,string);
			cmt=0;
			cmtb=0;
			if (conline>9) pause(&conline);
		}

/* There are two counters for quote marks. quo is used to check for */
/* unterminated strings. quot is used in testing printf() etc. for  */
/* parameter number. It tells the program when to begin and when to */
/* stop counting conversion characters.                             */

		if (quot) {
			if ((q=paramck(string))=='b'){
				conline++;
				printf("\
%s line: %d --> Unbalanced parameter/conversion number in printf:\n%s",fn,l,string);
				if (conline>9) pause(&conline);
				quot==0;
			}
			if (quot=='g') quot=0;
		}
		if (quo==1) {
			q=querybks(string);
			if (q=='b'){
				conline++;
				printf("\
%s line: %d --> Unterminated string:\n%s",fn,l,string);
				quo=0;
				if (conline>9) pause(&conline);
			}
		}
		if (sq==1){
			printf("\
%s line %d --> Unmatched apostrophe:\n%s",fn,l,string);
			conline++;
			if (conline>9) pause(&conline);
			sq=0;
		}
		q=funcdecl(string);
		if (q=='b') {
			conline++;
			printf("\
%s line: %d --> Semicoln after function declaration:\n%s",fn,l,string);
			if (conline>9) pause(&conline);
		}
		q=sminfor(string);
		if (q=='b') {
			conline++;
			printf("\
%s line: %d --> Improper number of arguments in for(;;) statement:\n%s",fn,l,string);
			if (conline>9) pause(&conline);
		}
	}
	if (ob!=cb) {
		conline++;
		printf("\n\
%s --> Unbalanced wavy brackets.",fn);
		if (conline>9) pause(&conline);
	}
	if (cmt) {
		conline++;
		printf("\n\
%s --> Unbalanced comment markers.",fn);
		if (conline>9) pause(&conline);
	}

	if (errno) {
		conline++;
		printf("\nThere was a file error. errno=%d",errno);
		if (conline>9) pause(&conline);
	}
	fclose(fp);
	printf("\n%cDone. Would you like to check another file? ",BELL);
	q=getreply(2,"yn");
	if (q=='n') exit();
	getfn(fn);
	op=0;cp=0;ob=0;cb=0;os=0;cs=0;l=0;r=1;conline=0,cmtb=0;
	goto o;
}

/* There are two versions of the following function getfn(). The first    */
/* is for CP/M. I like it because it lets me use Aztec C's T.LIB, which   */
/* results in much smaller compiled programs. It also seems to run better */
/* on my system.                                                          */

/* The second version uses fgets() instead of bdos(10) and so ought to be */
/* portable, I hope. I have left the first version "commented" OFF and    */
/* so the second version will be selected as this stands. You may want to */
/* try the first version if you have a CP/M system.                       */

getfn(name)
char name[];
{
	char i;
	name[0]=18;
	printf("\nPlease enter file name. ");
	bdos(10,name);
	printf("\n");
	name[name[1]+2]=0;
	for (i=0;i<18;i++) name[i]=name[i+2];
	for (i=0;name[i]!=0;i++) if (name[i]>96) name[i]-=32;
}

/*
getfn(name)
char name[];
{
	char i,*p,*fgets();
	printf("\nPlease enter file name. ");
	do { p=fgets(name,17,stdin); }
	while (name[0]=='\n');
	for (i=0;name[i]!='\n';i++);
	name[i]=0;
	for (i=0;name[i]!=0;i++) if (name[i]>96) name[i]-=32;
}
                                                                        */
/* The following function returns 'g' for good if the last character */
/* on the line is a backslash.                                       */

querybks(s)
char s[];
{
	char *p,*index();
	p=index(s,'\n');
	if (p==0) return('e');
	p--;
	if (*p=='\\') return('g');
	return ('b');
}

/* This one returns 'g' for good if there is a proper terminating character  */
/* at the end of the line. If the last character is a close parenthesis it   */
/* returns good if it appears to be a function declaration, as that does not */
/* want a normal line termination character.                                 */

endcheck(s)
char s[];
{
	char *p,*index(),q;

	p=index(s,'\n');
	if (p==0) return('e');
	p--;
	if (*p=='\\') return('g');
	if (*p=='{') return('g');
	if (*p=='}') return('g');
	if (*p==';') return('g');
	if ((*p=='/')&&(*(p-1)=='*')) return('g');
	if (*p==')') {
		q=askfunc(s);
		if (q=='y') return('f');
	}

/* The following returns 'g' if the only thing on the line is a begin-  */
/* comment marker, starting at column 1;                                */

	if ((p-s==1)&&(*(p)=='*')&&(*(p-1)=='/')) return('g');
	return('b');
}

/* This tries to determine if the line is a function declaration, and */
/* returns 'b' for bad if it appears to be a function declaration     */
/* with a semicolon after it. I included this because I often hit the */
/* semicolon key by reflex when it is not called for. However this    */
/* function does cause some spurious error messages. It returns 'b'   */
/* if column 1 contains a lower case alphabetic character, and there  */
/* is an open parenthesis within the first 10 characters, and the     */
/* line is terminated with a semicolon.                               */

funcdecl(s)
char s[];
{
	char *p,*index();
	int q;
	p=index(s,'\n');
	if (p==0) return('e');
	if (*(--p)==';')
		if (*(p-1)==')')
			if ((s[0]>96)&&(s[0]<123)){
				p=index(s,'(');
				if (p==0) return('g');

/* The 8 in the following line is because Aztec C limits labels to a   */
/* maximun of 8 characters. Thus anything longer is not likely to be   */
/* a function name. You may wish to change this if your system allows  */
/* longer function names. However the longer it is the more likely it  */
/* is to generate spurious error messages.                             */

				if ((p-s)>8) return('g');
				return('b');
			}
	return('g');
}

/* This is called if there is no "normal" terminating character at the  */
/* end of a line. It returns 'g' for good if the line is an "if", "for" */
/* or "while" loop. See also loopwscn() below.                          */

endexcpt(s)
char s[];
{
	char a=0,i=0;
	int q,parct=0,pmax=0;
	
	if (s[0]=='\n') return('g');
	while (s[i]=='\t') i++;
	if ((q=strncmp(s+i,"if",2))==0) a=1;
	if ((q=strncmp(s+i,"for",3))==0) a=1;
	if ((q=strncmp(s+i,"while",5))==0) a=1;
	if (a==0) return('b');
	
	for (i=0;s[i]!='\n';i++){
		if (s[i]=='(') parct++;
		if (s[i]==')') parct--;
		if (parct<0) return('b');
		if (pmax<parct) pmax=parct;
		if ((parct==0)&&(pmax>0)) break;
	}
	if (s[++i]=='\n') return('g');
	return('b');
}

/* This is another CP/M bdos call. I like it better than getchar() because */
/* I can use this one and still compile the program with the t.lib, which  */
/* makes for more compact programs. Also, on my compiler getchar()         */
/* requires you to press <RETURN>, whereas bdos(1,0) does not.             */
/*                                                                         */
/* The second version of this function uses getchar() instead, and so      */
/* should be portable. I have left the portable version ON and the CP/M    */
/* version OFF, as before. You may want to change that if you have CP/M.   */

getreply(n,s)
int n;
char s[];
{
	char c,i;
	for(;;){
		c=bdos(1,0);
		if ((c>64)&&(c<91)) c+=32;
		for (i=0;i<n;i++) if (c==s[i]) return(c);
	}
}

/*
getreply(n,s)
int n;
char s[];
{
	char c,i;
	for(;;){
		c=getchar();
		if ((c>64)&&(c<91)) c+=32;
		for (i=0;i<n;i++) if (c==s[i]) return(c);
	}
}
                                                                     */
/* This one just deletes white space from the end of the line for */
/* the convenience of other functions.                            */

diddle(s)
char s[];
{
	char i=0,q=0;
	for (;s[i]!='\n';i++){
		if (s[i]=='\t') continue;
		if (s[i]==' ') continue;
		q=1;
	}
	if (q==0) {
		s[0]='\n';
		s[1]=0;
		return;
	}
	i--;
	while((s[i]==' ')||(s[i]=='\t')) i--;
	s[++i]='\n';
	s[++i]=0;
}

/* This pauses operation when the screen has been filled with error     */
/* messages, and gives you a chance to read them and then continue      */
/* or exit the program. It also resets conline by means of the pointer. */

pause(a)
int *a;
{
	char q;
	printf("\n%cPaused. Would you like to continue? ",BELL);
	q=getreply(2,"yn");
	if (q=='n') exit();
	*a=0;
	printf("\n");
}

/* This is the one that checks for white space following a backslash */
/* as mentioned in the introduction. If there is more than one it    */
/* looks only at the last one.                                       */

cktsp(s)
char s[];
{
	char *p,*rindex();
	p=rindex(s,'\\');
	if (p==0) return('g');
	p++;
	if ((*p==' ')||(*p=='\t')) return('b');
	return('g');
}

/* This one returns 'y' if it thinks the line is a function declaration. */
/* It is not always right, so it causes some spurious error messages.    */
/* But I still like to have it.                                          */

askfunc(s)
char s[];
{
	char *p,*index();
	if ((s[0]>96)&&(s[0]<123)){
		p=index(s,'(');
		if (p==0) return('n');
		if ((p-s)>8) return('n');
		return('y');
	}
	return('n');
}

/* This function is the one that decides whether an "if" "for" or    */
/* "while" statement is properly terminated. As mentioned above,     */
/* it will flag a semicolon as an error if there is no other         */
/* statement on the same line. See the examples in the introduction. */

loopwscn(s)
char s[];
{
	char a=0,i=0;
	int q,parct=0,pmax=0;
	
	if (s[0]=='\n') return('g');
	while (s[i]=='\t') i++;
	if ((q=strncmp(s+i,"if",2))==0) a=1;
	if ((q=strncmp(s+i,"for",3))==0) a=1;
	if ((q=strncmp(s+i,"while",5))==0) a=1;
	if (a==0) return('g');
	
	for (i=0;s[i]!='\n';i++){
		if (s[i]=='(') parct++;
		if (s[i]==')') parct--;
		if (parct<0) return('b');
		if (pmax<parct) pmax=parct;
		if ((parct==0)&&(pmax>0)) break;
	}

	while ((s[++i]==' ')||(s[i]=='\t'));
	if (s[i]==';') return('b');
	return('g');
}

/* This will be passed lines that have quote marks in them. First it       */
/* looks for printf, sprintf, fprintf, or scanf and aborts if one is       */
/* not found, as its job is just to check those functions. At the first    */
/* quote mark it begins counting up % conversion characters. At the second */
/* quote mark it stops counting those and starts counting commas, until    */
/* the end of the line. If the two figures are equal it returns 'g' for    */
/* good, otherwise 'b' for bad. Also it returns 'b' if there is a comma    */
/* immediately before the final close parenthesis, as that suggests an     */
/* empty field. If all goes well it will generate an error message when the */
/* number of parameters does not match the number of % characters.         */
/* If the line is terminated with a backslash all the counters are held    */
/* static and the count continues on the next line. This function does not */
/* check for unbalanced parentheses, as that is flagged elsewhere.         */

paramck(s) 
char s[];
{
	char q,i=0;
	static int convct,paramct;
	static char a=0;

/* The following direct execution to the proper place in the function */
/* when the count has been carried over from a previous line.         */

	if (a==4) goto A4; 
	if (a==3) goto A3; 
	if (a==2) goto A2;
	if (a==1) printf("<a> error in paramck()\n");
	if (a>4) printf("<a> error in paramck()\n");
	convct=0 ; paramct=0;
	

/* The following will find the key words printf, scanf, fprintf, or sprintf */

	for (i=0;s[i]!='\n';i++)
		if ((q=strncmp(s+i,"printf",6))==0) a=1;
		if ((q=strncmp(s+i,"scanf",5))==0) a=1;
	if (a==0) return('g');

/* The following counts up the number of % characters */

A2:	for (i=0;s[i]!='\n';i++) 
		if (s[i]=='%'){
			convct++;
			i++;
			break;
		}
	if ((s[i]=='\n')&&(s[i-1]=='\\')){
		a=2;
		return('c');
	}
	if (s[i]=='\n') {
		a=0;
		return('g');
	}
A3:	for (;(s[i]!='\"')&&(s[i]!='\n');i++) if (s[i]=='%') convct++;
	if ((s[i]=='\n')&&(s[i-1]=='\\')){
		a=3;
		return('c');
	}
	if (s[i]!='\"') {
		a=0;
		return('g');
	}

/* This counts up the number of commas after the closing quotes. */

A4:	for (;(s[i]!='\n')&&(s[i]!=';');i++) if (s[i]==',') paramct++;
	if ((s[i]=='\n')&&(s[i-1]=='\\')){
		a=4;
		return('c');
	}
	if (s[i]!=';') {
		a=0;
		return('g');
	}

/* Now it re-sets the place marker and checks the counters for equality */

	a=0;
	if (convct!=paramct) return('b');

/* This steps backward from the end of the line past white space and */
/* parentheses and then looks for a comma. This is to make sure that */
/* the last field actually contains something.                       */

	while ((s[--i]==')')||(s[i]==' ')||(s[i]=='\t')||(s[i]=='('));
	if (s[i]==',') return('b');
	return('g');
}

/* This counts up the number of semicolons in a for(;;) loop. */

sminfor(s)
char s[];
{
	char i;
	int q,parct=0,pmax=0,ct=0;
	
	if (s[0]=='\n') return('g');

	for (i=0;s[i]!=0;i++) if ((q=strncmp(s+i,"for",3))==0) break;
	if (s[i]==0) return('g');
	
	for (i+=3;(s[i]==' ')||(s[i]=='\t');i++);
	if (s[i]=='(') parct++;
	else return('g');
	
	for (++i;s[i]!='\n';i++){
		if (s[i]=='(') parct++;
		if (s[i]==')') parct--;
		if (s[i]==';') ct++;
		if (parct<0) return('b');
		if (pmax<parct) pmax=parct;
		if ((parct==0)&&(pmax>0)) break;
	}
	if (ct==2) return('g');
	return('b');
}
