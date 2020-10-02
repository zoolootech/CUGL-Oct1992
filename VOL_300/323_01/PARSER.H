/*--------------------------------------------------------------------------*\
| PARSER.H                                                                   |
\*--------------------------------------------------------------------------*/

/*
   Adventure Parser
*/

#define v_sig 4               /* 1st 4 letters of vocab words significant */

#define max_cmd_size 64                /* at most 63 chars per command */
#define max_cmd_len max_cmd_size-1

#define max_word_size 16               /* at most 15 chars per word in input */
#define max_word_len max_word_size-1

typedef char v_cmd [max_cmd_size ];
typedef char v_word [max_word_size ];  /* a vocabulary (normal) word */


typedef struct        /* a command */
{
  v_cmd
    cm;               /* command entered */

  v_word
    verb,             /* verb part */
    noun,             /* noun part */
    sh_verb,          /* short form of verb and noun for comparison */
    sh_noun;

  int
    vn,               /* verb # in vocab */
    nn;               /* noun # in vocab */
}
CmdRec;


ParseCommand( char *cm, char *verb, char *noun );
resize_word( char *s );
int GetWordNum( char *w,   vocab_type *voc );
int GetVerbNum( char *verb );
int GetNounNum( char *noun );
AnalyseCommand( CmdRec *cmd );


extern
  CmdRec cmd;

