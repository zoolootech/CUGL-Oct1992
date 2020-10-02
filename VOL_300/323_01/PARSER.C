/*--------------------------------------------------------------------------*\
| PARSER.C                                                                   |
\*--------------------------------------------------------------------------*/

/*
   Adventure Parser

   Need to include the adv-world definition file before including this one.
   This is because the vocabulary definitions depend on the adventure-world
   objects.
*/

#include "string.h"

#include "adv-def.h"
#include "parser.h"

CmdRec cmd;           /* global command structure */

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))


/*----------------------------*/
/* Get the verb and the noun from a command of the form "verb noun".
   If noun is omitted, a null string returned.

   Will handle leading blanks, and too long strings.
*/
ParseCommand( char *cm, char *verb, char *noun )
{
  char *p,*q;

  *verb='\0'; *noun='\0';
  while (*cm && *cm==' ') cm++;   /* skip leading blanks in command */

  p = strchr( cm, ' ' );          /* find space delimiting verb */
  q = strchr( cm, '\0');          /* get last char */

  if (!p)   p = q;                /* if 1 word, set ptr */
  strncat( verb, cm, min(p-cm, max_word_len) );

  if (*p)                         /* if >1 word */
  {
    while (*p && *p==' ') p++;    /* skip lead blanks */
    strncat( noun, p, min(q-p, max_word_len) );
  }
}

/*----------------------------*/
/* Make a string exactly v_sig characters long (plus null), truncating or
   blank-padding if necessary
*/
resize_word( char *s )
{
  int i;

  for ( i=strlen(s); i<v_sig; i++ )
    *(s+i) = ' ';
  *(s+v_sig)='\0';
}

/*----------------------------*/
/* Return a word's vocabulary #, given the vocab array and it's size
*/
int GetWordNum( char *w,   vocab_type *voc )
{
  int wn,i;
  v_word sh_word;

  strcpy( sh_word, w );
  resize_word( sh_word );
  for ( wn=0;  *voc->name;  voc++ )
  {
    if (!strcmp( sh_word, voc->name ))
    {
      wn = voc->num;
      break;
    }
  }
  return( wn );
}

/*----------------------------*/
/*
*/
int GetVerbNum( char *verb )
{
  return( GetWordNum( verb, v_verb ));
}

int GetNounNum( char *noun )
{
  return( GetWordNum( noun, v_noun ));
}

/*----------------------------*/
/* Parse the command into the verb and noun, then find out the verb & noun #'s
*/
AnalyseCommand( CmdRec *cmd )
{
  ParseCommand( cmd->cm,   cmd->verb, cmd->noun );
  strlwr( cmd->verb );
  strlwr( cmd->noun );
  cmd->vn = GetVerbNum( cmd->verb );
  cmd->nn = GetNounNum( cmd->noun );

  strcpy( cmd->sh_verb, cmd->verb );
  resize_word( cmd->sh_verb );
  strcpy( cmd->sh_noun, cmd->noun );
  resize_word( cmd->sh_noun );
}

