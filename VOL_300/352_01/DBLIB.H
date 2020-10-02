#ifndef DBLIB_HEADER
	#define DBLIB_HEADER
#include "wtwg.h"
#include "string.h"
#include <iostream.h>
#include <alloc.h>
#include <dos.h>

/* farmemcpy()
 * in large models this is just memcpy
 * but in small models it's movedata()
 */
#undef LARGE_DATA_MODEL
#ifdef __LARGE__
	#define LARGE_DATA_MODEL
#endif
#ifdef __COMPACT__
	#define LARGE_DATA_MODEL
#endif
#ifdef __HUGE__
	#define LARGE_DATA_MODEL
#endif 




#ifdef LARGE_DATA_MODEL
	#define farmemcpy(aa,bb,cc)  memcpy (aa,bb,cc)
#else

	/* model is small or medium - need a far * equivalent of
	 * memcpy, write, read, memcmp
	 */
	#define farmemcpy(dest,src,num)  \
	movedata (FP_SEG(src), FP_OFF(src), FP_SEG(dest), FP_OFF(dest), num )

#endif	/* memcpy */



// min & max. - these are undefined in TurboC++
#ifndef min
	#define min(x,y)   (( (x) < (y) ) ? (x) : (y))
#endif
#ifndef max
	#define max(x,y)   (( (x) > (y) ) ? (x) : (y))
#endif





/* array index routines.
 *
 * roll_up, roll_down
 *	returns the next/prev index to an array of max items
 * 	arranged in a circle  0 -> 1 -> 2 -> ... -> max -> 0 etc...
 * inch_up, inch_down
 *	increases or decreases the index but stays within array bounds
 */
#define roll_up(n, max)  	( (n) == (max)-1 ? 0       : (n)+1 )
#define roll_down(n, max)	( (n) ==  0      ? (max)-1 : (n)-1 )
#define inch_up(n, max)		( (n) == (max)-1 ? (n)     : (n)+1 )
#define inch_down(n, max)	( (n) ==  0      ? 0       : (n)-1 )





// NORMALIZE macros.
//	Far pointer normalization routine
//		Arithmetic on far pointers only affects the offset,
//		does not carry into the segment portion.
// 	to avoid overflow errors,
//		must convert seg:off pair to place as much
// 		of the address as possible into the segment part.
//
//	 NORMALIZE () - unconditionally normalize a far pointer.
//      _NORMALIZE () - normalize a data pointer IF memory model requires it
//
//  use _NORMALIZE for pointers not explicitly declared far.
//
//	NOTE:  MODIFIED FOR C++  which must cast the result to the appropriate type
// 
#define NORMALIZE(fp, type)		\
   fp = (type) MK_FP ( ( FP_SEG(fp) + (FP_OFF(fp)>>4) ),  \
		((unsigned)(FP_OFF(fp))& 0x000f) )





#ifdef  __COMPACT__
	#define _NORMALIZE(fp,type)  NORMALIZE(fp,type)
#endif
#ifdef  __LARGE__
	#define _NORMALIZE(fp,type)  NORMALIZE(fp,type)
#endif
#ifdef __SMALL__
	#define _NORMALIZE(fp,type)
#endif
#ifdef __MEDIUM__
	#define _NORMALIZE(fp,type)
#endif
#ifdef __HUGE__
	#define _NORMALIZE(fp,type)
#endif













