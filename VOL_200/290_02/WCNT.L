%{

/*
**  word count
*/

char title[7] = "WCNT" ;
int nchar, nword, nline ;

void usage( void) ;

%}

%%

[\n]         			{ ++nchar ;  ++nline ; }
[\r]					{ ++nchar ; }
[^ \t\n]+   			{ ++nword ;  nchar += yyleng ; }
.           			{ ++nchar ; }

%%

int main( int argc, char *argv[])
{
    nchar = nline = nword = 0 ;

    if (argc != 2)
        usage() ;

    if ((yyin = fopen( argv[1],"rb")) == NULL)
    {
        printf( "file: %s wasn\'t found", argv[1]) ;
        exit(1) ;
    }

    (void) yylex() ;

    printf("FILE %s:\tcharacters  %5d, words %5d, lines %5d\n",
			argv[1], nchar, nword, nline) ;

    return 0 ;
}


void usage( void)
{
	fprintf( stderr, "%s\t\t%s\n", title, __DATE__) ;
    fprintf( stderr, "usage:\tWCNT infile.ext\n") ;
    fprintf( stderr, "\tcounts the number of characters,\n") ;
    fprintf( stderr, "\twords and lines in a text file.\n") ;

    exit(1) ;
}

