                                 Table I.

                    Proposed Standard for C Code Layout


	I.    Standard tabs are used for program indentation.

		 a. Four space tabs may be used, if desired, after column 56. 
                    This is permitted in order to prevent line wrap on 		    
                    80-column displays.

	II.   A brace is generally on a line by itself.

	 	 a. The lowest level braces appear on the left margin.

		 b. Comments  may appear on lines containing a brace,  but only 
                    after the brace.

		 c. Braces are indented to the same depth as the statement 
                    which invoked the block they frame.

		 d. In variable initializations, the opening brace may be 
                    placed on the same line as the equal sign, but the closing 
                    brace must have the same indentation as the opening one.

		 e. In general, no statements may appear on the same line as a 
                    brace.



	III.  A block begins with a left brace and ends with a right brace. Its 
              contents are indented an extra level to indicate the  nesting 
              depth.

		 a. Whenever a block is longer than 24 lines (a standard  CRT 
                    page), a comment should follow the closing brace to
		    indicate the block which the brace closes. 
			i.  This applies to whole functions as well as regular 
                            blocks.
                    	ii. This  rule  should  also be  applied  with  shorter 
                            blocks  when  block nesting makes the code  complex 
                            and these comments improve the readability.

		 b. The opening and closing braces of a block are always 
                    indented indentically.

		 c. The  case/default labels of a switch statement  are  always 
                    indented   a  level  like  statements  in  a  block.    The 
                    statements which follow these labels are always indented an 
                    extra level to improve readability.

		 d. Regular labels (destinations for goto's) are always placed 
                    at the left margin, regardless of nesting depth.

		 e. When a null block is used (eg '{}'), it may appear on the 
                    same line as statements (eg do {} while(expr);).

		 f. If  a single statement is used instead of a  block,  it  is 
                    indented  a single level,  just as if it were surrounded by 
                    braces.
			i.  Null statements (ie just a ';') are indented in the 
                            same way as regular statements.

	IV.  White space is added in expressions and assignments to improve  
             readability.

		 a. Relational operators are delimited by single spaces.

		 b. Equal signs are delimited by single spaces.

		 c. Unary operators are not separated by space from their 
                    operands.

		 d. Parentheses are added to improve readability in complex 
                    expressions, even if they are not required to produce 
                    correct evaluation.
			i.  A return statement always has a set of parentheses 
                            surrounding its expression.

		 e. No white space character is placed between function names 
                    and their parenthesized argument list.

		 f. No white space character is placed between a keyword (eg. 
                    if) and its parenthesized argument.

	V.   Comments are added liberally to make the program read easily.

		 a. If a comment requires more than one line, the start/end 
                    comment tokens are placed on lines containing no comment 
                    text. In this case, the start/end tokens are indented 
                    identically.

		 b. If a comment fits on a single line, the start/end tokens 
                    must also be placed on that line.

	VI.   Variables are always declared, even if your version of C has a 
              default type.  Always explain the purpose of your variables.

		 a. Declare the variables in logical groups, and include a 
                    comment on the same line as the declaration to describe the 
                    function(s) of the variable(s).

	         b. Avoid numerous declarations on a single line.

		 c. Explain complex pointer declarations.

		 d. Variable names are always lower case only.

		 e. External variable declarations may be indented a single 
                    level for greater readability.

	VII.  Constants created with #define are always upper case.  Macros 
              created with #define may be upper or lower case.

		 a. As with variables, constants should have explanatory com-
                    ments to explain their purpose.

		 b. Macros should be explained via comments to avoid mis-
                    understandings about their uses.  This is especially 
                    important since macros tend to be cryptic.

		 c. Restrict #define statements to the beginning of code files 
                    (ie before the first function).  This avoids the potential 
                    for redefinition and other confusion.

	VIII. Other items.

		 a. The else..if(expr) construct is placed on a single line as 
                    if it were a single keyword.

		 b. Function names are always lower case.

	IX.   Portability considerations.

		 a. When using a non-standard C feature, always prepare a 
                    portable alternative which may be selected via conditional 
                    compilation.

                 b. Indicate any subtle uses of sign extension and type 
                    conversions made by your program or your specific compiler.

                 c. Indicate any deviations in the way your compiler handles 
                    pointer arithmetic.

                 d. Include the ampersand operator ('&') when you pass pointers 
                    to structures, even if your compiler doesn't require it.  
                    Someone else's compiler may support passing structures.

		 e. Indicate any deviations of your runtime library from 
                    the standard.  

                 f. Indicate any deviation in the (argc,argv) command line 
                    conventions made by your compiler.