/* the next set of definitions allow use of Turbo C's ability to
 *  generate in-line interrupts, while maintaining compatibility
 *  with compilers that have an int86() function and a union REGS.
 *
 *  To use this technique, ALWAYS load the registers in inverse
 *  alphabetical order (DX first, then CX, BX and AX last)
 *  and don't try to do any complex computations in the lines
 *  that access the registers.
 *	 WRONG, for 3 reasons:
 *		_AH = 1;
 *		_DX = value1 + 3*value2;	===> TurboC uses AX
 *		geninterrupt ( variable_int_number );  ditto
 *	RIGHT, but useless:
 *		value3 = value1 + 3*value2;
 *		if ( int_number = 0x01 )
 *			{
 *			_DX = value3;
 *			_AX = 1;
 *			geninterrupt ( 0x01 );
 *			}
 *		else	{
 *			_DX = value3;
 *			_AX = 1;
 *			geninterrupt ( 0x02 );
 *			}
 *
 */
	#ifndef  __TURBOC__
		#define PSEUDOREGS union REGS regs;
		#define _AX	regs.x.ax
		#define _AL	regs.h.al
		#define _AH	regs.h.ah
		#define _BX	regs.x.bx
		#define _BL	regs.h.bl
		#define _BH	regs.h.bh
		#define _CX	regs.x.cx
		#define _CL	regs.h.cl
		#define _CH	regs.h.ch
		#define _DX	regs.x.dx
		#define _DL	regs.h.dl
		#define _DH	regs.h.dh
		#define _FLAGS	regs.x.flags

		#define geninterrupt(intno)	int86((intno), &regs, &regs)

	#else
		#define	PSEUDOREGS
	#endif	/* end of redifinning pseudoregs and interrupts */

	//	generate inport and outport instructions inline
	//  temp. fix pending TURBOC++ upgrade.
	#ifndef outportb
		#define outportb(portid, value)  __outportb__( (portid), (value) )
		void	 _Cdecl __outportb__(int __portid, unsigned char __value);

	#endif
	#ifndef inportb
		#define inportb(portid)  __inportb__( (portid) )
		unsigned char	_Cdecl __inportb__(int __portid);
	#endif

// wmatherr() -
//		a routine to trap integer and floating point math errors.
//		prevents program from crashing, which would leave system...
//			 in bizarre video modes, with expanded memory allocated, etc...
//		SOURCE: wmatherr.cpp
void wmatherr (void);



// STRINGPP.H - a string class for TurboC++
//
// D Blum 8/90 
//


class String {
	public:
			static 	char caseSens;		// default = OFF  any NONZERO is ON
	private:
			int	n;			// strlen = numbytes not counting \0
			char *s;		// the string.
			void construct ( char *p );	// local utility for construction
			void destruct ( );        	// local utility for destruction
										// source for both: STRPP.CPP
			// Utility functions called by functions in String::
			//	the utilities include copy, comparison, scanning, allocation
			//												// source files:
			static int cmp (char *a, char *b);				// STRPPCMP.CPP
			static int memcmp (char *a, char *b, int nb);	// STRPPMCP.CPP
			static int chrcmp ( char a, char b );			// STRPPCHC.CPP
			static int findstr ( char *a, int na, char *b );// STRPPFNS.CPP	
			static int findchr ( char *a, int na, char c );	// STRPPFNC.CPP
			void       strpad ( );							// STRPPPAD.CPP
			void  		slide ( int start, int stop );		// STRPPSLD.CPP 
	public: 
			// Constructors: make String from a String, a char *ptr, or a char
			//   			 also can make fixed length strings.
				String()	{ n=0; s=NULL;};
				String(int);					// make a fixed len String 
												// init to all spaces.
												// source: STRPPFIX.CPP
				String ( char *ptr ){n = strlen (ptr); construct (ptr); };		
				String ( String& str ){ n= str.n; construct ( str.s ); };
			// Copy constructor utility.
				private: void copy( int l, char *p );	// source: STRPP.CPP
			public:
				void    assign ( char *data );	// assign ownership of data
												// data MUST have been malloc'd
												// and will be free'd by String
												//		BE CAREFULL !!!!!
												// STRPPASN.CPP
				String& operator=(String& source)		
					{
					copy ( source.n, source.s );
					return *this;
					};			// end assignemt operator
				String& operator=(char *ptr)
					{
					copy ( strlen(ptr), ptr );
					return *this;
					};
				String& operator= (char c)
					{
					copy ( 1, &c );
					return *this;
					};
			// Destructor
				~String ( ) {destruct();};		// source: STRPP.CPP
				
			// comparison operators.
			
