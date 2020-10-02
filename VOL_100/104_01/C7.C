
#ifndef TRUE    /* see if include needed */
#include <C.DEF>
#endif
heir11(lval,status)
int *lval;
int *status;
{
        int k;
        char *ptr;
        int  lval2[lvalsize];
        k=primary(lval,status);
        ptr=lval[0];
        blanks();
        switch(ch()) {

	        /* subscript varble  */
	        case '[' :
		        gch();  /* eat '[' */
		        switch(ptr[ident]) {

			        /* array of elements */
			        case array :
					*status=FALSE;
					++lval[1];
				        if (const_exp(lval2) && ch()==']') {
					        lval[2]=data_size(lval)*
						        lval2[0];
					        needbrack("]");
					        return NOTLOADED;
					        }
				        else {
					        expression();
					        size_adjust(lval);
					        swap();
					        address(lval,NOTLOADED);
					        add_address();
					        needbrack("]");
					        return ADDRESS;
					        }

			        /* pointer to elements */
			        case pointer:
					*status=FALSE;
				        if (const_exp(lval2) && ch()==']') {
						immed();
						outdec(data_size(lval)*
							lval2[0]);
						nl();
						swap();
						rvalue(lval,2);
						add_address();
					        }
				        else {
					        expression();
					        size_adjust(lval);
					        swap();
					        rvalue(lval,2);
					        add_address();
					        }
				        needbrack("]");
					lval[1] += 1;
				        return ADDRESS;

			        /* not a vaild ident for subscript */
			        default :
				        suberror(ptr);
				        return LOADED;
			        }

	        /* a function call */
	        case '(' :
			*status=FALSE;
		        gch(); /* eat '(' */
		        callfunction(lval[0]);
		        return LOADED;

	        /* not a subscripted or function call (not for this level */
	        default:
		        return k;
	        }
        }

primary(lval,status)
int *lval;
int *status;
{
        char *ptr;
        char sname[namesize];
        int  num[1];
        int k;

        /* clear lval array for use */
        lval[0]=
        lval[1]=
        lval[2]=0;

        if(match("(")) {
	        k=heir1(lval,status);
	        needbrack(")");
	        return k;
	        }

	*status=FALSE;
        if (symname(sname)) {
	        if ((lval[0]=findloc(sname))) return 2;
	        if ((lval[0]=ptr=findglb(sname)))
		        if (ptr[ident] != function) return 2;
	        if (ch() =='(') {
		        if (!(ptr=findglb(sname)))
		        if (glbptr>=endglb) {
			        error("global symbol table ofverflow");
			        }
		        else {
			        ptr=glbptr;
			        glbptr+=symsiz;
			        strcpy(ptr+name,sname);
			        ptr[ident]=function;
			        ptr[type]=cint;
			        ptr[offset]=
			        ptr[offset1]=
			        ptr[storage]=
			        ptr[indcnt]=0;
			        }

		        lval[0]=ptr;
		        return 2;
		        }
	        else {
		        undefine(sname);
		        return 0;
		        }
	        }
        if (constant(num)) {
	        return 0;
	        }
        else {
	        error("invalid expression");
	        junk();
	        return 0;
	        }
    }

/*					        */
/*      written 4/23/81 by Mike Bernson         */
/*					        */
size_adjust(lval)
int *lval;
{
        int size;
        size=data_size(lval);
        if (size == 1) return;
        if (size == 2) {
	        doublereg();
	        }
        }

/*					        */
/*      written 4/23/81 By Mike Bernson         */
/*					        */
data_size(lval)
int *lval;
{
        char *ptr;
        if (!(ptr=lval[0])) return 0;
        switch(ptr[ident]) {

	        /* array of elements */
	        case array :
		        switch(ptr[type]) {

			        /* char type */
			        case cchar:
				        return lchar;

			        /* int type */
			        case cint :
				        return lint;
			        }

	        /* pointer to elements */
	        case pointer :
		        if (lval[1] < ptr[indcnt] && ptr[indcnt] > 1)
			        return lpoint;
		        else switch(ptr[type]) {

			        /* type char */
			        case cchar :
				        return lchar;

			        /* type int */
			        case cint:
				        return lint;

			        }
		/* just a plain variable */
		case variable :
			return 1;
	        }
        }
