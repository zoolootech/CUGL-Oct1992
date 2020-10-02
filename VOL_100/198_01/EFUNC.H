/*	EFUNC.H:	MicroEMACS function declarations and names

		This file list all the C code functions used by MicroEMACS
	and the names to use to bind keys to them. To add functions,
	declare it here in both the extern function list and the name
	binding table.

*/

/*	External function declarations		*/

extern  int     ctrlg();                /* Abort out of things          */
extern  int     quit();                 /* Quit                         */
extern  int     ctlxlp();               /* Begin macro                  */
extern  int     ctlxrp();               /* End macro                    */
extern  int     ctlxe();                /* Execute macro                */
extern  int     fileread();             /* Get a file, read only        */
extern  int     filefind();		/* Get a file, read write       */
extern  int     filewrite();            /* Write a file                 */
extern  int     filesave();             /* Save current file            */
extern  int     filename();             /* Adjust file name             */
#if	ABACKUP
extern  int     filercover();		/* Get a saved file		*/
extern	int	fileback();		/* Autosaves the file		*/
#endif
extern  int     getccol();              /* Get current column           */
extern  int     gobol();		/* Move to start of line        */
extern  int     forwchar();             /* Move forward by characters   */
extern  int     goeol();		/* Move to end of line          */
extern  int     backchar();             /* Move backward by characters  */
extern  int     forwline();             /* Move forward by lines        */
extern  int     backline();             /* Move backward by lines       */
extern  int     forwpage();             /* Move forward by pages        */
extern  int     backpage();             /* Move backward by pages       */
extern  int     gobob();		/* Move to start of buffer      */
extern  int     goeob();		/* Move to end of buffer        */
extern  int     setfillcol();           /* Set fill column.             */
extern  int     setmark();              /* Set mark                     */
extern  int	remmark();              /* clear mark                   */
extern  int     swapmark();             /* Swap "." and mark            */
extern  int     forwsearch();           /* Search forward               */
extern  int     backsearch();           /* Search backwards             */
extern	int	sreplace();		/* search and replace		*/
extern	int	qreplace();		/* search and replace w/query	*/
extern  int     showcpos();             /* Show the cursor position     */
extern  int     nextwind();             /* Move to the next window      */
extern  int     prevwind();             /* Move to the previous window  */
extern  int     onlywind();             /* Make current window only one */
extern  int     splitwind();            /* Split current window         */
extern  int     mvdnwind();             /* Move window down             */
extern  int     mvupwind();             /* Move window up               */
extern  int     enlargewind();          /* Enlarge display window.      */
extern  int     shrinkwind();           /* Shrink window.               */
extern  int     listbuffers();          /* Display list of buffers      */
extern  int     usebuffer();            /* Switch a window to a buffer  */
extern  int     killbuffer();           /* Make a buffer go away.       */
extern  int     reposition();           /* Reposition window            */
extern  int     refresh();              /* Refresh the screen           */
extern  int     twiddle();              /* Twiddle characters           */
extern  int     tab();                  /* Insert tab                   */
extern  int     newline();              /* Insert CR-LF                 */
extern  int     indent();               /* Insert CR-LF, then indent    */
extern  int     openline();             /* Open up a blank line         */
extern  int     deblank();              /* Delete blank lines           */
extern  int     quote();                /* Insert literal               */
extern  int     backword();             /* Backup by words              */
extern  int     forwword();             /* Advance by words             */
extern  int     forwdel();              /* Forward delete               */
extern  int     backdel();              /* Backward delete              */
extern  int     killtext();             /* Kill forward                 */
extern  int     yank();                 /* Yank back from killbuffer.   */
extern  int     upperword();            /* Upper case word.             */
extern  int     lowerword();            /* Lower case word.             */
extern  int     upperregion();          /* Upper case region.           */
extern  int     lowerregion();          /* Lower case region.           */
extern  int     capword();              /* Initial capitalize word.     */
extern  int     delfword();             /* Delete forward word.         */
extern  int     delbword();             /* Delete backward word.        */
extern  int     killregion();           /* Kill region.                 */
extern  int     copyregion();           /* Copy region to kill buffer.  */
extern  int     spawncli();             /* Run CLI in a subjob.         */
extern  int     spawn();                /* Run a command in a subjob.   */
#if	BSD | VMS
extern	int	bktoshell();		/* suspend emacs to parent shell*/
#endif
#if	BSD
extern	int	rtfrmshell();		/* return from a suspended state*/
#endif
extern  int     quickexit();            /* low keystroke style exit.    */
extern	int	setmode();		/* set an editor mode		*/
extern	int	delmode();		/* delete a mode		*/
extern	int	gotoline();		/* go to a numbered line	*/
extern	int	namebuffer();		/* rename the current buffer	*/
#if	WORDPRO
extern	int	gobop();		/* go to beginning/paragraph	*/
extern	int	goeop();		/* go to end/paragraph		*/
extern	int	fillpara();		/* fill current paragraph	*/
#endif
extern	int	help();			/* get the help file here	*/
extern	int	deskey();		/* describe a key's binding	*/
extern	int	viewfile();		/* find a file in view mode	*/
extern	int	insfile();		/* insert a file		*/
extern	int	scrupnxt();		/* scroll next window back	*/
extern	int	scrdwnxt();		/* scroll next window down	*/
extern	int	bindtokey();		/* bind a function to a key	*/
extern	int	unbindkey();		/* unbind a key's function	*/
extern	int	namedcmd();		/* execute named command	*/
extern	int	desbind();		/* describe bindings		*/
extern	int	execcmd();		/* execute a command line	*/
extern	int	execbuf();		/* exec commands from a buffer	*/
extern	int	execfile();		/* exec commands from a file	*/
extern	int	nextbuffer();		/* switch to the next buffer	*/
extern	int	prevbuffer();		/* switch to the prev bufffer	*/
#if	WORDPRO
extern	int	killpara();		/* kill the current paragraph	*/
#endif
extern	int	setgmode();		/* set a global mode		*/
extern	int	delgmode();		/* delete a global mode		*/
extern	int	insspace();		/* insert a space forword	*/
extern	int	forwhunt();		/* hunt forward for next match	*/
extern	int	backhunt();		/* hunt backwards for next match*/
extern	int	pipecmd();		/* pipe command into buffer	*/
extern	int	filter();		/* filter buffer through dos	*/
extern	int	delwind();		/* delete the current window	*/
extern	int	cbuf1();		/* execute numbered comd buffer */
extern	int	cbuf2();
extern	int	cbuf3();
extern	int	cbuf4();
extern	int	cbuf5();
extern	int	cbuf6();
extern	int	cbuf7();
extern	int	cbuf8();
extern	int	cbuf9();
extern	int	cbuf10();
extern	int	cbuf11();
extern	int	cbuf12();
extern	int	cbuf13();
extern	int	cbuf14();
extern	int	cbuf15();
extern	int	cbuf16();
extern	int	cbuf17();
extern	int	cbuf18();
extern	int	cbuf19();
extern	int	cbuf20();
extern	int	cbuf21();
extern	int	cbuf22();
extern	int	cbuf23();
extern	int	cbuf24();
extern	int	cbuf25();
extern	int	cbuf26();
extern	int	cbuf27();
extern	int	cbuf28();
extern	int	cbuf29();
extern	int	cbuf30();
extern	int	cbuf31();
extern	int	cbuf32();
extern	int	cbuf33();
extern	int	cbuf34();
extern	int	cbuf35();
extern	int	cbuf36();
extern	int	cbuf37();
extern	int	cbuf38();
extern	int	cbuf39();
extern	int	cbuf40();
extern	int	storemac();		/* store text for macro		*/
extern	int	resize();		/* resize current window	*/
extern	int	clrmes();		/* clear the message line	*/
extern	int	meta();			/* meta prefix dummy function	*/
extern	int	cex();			/* ^X prefix dummy function	*/
extern	int	unarg();		/* ^U repeat arg dummy function	*/
extern	int	istring();		/* insert string in text	*/
extern	int	unmark();		/* unmark current buffer	*/
#if	ISRCH
extern	int	fisearch();		/* forward incremental search	*/
extern	int	risearch();		/* reverse incremental search	*/
#endif
#if	WORDPRO
extern	int	wordcount();		/* count words in region	*/
#endif
extern	int	savewnd();		/* save current window		*/
extern	int	restwnd();		/* restore current window	*/
extern	int	upscreen();		/* force screen update		*/
extern	int	writemsg();		/* write text on message line	*/
#if	FLABEL
extern	int	fnclabel();		/* set function key label	*/
#endif
#if	APROP
extern	int	apro();			/* apropos fuction		*/
#endif
#if	CRYPT
extern	int	setkey();		/* set encryption key		*/
#endif
extern	int	wrapword();		/* wordwrap function		*/
#if	CFENCE
extern	int	getfence();		/* move cursor to a matching fence */
#endif
extern	int	newsize();		/* change the current screen size */
extern	int	setvar();		/* set a variables value */
extern	int	newwidth();		/* change the current screen width */