			friend int operator==(String& a, String &b)
				{return (String::cmp (a.s, b.s)==0);};
			friend int operator>=(String& a, String &b)
				{return (String::cmp (a.s, b.s)>=0);};	
			friend int operator<=(String& a, String &b)
				{return (String::cmp (a.s, b.s)<=0);};	
			friend int operator>(String& a, String &b)
				{return (String::cmp (a.s, b.s)>0);};	
			friend int operator<(String& a, String &b)
				{return (String::cmp (a.s, b.s)<0);};	
			friend int operator==(char *a, String &b)
				{return (String::cmp (a, b.s)==0);};
			friend int operator>=(char *a, String &b)
				{return (String::cmp (a, b.s)>=0);};	
			friend int operator<=(char *a, String &b)
				{return (String::cmp (a, b.s)<=0);};	
			friend int operator>(char *a, String &b)
				{return (String::cmp (a, b.s)>0);};	
			friend int operator<(char *a, String &b)
				{return (String::cmp (a, b.s)<0);};	
			friend int operator==(String& a, char *b)
				{return (String::cmp (a.s, b)==0);};
			friend int operator>=(String& a, char *b)
				{return (String::cmp (a.s, b)>=0);};	
			friend int operator<=(String& a, char *b)
				{return (String::cmp (a.s, b)<=0);};	
			friend int operator>(String& a, char *b)
				{return (String::cmp (a.s, b)>0);};	
			friend int operator<(String& a, char *b)
				{return (String::cmp (a.s, b)<0);};	
				
				
		// CONCATENATION - 
		//		two ways:  += replace old string. 
		//					+ allocates new string intermediate.
		//      source: STRPPCAT.CPP
		//
		friend String& operator+=(String& a, String& b);
		friend String& operator+=(String& a, char *b);
		friend String  operator+(String& a, String& b);
		friend String  operator+(String& a, char *b);
		friend String  operator+(char *a, String& b);
		
		// STREAM IO
		friend ostream& operator<<(ostream& os, String& a)
			{ return os<<a.s; };
		friend ostream& operator<<(ostream& os, String* a)
			{ return os<<a->s; };
				
		// EXTERNAL ACCESS		casts & subscripts
		// cast String to char* for times when you need a C string ptr.
		// (this lets you to use Strings as parms in C funcs that need char* )
		//			ex:  String myFile="prog.cpp";	fopen (myFile, "r");
		//
		operator char* () {return s;};		
		
		// subscript a String just like a C string ptr
		char   	operator [](int i) {return s[i];};	
		
		// len() get lenght of String.
		int 	len()  {return n;};
				
		// set character at position i to value c.
		//		 NOTE: 	no bounds checking - that's your job. 
		//				Also, DONT set contents to \0 ie: myString.set (3, 0);
		void set( int i, char c ) { s[i]=c; };
			
		// resetLen()
		//	recalculate the length of the String.
		//	used if a non-String func does something to shorten contents.
		//
		void resetLen()  { n=strlen(s); if (n==0) destruct(); };
		
		
		// SUBSTRING()
		// 		creates a substring from position start for length len.
		// 		if len not specified or less than 0, goes to end of string.
		//		if len extends beyond end of 'this' String, pads result with ' '
		// 		NOTE: 1) creates a new string, which must be explicitly deleted.
		//			  2) returns a pointer to the created String
		//			  3) obeys String::caseSens
		//	
		//											source: STRPPSUB.CPP
		String *substring(int start, int len=0);
		String *substr   (int start, int len=0) {return substring(start,len);};
			
			
		// INSERT  insert one string into another.
		// insert string ins into current string at position pos.
		// alters the existing string.
		//											source: STRPPINS.CPP
		String&  insert ( char *ins, int pos );
		String&  insert ( String& ins, int pos ) {return insert(ins.s,pos);};
			
			
		// CUT  remove part of a string.
		//		removes section of string between positions specified.
		//			(between is inclusive)
		//		Changes original string.
		//		If values provided are out of range, 
		//			string is replaced by an error message.
		//		Default for second value is cut to end of string.
		//		Example:
		//		input string:    abcdefghi0
		//			positions:   0123456789		n =9 letters in string.
		//     		cut ( 2, 5 ):abghi0	
		//					position 6 moved right to position 3
		//			cut ( 2 ):   ab0			cuts to end of string.
		//											source: STRPPCUT.CPP
		String& cut ( int start, int stop= -1 );
			
			
		// String::find() searches for a substring within target string.
		//				similar to ANSI C functions strchr(), strstr() 
		//				but: obeys String::caseSens rule for case sensitive 
		//				RETURNS: offset into 'this' String of first occurance.
		//						 -1 if not found. 
		//
		// example: String s1="Hello, there";
		//			s1.find("there")  ----> returns 7 = offset of there.
		//			s1.findAny("sample")  ----> returns 2 = position of 'l'
		//	source: STRPPFNS.CPP and STRPPFNC.CPP
		//
		int find ( String& a ) {return String::findstr(s,n,a.s);};
		int find ( char   *a ) {return String::findstr(s,n,a  );};
		int find ( char    c ) {return String::findchr(s,n,c  );};
		
		
		