/*					        */
/*      written 4/23/81 By Mike Bernson         */
/*					        */
address(lval,k)
int *lval;
{
        char *ptr;
        if (!(ptr=lval[0]) || k != NOTLOADED) return;
        switch(ptr[storage]) {

	        /* global symbol */
	        case statik :
		        immed();
		        outstr(ptr+name);
		        if (lval[2]) {
			        outstr("+");
			        outdec(lval[2]);
			        }
		        nl();
		        break;

	        /* local symbol */
	        case stkloc :
	        case stkarg :
		        immed();
		        outdec((ptr[offset]+ptr[offset1]*256+lval[2])-sp);
		        nl();
		        ot("dad\tsp");
		        nl();
		        break;

	        }
        }
/*						*/
/*	date written 6/6/81 by Mike Bernson	*/
/*						*/
/*	check to see if static load or store	*/
/*						*/
loadstatic(lval,k)
int *lval;
int k;
{
	char *ptr;

	ptr=lval[0];
	if (ptr[storage] == statik && k == NOTLOADED) return TRUE;
	else return FALSE;
	}
/*					        */
/*      date written 4/30/81 By Mike Bernson    */
/*					        */
store(lval,k)
int *lval;
int k;
{
        char *ptr;
	ptr=lval[0];
        if (k==0) return;
        switch(ptr[storage]) {

	        /* local or arg type */
	        case stkloc :
	        case stkarg :
	        switch(ptr[ident]) {

		        /* ident =array */
		        case array:
			        if (lval[1] == ptr[indcnt])
					if (k==ADDRESS) putstk(ptr[type]);
					else putmem_stack(lval,ptr[type]);
			        break;

		        /* ident is an pointer */
		        case pointer:
			        if (lval[1] == ptr[indcnt])
					if (k==ADDRESS) putstk(ptr[type]);
					else putmem_stack(lval,ptr[type]);
			        else	if (k==ADDRESS) putstk(cint);
					else putmem_stack(lval,cint);
			        break;

		        /* ident is a variable */
		        case variable :
			        if (k==ADDRESS) putstk(ptr[type]);
				else putmem_stack(lval,ptr[type]);
			        break;
		        }
		        break;

	        /* static varbles */
	        case statik :
		        switch(ptr[ident]) {

		        /* array */
		        case array :
			        if (lval[1] == ptr[indcnt])
				        if (k==1) putstk(ptr[type]);
				        else putmem(ptr+name,ptr[type],
					        lval[2]);
			        break;

		        /* pointer */
		        case pointer:
			        if (lval[1] == ptr[indcnt])
				        if (k==1) putstk(ptr[type]);
				        else putmem(ptr+name,ptr[type],lval[1]);
			        else
				        if (k==1) putstk(cint);
				        else putmem(ptr+name,cint,0);
			        break;

		        /* variable */
		        case variable :
			        if (k==1) putstk(ptr[type]);
			        else putmem(ptr+name,ptr[type],0);
			        break;
		        }
	        }
        }
/*					        */
/*      written 4/24/81 By Mike Bernson         */
/*					        */
rvalue(lval,k)
int *lval;
int k;
{
        char *ptr;
        ptr=lval[0];
        if (k==0) return;
        switch(ptr[storage]) {

	        /* local or arg type */
	        case stkloc :
	        case stkarg :
	        switch(ptr[ident]) {

		        /* ident =array */
		        case array:
			        if (lval[1] == ptr[indcnt])
					if (k==ADDRESS) indirect(ptr[type]);
					else getmem_stack(lval,ptr[type]);
				else 	address(lval,k);
			        break;

		        /* ident is an pointer */
		        case pointer:
			        if (lval[1] == ptr[indcnt])
					if (k==ADDRESS) indirect(ptr[type]);
					else getmem_stack(lval,ptr[type]);
			        else    if (k==ADDRESS) indirect(cint);
					else getmem_stack(lval,cint);
			        break;

		        /* ident is a variable */
		        case variable :
			        if (k==ADDRESS) indirect(ptr[type]);
				else getmem_stack(lval,ptr[type]);
			        break;
		        }
		        break;

	        /* static varbles */
	        case statik :
		        switch(ptr[ident]) {

		        /* array */
		        case array :
			        if (lval[1] == ptr[indcnt])
				        if (k==1) indirect(ptr[type]);
				        else getmem(ptr+name,ptr[type],
					        lval[2]);
			        else
				        if (k==1) indirect(ptr[type]);
				        else address(lval,k);
			        break;

		        /* pointer */
		        case pointer:
			        if (lval[1] == ptr[indcnt])
				        if (k==1) indirect(ptr[type]);
				        else getmem(ptr+name,ptr[type],lval[2]);
			        else
				        if (k==1) indirect(cint);
				        else getmem(ptr+name,cint,0);
			        break;

		        /* variable */
		        case variable :
			        if (k==1) indirect(ptr[type]);
			        else getmem(ptr+name,ptr[type],0);
			        break;
		        }
	        }
        }