extern	int	narrow();		/* narrow to region */
extern	int	widen();		/* widen from narrowed region */

#if	DEBUGM
extern	int	desfunc();		/* describe functions */
extern	int	desvars();		/* describe variables */
extern	int	dispvar();		/* display a variable */
#endif

#if	ACMODE
extern	int	acmode();		/* set into C mode for .c and .h */
#endif

#if	DECEDT
extern	int advbot();			/* goto top or bottom of buffer */
extern	int advchar();			/* advance character */
extern	int advhunt();			/* hunt */
extern	int advline();			/* advance line */
extern	int adveol();			/* advance end of line */
extern	int advpage();			/* advance page */
extern	int advsearch();		/* search */
extern	int advword();			/* advance word */
extern	int advadv();			/* set advance to go forward */
extern	int advback();			/* set advance to go backward */

extern	int nullcmd();			/* null enter command */
extern  int specins();			/* special insertion */
extern	int yankchar();			/* Undelete last deleted char */
extern	int caseregion();		/* change case */
extern	int regionwrite();		/* write a region */
extern	int cutregion();		/* Kill region, remove mark */
extern  int apkillregion();             /* Kill region, add to kill buffer */
extern  int apcopyregion();             /* Append region to kill buffer */
extern	int special();			/* special prefix dummy function */
extern	int octalins();			/* Octal insertion */
extern	int filerevert();		/* Restore last saved version	*/
extern	int filewfind();		/* Find file into other window */
extern	int showslen();			/* Show string length */
extern	int delfline();			/* Delete forward line */
extern	int setovrwrt();		/* toggle overwrite mode */
extern	int scdnwind();			/* smooth scroll cursor down */
extern	int scupwind();			/* smooth scroll cursor up */
extern	int scnxpage();			/* smooth scroll next page */
extern	int scprpage();			/* smooth scroll previous page */
#endif