		// findAny()  - finds first occurance of any char within 'this' String.
		//				similar to ANSI C strpbrk() 
		//				but: obeys String::caseSens rule for case sensitive 
		//				RETURNS: offset into 'this' String of first occurance.
		//						 -1 if not found. 
		// 											source: STRPPFNA.CPP
		int findAny ( char *a );	
		int findAny ( String& a ) {return String::findAny(a.s);};	
		
		// findNot()  - finds first char which is not in the param string.
		// 				functions similar to findAny()
		//				RETURNS: index for first char not in param string.
		//						 -1 if all chars in param are also in 'this'
		//	
		//											source: STRPPFNT.CPP
		int findNot ( char * );
		int findNot ( String& a)	{return String::findNot(a.s);};
			
			
			
		
		// String::tokenize()  parse original string into tokens.
		//		RETURNS: ptr to newly constructed String.			
		//			
		//		NOTE: caller has to delete each returned string
		//						  or they will accumulate on the heap.
		//
		//		the returned string contains chars in between the tokens
		//		tokenize progressively decimates 'this' down to a \0.
		//
		//		example:
		//		String  x="a1+b2-c3";	String *y;
		//				y = x.tokenize("+-");	Now x="b-3", y="a1"
		//				delete y;
		//				y = x.tokenize("+-");	Now x="3";   y="b2"
		//				delete y;
		//				y = x.tokenize("+-");	Now x=NULL,  y="c3"
		//				delete y;
		//
		//		if no tokens are found, the entire string is returned  
		//			and 'this' is decimated.			
		//
		//											source: STRPPTOK.CPP
		//
		String *tokenize ( char *tok );
		String *tokenize ( String &Stok )  { return tokenize (Stok.s); };

		// case conversion 
		//		source: STRPPTOU.CPP and STRPPTOL.CPP
		void toUpper();
		void toLower();
			
		
		// squeeze()  
		//		- the characters provided are squeeze'd from 'this' String
		//		The resulting string may be shorter.
		//		Useful for removing all whitespace, carriage returns, etc...
		//			(default is remove whitespace).
		//
		//										source: STRPPSQZ.CPP
		String& squeeze ( char* =" \r\n\t");	
		String& squeeze ( String& a ) { return squeeze (a.s); };	
			
		// noExtra()
		//		You specify a list of charaters to squeeze out (target chars).
		//			target defaults to whitespace.
		//		any 'extra' target chars from the String provided are removed.
		//		characters are extra if they immediately follow another target.
		//			ie: a consecutive run of target chars is shortenned to 1.
		//		first char of each run is replaced by first char of target.
		//		obeys String::caseSens for identifying target chars.
		//
		//		Similar to squeeze() but not as drastic.
		//			use to simplify whitespace to single spaces only
		//	
		//		example:  String too_many_vowels = "Wear green paint today";
		//				  String vowels = "aeiouy"
		//				  too_many_vowels.noExtra(vowels);
		//					result is "War gran pant tada"
		//		example:  String spacey ="Lots    \nof\t\r \nspace";	
		//				  spacey.noExtra() becomes: "Lots of space";
		//										source: STRPPNOX.CPP
		String& noExtra ( char* =" \r\n\t");
		String& noExtra ( String& a) { return noExtra (a.s);};
		
		
		// trim()
		//		trailing chars of 'this' are removed if found in given string
		//										source: STRPPTRM.CPP
		String& trim ( char* = " \r\n\t" );
		String& trim (String& a ) { return trim (a.s); };
			
			
		// translate()
		//		characters in String found in first parm 
		//		are changed to corresponding character in second parm.
		//		example: String X = "12345";  
		//				 X.translate ( "1234567890", "abcdefghij" );
		//				 X becomes "abcde"
		//										source: STRPPTRN.CPP
		String& translate ( char *first, char *second );
			