equal_exp(lval,k)
int *lval;
int k;
{
        char *ptr;
        int  lval2[lvalsize];
	int status;

        ptr=lval[0];

        /* check to see if vaild lval */
        if ((ptr[ident]==array && ptr[indcnt] != lval[1]) ||
	        k==0 || lval[0] == 0) {
	        needlval();
	        return;
	        }
        address(lval,k);
        push();
        rvalue(lval,1);
	push();
        rvalue(lval2,heir1(lval2,&status));
        }


/*					        */
/*      written 4/9/81 By Mike Bernson          */
/*					        */
equal(lval,k)
int lval[];
int k;
{
        char *ptr;
        int lval2[lvalsize];
	int status;

        ptr=lval[0];

        /* check to see that is expresstion is array must be element */
        if (ptr[ident] == array && ptr[indcnt] != lval[1]) {
		        needlval();
		        return;
		        }
        switch(k) {

	        /* data is a constant or value */
	        case 0 :
		        needlval();
		        break;

	        /* address of variable is on stack */
	        case 1 :
		        push();
		        break;

	        /* nothing of variable is on stack */
	        case 2 :
		        break;
	        }
        rvalue(lval2,heir1(lval2,&status));
        }

test(label)
int label;
{
	int status;

        needbrack("(");
        status=expression();
        needbrack(")");
        testjump(label,status);
        }
/*				        */
/*      written by Mike Bernson 3/80    */
/*				        */
const_exp(val)
int val[];
{
        blanks();
        if (hex(val)) return 1;
        if (number(val)) return 1;
        if (pstr(val)) return 1;
        return 0;
        }
constant(val)
int val[];
{
        if (hex(val)) immed();
        else if (number(val)) immed();
        else if (pstr(val)) immed();
        else if (qstr(val)) {
	        immed();
	        printlabel(litlab);
	        outbyte('+');
	        }
        else return 0;
        outdec(val[0]);
        nl();
        return 1;
        }
hex(val)
int val[];
{
        int check;
        char look[2];
        if (ch() != '0' || (nch() != 'X' && nch() != 'x'))
	        return 0;
        gch();
        gch();
        look[1]=0;
        look[0]=toupper(gch());
        if (!(check=strpos("0123456789ABCDEF",look))) {
	        error("invaild hex constant");
	        junk();
	        val[0]=0;
	        return 1;
	        }
        val[0]=check-1;
        while(1) {
	        look[0]=toupper(ch());
	        if (!(check=strpos("0123456789ABCDEF",look)))
		        return 1;
	        val[0]=val[0]*16+check-1;
	        gch();
	        }
        }
number(val)
int val[];
{
        int k,minus;char c;
        k=minus=1;
        while(k) {
	        k=0;
	        if (match("+")) k=1;
	        if (match("-")) {
		      minus;
		        k=1;
		        }
	        }
        if (!numeric(ch())) return 0;
        while(numeric(ch())) {
	        c=inbyte();
	        k=k*10+(c-'0');
	        }
        if (minus<0) k=k;
        val[0]=k;
        return 1;
        }
pstr(val)
int val[];
{
        int k;
        k=0;
        if (!match("\'")) return 0;
        while(ch() != '\'' && ch()) k=(k&255)*256 + estr();
        gch();
        val[0]=k;
        return 1;
        }
qstr(val)
int val[];
{
        char c;
        if (!match("\"")) return 0;
        val[0]=litptr;
        while (ch() != '\"') {
	        if(!ch()) break;
	        if(litptr>=litmax) {
		        error("string space exhausted");
		        while(ch() !='\"' && ch()) estr();
		        return 1;
		        }
	        litq[litptr++]=estr();
	        }
        gch();
        litq[litptr++]=0;
        return 1;
        }
estr()
{
        char check;
        if (ch() != '\\') return gch();
        gch();
        switch(toupper(check=gch())) {
	        case 'R' : return CR;
	        case 'N' : return LF;
	        case 'B' : return BS;
	        case 'T': return TB;
	        case 'F' : return FF;
	        }
        if (!isdigit(check)) return check;
        check=check-'0';
        while(isdigit(ch())) check=check*8+gch()-'0';
        return check;
        }
