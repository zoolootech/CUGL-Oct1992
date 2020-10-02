
Reprinted from: Micro/Systems Journal, Volume 1. No. 2. May/June 1985
Article Title: "C Forum - Writing A Translation Program In C"
Author: Alex Soya
-----------------------------------------------------------------
Copy of back issue may be obtained for $4.50 (foreign $6) from:
Subscriptions: $20/yr & $35/2 yrs domestic; published bimonthly
Micro/Systems Journal
Box 1192
Mountainside NJ 07092
-----------------------------------------------------------------
Copyright 1986
Micro/Systems Journal, Box 1192, Mountainside NJ 07092
This software is released into the public domain for
 non-commercial use only.
-----------------------------------------------------------------

±  #includå <stdio.h>
2  #include <ctype.h>

4  #define TRUE		1
5  #define FALSE        0

7  #define NL		'\n'
8  #define CR		'\r'
9  #define SOFT_HYPHEN	0x1f	/* inserted to break words at eol */

11  #define TABSIZE	8

13  #define CONTROL(x)	((x)-'a')
14  #define BOLD	(CONTROL('b'))
15  #define SUPERSCRIPT	(CONTROL('t'))
16  #define SUBSCRIPT	(CONTROL('v'))
17  #define UNDERSCORE	(CONTROL('s'))

19  #define BOLD_BEGIN		"<<<"
20  #define BOLD_END		">>>"
21  #define UNDERSCORE_BEGIN	"<_"
22  #define UNDERSCORE_END	"_>"
23  #define SUBSCRIPT_BEGIN	"<<"
24  #define SUBSCRIPT_END	">>"
25  #define SUPERSCRIPT_BEGIN	"<^"
26  #define SUPERSCRIPT_END	"^>"

28  typedef int boolean;

30  int column;		/* column to output next character */
31  int c;          	/* last character read */

33  boolean superscript = FALSE;/* if superscripting */
34  boolean subscript = FALSE;	/* if subscripting */
35  boolean bold = FALSE;	/* if emboldening */
36  boolean underscore = FALSE;	/* if underscoring */
37  boolean msb_was_set;	/* if char most significant bit set */
38  boolean soft_space = FALSE;	/* if between words and have seen */
39  				/* soft space, but no other spaces */
40  boolean soft_hyphen = FALSE;/* if in the middle of a word that */
41  				/* was hyphenated by ws */
42  boolean dotcmd = FALSE;	/* if in the middle of a dot cmd */
43  boolean wrap_soon = FALSE;	/* if should wrap as soon as we get */
44  				/* to the end of the current word */

46  int spacerun = 0;		/* number of spaces seen in a row */

48  main() {
49      while (TRUE) {
50          /* look at single characters */
51          msb_was_set = FALSE;
52          if (EOF == (c = getchar())) break;
53          if (!isascii(c)) {	/* is most sig. bit set? */
54              c = toascii(c);	/* turn of most sig. bit */Š55              msb_was_set = TRUE;
56          }
57          if (c == CR) continue; /* always followed by an NL */
58          if (c == NL) {
59              if (soft_hyphen) {
60                  wrap_soon = TRUE;
61              } else newline();
62          } else if (dotcmd) {
63              /* throw away chars if we are in a dot command */
64              continue;
65          } else if (c == ' ') {
66              /* ignore blanks with msb set - they are soft */
67              if (!msb_was_set) {    /* a real space */
68                  soft_space = FALSE;
69                  spacerun++;
70                  column++;
71                  if (wrap_soon) { /* wrap now! */
72                      newline();
73                      wrap_soon = FALSE;
74                  }
75              } else soft_space = TRUE;
76          } else if (c == SOFT_HYPHEN) {
77              /* ignore hyphens with msb set - they are soft */
78              soft_hyphen = TRUE;
79          } else if (c == '.' && column == 0) {
80              /* text processing directive */
81              dotcmd = TRUE;
82          } else if (iswscntrl(c)) {
83              /* ignore wordstar print control characters */
84              /* e.g. ^S (underscore), ^B (bold) */
85              wscntrl(c);
86          } else if (iscntrl(c)) {
87              /* unknown control character - ignore */
88              continue;
89          } else { /* normal character */
90              /* if we encountered a soft space, stick in at */
91              /* least one space */
92              if (soft_space) {
93                  spacerun = 1;
94                  column++;
95                  soft_space = FALSE;
96              }
97              if (spacerun) { /* beginning of word */
98                  /* calculate tabs and blanks to lay down */
99          space_out(column-spacerun,column);
100                  spacerun = 0;
101              }
102              putchar(c);
103              column++;
104          }
105      }
106  }

108  /* print least number of spaces & tabs to move from "oldpos" */
109  /* to "newpos" */Š110  space_out(oldpos,curpos)
111  int oldpos;    /* old position */
112  int newpos;    /* new position */
113  {
114      int spaces, tabs;   /* number of spaces & tabs to print */
115      int i;

117      if (oldpos >= newpos) return;    /* no space in between */

119      /* first calculate tabs */
120      tabs = newpos/TABSIZE - oldpos/TABSIZE;

122      /* now calulate spaces */
123      /* if old & new at same tab stop, its just difference */
124      if (tabs == 0) spaces = newpos - oldpos;
125      /* if not, then its remainder from nearest tab stop */
126      else spaces = newpos % TABSIZE;

128      for (i=0;i<tabs;i++) putchar('\t');
129      for (i=0;i<spaces;i++) putchar(' ');
130  }

132  /* true if wordstar control character */
133  boolean
134  iswscntrl(c)
135  int c;
136  {
137      if ((c == BOLD) ||
138         (c == UNDERSCORE) ||
139         (c == SUPERSCRIPT) ||
140         (c == SUBSCRIPT)) return(TRUE);
141      else return(FALSE);
142  }

144  newline()
145  {
146      if (!dotcmd) putchar('\n');
147      column = 0;
148      spacerun = 0;
149      dotcmd = FALSE;
150  }

152  wscntrl(c)
153  char c;
154  {
155      switch (c) {    /* print control character */
156      case BOLD:
157          if (bold) printf(BOLD_END);
158          else printf(BOLD_BEGIN);
159          bold = !bold;
160          break;
161      case UNDERSCORE:
162          if (underscore) printf(UNDERSCORE_END);
163          else printf(UNDERSCORE_BEGIN);
164          underscore = !underscore;Š165          break;
166      case SUBSCRIPT:
167          if (subscript) printf(SUBSCRIPT_END);
168          else printf(SUBSCRIPT_BEGIN);
169          subscript = !subscript;
170          break;
171      case SUPERSCRIPT:
172          if (superscript) printf(SUPERSCRIPT_END);
173          else printf(SUPERSCRIPT_BEGIN);
174          superscript = !superscript;
175          break;
176      default:
177          /* unknown - ignore for now */
178          break;
179      }
180  }