		// replace()
		// 	replace all occurences of *original with *replacement.
		// 	obeys String::caseSens rule for finding original.
		//	original and replacement don't have to be same lengths.
		//	example: String test = "No I do not know if Nolan noticed";
		//			 test.replace ("no", "yes");
		//			result is "yes I do yest kyes if yeslan yesticed"
		//										source: STRPPREP.CPP
		String& replace ( char *, char * );
		String& replace ( String& o, String& r ) {return replace(o.s,r.s);};
		String& replace ( char   *o, String& r ) {return replace(o  ,r.s);};
		String& replace ( String& o, char   *r ) {return replace(o.s,r  );};
		
			
					// end definition of class String
		};		
			
//---------------------------------------------------------------------------


// make_fullfilename ()
//		routine to concatenate a directory name and a file name
//		PARMS:  traditional C style ptrs to char strings
//				specifying directory (may be "C:\\path", "C:", "C:\\path\\" etc
//				and filename (may be up to 8+1+3+1 bytes: name.ext or just name)
//				and ( optional ) extension which may or may not incl. the "."
//
//		RETURNS a new String object (which must explicitly be deleted)
//		example:
//		String  *fullname = make_fullfilename ("C:\\mydir", "name.ext");
//				fopen ( (char*)fullname, "rt" );
//				delete fullname;
//			or: *fullname = make_fullfilename ( NULL, "name.ex1", ".ex2" );
//					(above returns "name.ex2")

//		SOURCE: FULLFLNM.CPP
String *make_fullfilename ( char *dir, char *fn, char *ext=NULL ); 




// filesize()
//		get and return filesize.
//		returns -1 if file not found.
//										source: FILESIZE.CPP
long filesize ( char *filename );


// readASCIIfile ()  writeASCIIfile ()  editASCIIfile ()
//	
//		readASCIIfile allocates a buffer and reads an ASCII file (text mode)
//		the buffer is allocated based on the size of the ASCII file.
//		extra bytes of padding may be specified, and a maximimum size can be set
//		PARMS:	char *dir, char *fn, char *ext - as used by make_fullfilename()
//				int pad = number of extra bytes to add to the end of the buffer
//							(so you can add more to the file later)
//				NOTE: the buffer is NEVER longer than 32000 bytes
//						defined in RD_ASCII_MAXBUFFER
//		RETURNS: char *buffer - ptr to the buffer holding the file's contents.
//								must be freed later.
//								IF FILE did not exist, buffer[0] = 0.
//
//	writeASCIIfile()
//		this routine writes a buffer to the named ASCII file.
//		
// editASCIIfile ()
//		reads in specified file, allows user to change contents, rewrites
//
//															TEXTFILE.CPP
#define RD_ASCII_MAX_BUFFER  32000
void writeASCIIfile(char *dir, char *fn, char *ext, char *buffer);
char *readASCIIfile(char *dir, char *fn, char *ext, size_t pad=0);
void editASCIIfile ( char *title, char *dir, char *fn, char *ext, int pad=25 );







// argscan() and ARGTBL
//		unified approach to command-line parms.
//		ARGTBL: NULL-terminated list of strings to match and addresses of data
//								also max length of string (incl term. 0)
//								if len==1, data is treated like single char.
//								if len==0, data value is converted to int.
//		argscan () - interpret argc/argv according to argtbl
//				RETURNS:  number of unmatched args.
//				MODIFIES: argv points to list of unmatched args, terminated by 0
//					(array of ptrs to strings, with NULL terminator.)
//					return value of 0 means all args were matched.
//				NOTE that argv[0] was program name, becomes 1st unmatched arg.
//				
//		example entries: invoke program.exe  -A- -c23 name=GEORGE 
//					char name[10], switchA='+', int count=1;  ...defaults
//					ARGTBL args[]= {"name=",name, sizeof(name),
//									"-A",   switchA, sizeof(switchA),
//									"-C",   count, 0 }; 
//		NOTE: uses case-insenstive match.
//		
//									source: ARGSCAN.CPP
typedef struct st_argtbl
	{
	char *arg_id;
	void *arg_ptr;
	int   arg_len;
	}
	ARGTBL;