#if	AEDIT
extern	int trim();			/* trim whitespace from end of line */
extern	int detab();			/* detab rest of line */
extern	int entab();			/* entab rest of line */

extern	int scrlforw();			/* scroll forward */
extern	int scrlback();			/* scroll backward */
#endif

#if	PROC
extern	int storeproc();		/* store names procedure */
extern	int execproc();			/* execute procedure */
#endif

#if	MENUS
extern	int menu();			/* enter menu area */
#endif

extern	int nullproc();			/* does nothing... */

/*	Name to function binding table

		This table gives the names of all the bindable functions
	end their C function address. These are used for the bind-to-key
	function.
*/

NBIND	names[] = {
	{"abort-command",		ctrlg},
	{"add-global-mode",		setgmode},
	{"add-mode",			setmode},
#if	DECEDT
	{"adv-bottom",			advbot},
	{"adv-character",		advchar},
	{"adv-end-of-line",		adveol},
	{"adv-hunt",			advhunt},
	{"adv-line",			advline},
	{"adv-page",			advpage},
	{"adv-search",			advsearch},
	{"adv-word",			advword},
	{"advance",			advadv},
	{"append-and-copy-region",	apcopyregion},
	{"append-and-kill-region",	apkillregion},
#endif
#if	APROP
	{"apropos",			apro},
#endif
#if	ACMODE
	{"auto-mode",			acmode},
#endif
#if	ABACKUP
	{"auto-save-file",		fileback},
#endif
#if	DECEDT
	{"backup",			advback},
#endif
	{"backward-character",		backchar},
	{"begin-macro",			ctlxlp},
	{"beginning-of-file",		gobob},
	{"beginning-of-line",		gobol},
	{"bind-to-key",			bindtokey},
	{"buffer-position",		showcpos},
#if	DECEDT
	{"case-region-change",		caseregion},
#endif
	{"case-region-lower",		lowerregion},
	{"case-region-upper",		upperregion},
	{"case-word-capitalize",	capword},
	{"case-word-lower",		lowerword},
	{"case-word-upper",		upperword},
	{"change-file-name",		filename},
	{"change-screen-size",		newsize},
	{"change-screen-width",		newwidth},
#if	DECEDT
	{"char-to-octal-insert",	octalins},
#endif
	{"clear-and-redraw",		refresh},
#if	DECEDT
	{"clear-mark",			remmark},
#endif
	{"clear-message-line",		clrmes},
	{"copy-region",			copyregion},
#if	WORDPRO
	{"count-words",			wordcount},
#endif
#if	DECEDT
	{"cut-region",			cutregion},
#endif
	{"ctlx-prefix",			cex},
	{"delete-blank-lines",		deblank},
	{"delete-buffer",		killbuffer},
	{"delete-global-mode",		delgmode},
	{"delete-mode",			delmode},
	{"delete-next-character",	forwdel},
#if	DECEDT
	{"delete-next-line",		delfline},
#endif
	{"delete-next-word",		delfword},
	{"delete-other-windows",	onlywind},
	{"delete-previous-character",	backdel},
	{"delete-previous-word",	delbword},
#if	DECEDT
	{"delete-to-kill-buffer",	killregion},
#endif
	{"delete-window",		delwind},
	{"describe-bindings",		desbind},
#if	DEBUGM
	{"describe-functions",		desfunc},
#endif
	{"describe-key",		deskey},
#if	DEBUGM
	{"describe-variables",		desvars},
#endif
#if	AEDIT
	{"detab-line",			detab},
#endif
#if	DEBUGM
	{"display",			dispvar},
#endif
	{"end-macro",			ctlxrp},
	{"end-of-file",			goeob},
	{"end-of-line",			goeol},
#if	AEDIT
	{"entab-line",			entab},
#endif
#if	DECEDT
	{"enter",			nullcmd},
#endif
	{"exchange-point-and-mark",	swapmark},
	{"execute-buffer",		execbuf},
	{"execute-command-line",	execcmd},
	{"execute-file",		execfile},
	{"execute-macro",		ctlxe},
	{"execute-macro-1",		cbuf1},
	{"execute-macro-2",		cbuf2},
	{"execute-macro-3",		cbuf3},
	{"execute-macro-4",		cbuf4},
	{"execute-macro-5",		cbuf5},
	{"execute-macro-6",		cbuf6},
	{"execute-macro-7",		cbuf7},
	{"execute-macro-8",		cbuf8},
	{"execute-macro-9",		cbuf9},
	{"execute-macro-10",		cbuf10},
	{"execute-macro-11",		cbuf11},
	{"execute-macro-12",		cbuf12},
	{"execute-macro-13",		cbuf13},
	{"execute-macro-14",		cbuf14},
	{"execute-macro-15",		cbuf15},
	{"execute-macro-16",		cbuf16},
	{"execute-macro-17",		cbuf17},
	{"execute-macro-18",		cbuf18},
	{"execute-macro-19",		cbuf19},
	{"execute-macro-20",		cbuf20},
	{"execute-macro-21",		cbuf21},
	{"execute-macro-22",		cbuf22},
	{"execute-macro-23",		cbuf23},
	{"execute-macro-24",		cbuf24},
	{"execute-macro-25",		cbuf25},
	{"execute-macro-26",		cbuf26},
	{"execute-macro-27",		cbuf27},
	{"execute-macro-28",		cbuf28},
	{"execute-macro-29",		cbuf29},
	{"execute-macro-30",		cbuf30},
	{"execute-macro-31",		cbuf31},
	{"execute-macro-32",		cbuf32},
	{"execute-macro-33",		cbuf33},
	{"execute-macro-34",		cbuf34},
	{"execute-macro-35",		cbuf35},
	{"execute-macro-36",		cbuf36},
	{"execute-macro-37",		cbuf37},
	{"execute-macro-38",		cbuf38},
	{"execute-macro-39",		cbuf39},
	{"execute-macro-40",		cbuf40},
	{"execute-named-command",	namedcmd},
#if	PROC
	{"execute-procedure",		execproc},
#endif
#if	DECEDT
	{"exit",			quickexit},
#endif
	{"exit-emacs",			quit},
#if	WORDPRO
	{"fill-paragraph",		fillpara},
#endif
	{"filter-buffer",		filter},
	{"find-file",			filefind},
#if	DECEDT
	{"find-file-into-other-window",	filewfind},
#endif
	{"forward-character",		forwchar},
	{"goto-line",			gotoline},
#if	CFENCE
	{"goto-matching-fence",		getfence},
#endif
	{"grow-window",			enlargewind},
	{"handle-tab",			tab},
	{"hunt-forward",		forwhunt},
	{"hunt-backward",		backhunt},
	{"help",			help},
	{"i-shell",			spawncli},
#if	DECEDT
	{"include",			insfile},
#endif
#if	ISRCH
	{"incremental-search",		fisearch},
#endif
	{"insert-file",			insfile},
	{"insert-space",		insspace},
	{"insert-string",		istring},
#if	WORDPRO & DECEDT
	{"justify-paragraph",		fillpara},
#endif
#if	WORDPRO
	{"kill-paragraph",		killpara},
#endif
	{"kill-region",			killregion},
	{"kill-to-end-of-line",		killtext},
#if	FLABEL
	{"label-function-key",		fnclabel},
#endif
	{"list-buffers",		listbuffers},
#if	MENUS
	{"menu",			menu},
#endif
	{"meta-prefix",			meta},
	{"move-window-down",		mvdnwind},
	{"move-window-up",		mvupwind},
	{"name-buffer",			namebuffer},
	{"narrow-to-region",		narrow},
	{"newline",			newline},
	{"newline-and-indent",		indent},
	{"next-buffer",			nextbuffer},
	{"next-line",			forwline},
	{"next-page",			forwpage},
#if	WORDPRO
	{"next-paragraph",		goeop},
#endif
	{"next-window",			nextwind},
	{"next-word",			forwword},
	{"nop",				nullproc},
	{"open-line",			openline},
#if	DECEDT
	{"over-write",			setovrwrt},
#endif
	{"pipe-command",		pipecmd},
	{"previous-buffer",		prevbuffer},
	{"previous-line",		backline},
	{"previous-page",		backpage},
#if	WORDPRO
	{"previous-paragraph",		gobop},
#endif
	{"previous-window",		prevwind},
	{"previous-word",		backword},
	{"query-replace-string",	qreplace},
	{"quick-exit",			quickexit},
#if	DECEDT
	{"quit",			quit},
#endif
	{"quote-character",		quote},
	{"read-file",			fileread},
#if	ABACKUP
	{"recover-file",		filercover},
#endif
	{"redraw-display",		reposition},
	{"remove-mark",			remmark},
	{"resize-window",		resize},
	{"restore-window",		restwnd},
	{"replace-string",		sreplace},
#if	ISRCH
	{"reverse-incremental-search",	risearch},
#endif
#if	DECEDT
	{"revert-file",			filerevert},
#endif
#if	PROC
	{"run",				execproc},
#endif
	{"save-file",			filesave},
	{"save-window",			savewnd},
#if	AEDIT
	{"scroll-backward",		scrlback},
	{"scroll-forward",		scrlforw},
#endif
	{"scroll-next-down",		scrdwnxt},
	{"scroll-next-up",		scrupnxt},
	{"search-forward",		forwsearch},
	{"search-reverse",		backsearch},
	{"select-buffer",		usebuffer},
	{"set",				setvar},
#if	CRYPT
	{"set-encryption-key",		setkey},
#endif
	{"set-fill-column",		setfillcol},
	{"set-mark",			setmark},
#if	DECEDT
	{"set-screen-width",		newwidth},
#endif
	{"shell-command",		spawn},
	{"shrink-window",		shrinkwind},
#if	DECEDT
	{"smooth-next-line",		scdnwind},
	{"smooth-next-page",		scnxpage},
	{"smooth-previous-line",	scupwind},
	{"smooth-previous-page",	scprpage},
	{"source",			execfile},
	{"special-insertion",		specins},
	{"special-prefix", 		special},
#endif
	{"split-current-window",	splitwind},
	{"store-macro",			storemac},
#if	PROC
	{"store-procedure",		storeproc},
#endif
#if	DECEDT
	{"string-length",		showslen},
#endif
#if	BSD | VMS
	{"suspend-emacs",		bktoshell},
#endif
	{"transpose-characters",	twiddle},
#if	AEDIT
	{"trim-line",			trim},
#endif
	{"unbind-key",			unbindkey},
	{"universal-argument",		unarg},
	{"unmark-buffer",		unmark},
	{"update-screen",		upscreen},
	{"view-file",			viewfile},
	{"widen-from-region",		widen},
	{"wrap-word",			wrapword},
	{"write-file",			filewrite},
	{"write-message",		writemsg},
#if	DECEDT
	{"write-region",		regionwrite},
#endif
	{"yank",			yank},
#if	DECEDT
	{"yank-character",		yankchar},
#endif

	{"",			NULL}
};

