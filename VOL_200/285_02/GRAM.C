/*
**	file:		gram.c
**	purpose:	Allocate input grammar variables
**	mods:		1989.12.30 - Roberto Artigas Jr
**			Added DEFAULT initialization
**
Copyright (C) 1984, 1986 Bob Corbett and Free Software Foundation, Inc.

BISON is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the BISON General Public License for full details.

Everyone is granted permission to copy, modify and redistribute BISON,
but only under the conditions described in the BISON General Public
License.  A copy of this license is supposed to have been given to you
along with BISON so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!
*/

/* comments for these variables are in gram.h  */

#include <stdio.h>

int nitems = 0;
int nrules = 0;
int nsyms = 0;
int ntokens = 0;
int nvars = 0;

short *ritem = NULL;
short *rlhs = NULL;
short *rrhs = NULL;
short *rprec = NULL;
short *sprec = NULL;
short *rassoc = NULL;
short *sassoc = NULL;
short *token_translations = NULL;
short *rline = NULL;

int start_symbol = 0;

int translations = 0;

int max_user_token_number = 0;

int semantic_parser = 0;

int pure_parser = 0;

int error_token_number = 0;