int argscan ( int argc, char **argv, ARGTBL *argtbl );
	




//FEOW.C - function to ask user "File Exists, Over Write?"
//		PARAMS: char dir = directory name. 		MUST HAVE strlen(dir) < MAXDIR
//				char fn[8+1=3+1] = file name
//
//		User gets chance to change file name, so provide storage for 8+1+3+1
//		RETURNS: ENTER or ESCAPE
//											source: FEOW.CPP
int fe_ow ( char *dir, char *fn );		
	
	
	
	







//  LINKLIST.- a base class for including linked lists into other classes
//				for TurboC++
//
//		This is NOT a container class. 
//		Each object in this class is one node of a linked list,
//			so to create a linked list of a derived class, 
//			the derived class has to have one base-node for the whole class
//										and as many nodes as there are objects.
//
//		example:(note macro creates 'node' and 'base'.):
//		class SampleClass {
//				InheritLinkClass(SampleClass)		<<<< NOTE no semicolon
//				public:
//				SampleClass()  : node(base,this) { ...constructor stuff... };
//				~SampleClass() 					 {   node.unLink(); };			
//				};
//
//
//	DBlum 8/90


class  LinkClass {
	private:  LinkClass *nx, *pv; void *dt;
			  void initBase ();
	public: 		
		// Constructors: one for empty nodes, one for nodes linked to base.
		LinkClass() 			{ nx=pv=this; dt=NULL;}; 
		LinkClass(LinkClass& base, void *data);
		// Destructors: none. assumes derived classes removes nodes from list
		int  isEmpty ();
		void insertAbove ( LinkClass & );	// mv 'this' above specified obj
		void insertBelow ( LinkClass & );	// mv 'this' below
		void * next () 	{return  nx-> dt;};	
		void * prev ()	{return  pv-> dt;};
		void unLink (); 
		void data ( void *new_this )	{ dt = new_this; };
		};
		
	// InheritLinkClass() macro allows correct typecasting of return values
	// 	from the calls to first() last() next() and prev().
	//

	#define InheritLinkClass(typecast)	\
	private:   static 	LinkClass base;			/* one per class */	\
						LinkClass node;			/* one per object*/	\
	public:														\
	typecast* 	first( ) { return (typecast*) base.next(); };	\
	typecast* 	last ( ) { return (typecast*) base.prev(); };	\
	typecast* 	next ( ) { return (typecast*) node.next(); };	\
	typecast* 	prev ( ) { return (typecast*) node.prev(); };	\
	static int 	isEmpty() { return base.isEmpty(); };			\
	void 		moveAbove(typecast & target ) 		\
					{								\
					node.unLink();				\
					node.insertAbove (target.node);	\
					};								\
	void 		moveBelow(typecast & target ) 		\
					{ 								\
					node.unLink(); 				\
					node.insertBelow (target.node);	\
					};								
	// end of InheritLinkClass() macro.




// Vlist - a general array of ptrs. 
//		 works best as an array of ptrs to char*
//		 the Vlist makes its own copy of the indicated string
//			when the list is deleted.
//		So you can add static strings, consts, items on local heap, etc...
//		ie: Vlist is NOT a container class 
//			in the sense of smalltalk or like the TurboC++ classlib.
//								

class Vlist {
	private:
		int max;			// max number of entries without reallocating.
		int n;				// number actually in list, not counting NULL @end
		void **list;		// ptr to array of ptrs
		virtual int compare ( void*, int );
					// compare as strings or FixedLen structures.
		virtual void copy ( void* );		// add copy of element.
											// doesn't check size of list.
	public:
		Vlist ();			// constructors. No parms = list of strings
		void Vlist::clear (void);			// remove all entries.		
		~Vlist ()  {this->clear();};
		
