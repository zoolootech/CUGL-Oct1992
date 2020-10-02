#
# Sample infix notation calculator program for SCI (version 1.5)
#
char *Lineptr;
int Stack[10], Stackptr, Stacktop;

calc()
{
   char line[80];

   Stacktop = 10;
   for(;;)
   {
      puts("-> ");
      if( gets(Lineptr=line) )
      {
         addition();
         printf( "%d\n", pop() );
      }
      else
         return;
   }
}

number()
{
   if( isdigit( *Lineptr ) )
   {
      push( atoi( Lineptr ) );
      while( isdigit( *Lineptr ) )
         ++Lineptr;
   }
}

addition()
{
   int num;

   multiplication();
   for(;;)
   {
      if( *Lineptr=='+' )
      {
         ++Lineptr;
         multiplication();
         push( pop() + pop() );
      }
      else if ( *Lineptr=='-' )
      {
         ++Lineptr;
         multiplication();
         num = pop();
         push( pop() - num );
      }
      else
         return;
   }
}

multiplication()
{
   int num;

   number();
   for(;;)
   {
      if( *Lineptr=='*' )
      {
         ++Lineptr;
         number();
         push( pop() * pop() );
      }
      else if ( *Lineptr=='/' )
      {
         ++Lineptr;
         number();
         num = pop();
         push( pop() / num );
      }
      else
         return;
   }
}

push( n )
{
   if( Stackptr<Stacktop )
      return Stack[ Stackptr++ ] = n;
   puts( "stack overflow\n" );
}

pop()
{
   if( Stackptr>0 )
      return Stack[ --Stackptr ];
   puts( "stack underflow\n" );
}

isdigit(c){ return '0'<=c && c<='9';}