		Vlist (Vlist&);						// VLISTEQ.CPP
		Vlist& operator=(Vlist&);			// VLISTEQ.CPP

		int count (void)	{return n;};
		int isEmpty (void)	{return (n==0);};

		
		void push ( void* );			// add item to list.
										
		void push ( Vlist& );			// add one list to another. VLISTEQ.CPP		
															
		void pop ( void );				// remove most recent item from list						
										
		void Remove ( int );			// remove specific item from list
										// parm is item to match to.
										// NOTE remove() is a TurboC macro
										//	so can't use it as a func. name.
																
		operator char **() 	{ return (char **)list;	};									
		char * 	operator[](int i) { return (char*)( list[i] );};						// index list gives items stored.
										// ie: Vlist vl;
										//     vl.push ("String a");
										//	   vl.push ("String b");
										//     for (int n=0; vl[n] !=NULL; ++n)
										//			... iterate thru list.
										//			vl[1] = "String b"
		
		int match ( void * );			// find item in given Vlist
										// returns offset number 
										// returns n ( [] to NULL ) if not found
										// VLMATCH.CPP
		
		
		
		};		// end of definition of class Vlist.
		
		
// VlistParseString () -  parse a String into a Vlist.
//		params: Vlist& v is list to hold output (=parsed string)
//				char* s is input String to parse ( or may use String& s )
//				sepchars are separators to use in parsing.
//				NOTE: uses Vlist, String::tokenize() functions.
//		returns: number of token words found.    VLPARSES.CPP
int VlistParseString ( Vlist& v, char *s, char *sepchars =",;\n\r" ); 		
		
		

//	WILDFILE - functions for expanding DOS wildcard filenames
//	wildfile - pass it a filename (may include drive and path), and attr
//				(see findnext/findfirst in TurboC for meanings of attr)
//			 - return is a ptr to a new Vlist of all matching files.
//					(the new Vlist must be explicitly free'd)
//
//	wildfile_pick - params are same as for wildfile().
//				constructs a Vlist, shows contents to user (calls wpicklist() )
//				user selects a file, 
//				caller's string area for filename is overwritten
//				with the selected filename ( max len=8+1+3+1 )
//				or with \0 if no matching files or user hits ESCAPE.
//				(The Vlist is free'd);
//
void  wildfile_pick (char *spec, int attr =0);
Vlist  *wildfile 	(char *spec, int attr =0);





/* wfvfile ()	
 *		WFORM entry validation routine for file names.
 *		uses data in form as a string for file specification "*.*" or similar
 *		if wildcards are present, presents user with list of files.
 *		if no wildcards, makes sure file exists, and gives 'not found' message
 *		PARAMETERS & RETURN VALUES: as defined for form editing routines.
 *		NOTE: the data item named MUST BE AT LEAST 13 bytes, long enough to hold
 *				"filename.ext"+terminal 0 = 8+1+3+1.
 * 	SOURCE WFVFILE.C
 */
int wfvfile ( WFORM *form, char *buffer );


/* macro for easy form creation for file items.
 */
#define  WFORMENTRY_FL(xx) #xx ": ",  xx, NULL, "%s", wfvfile, 13, 0


// wfvpath()
//	form entry validation to validate DOS paths
//	valid entries are: NULL, path\ or c:\path, etc...
//	path is checked to make sure it exists and is not empty.
//										source: WFVPATH.CPP
int wfvpath (WFORM *fm, char *buf);

/* macro for easy form creation
 */
#define  WFORMENTRY_PT(xx) #xx ": ",  xx, NULL, "%s", wfvpath, sizeof(xx), 0


// validate_path()
//	PARAM: char *path = string giving (optional) drive: and path to check.
//						max length of string is MAXDRIVE+MAXDIR (see dir.h)
//			path may or may not end in \
//	RETURN  0 if path exists
//		   -1 if path doesn't exist.
//	NOTE: 	NULL is considered to be a valid path (current directory)
//						
//										source: VLDTPATH.CPP
int validate_path ( char *path );		
	




#endif		// end of DBLIB.H for TurboC++ 