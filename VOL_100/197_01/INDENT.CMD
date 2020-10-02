;	INDENT.CMD:	In/undent region & reformat indented paragraph macros

;	indent region

store-procedure	rcindent
	write-message "[Indenting region]"
	set %endline $curline
	set %endpos $cwline
	exchange-point-and-mark
	set $discmd FALSE
	set-mark
	set $discmd TRUE

	!while &gre %endline $curline
		beginning-of-line
		handle-tab
		next-line
	!endwhile

	beginning-of-line	
	set $cwline %endpos
	write-message "[Region indented]"
!endm

;	undent region

store-procedure	rcundent
	write-message "[Undenting region]"
	set %endline $curline
	set %endpos $cwline
	exchange-point-and-mark
	set $discmd FALSE
	set-mark
	set $discmd TRUE

	!while &gre %endline $curline
		beginning-of-line
		!if &gre $lwidth 0
			delete-next-character
		!endif
		next-line
	!endwhile

	beginning-of-line	
	set $cwline %endpos
	write-message "[Region undented]"
!endm

;	reformat indented paragraph

store-procedure refin
	write-message "                    [Fixing paragraph]"
	set %endline $curline
	set $discmd "FALSE"
	kill-region
	2 split-current-window
	select-buffer "[temp]"
	yank

	end-of-file
	set %endline $curline
	beginning-of-file
	!while &gre %endline $curline
		beginning-of-line
		!if &gre $lwidth 0
			delete-next-character
		!endif
		next-line
	!endwhile

	&sub $fillcol 8 set-fill-column
	1 goto-line
	fill-paragraph
	&add $fillcol 8 set-fill-column

	end-of-file
	set %endline $curline
	beginning-of-file
	!while &gre %endline $curline
		beginning-of-line
		handle-tab
		next-line
	!endwhile

	beginning-of-file
	set-mark
	end-of-file
	kill-region
	unmark-buffer
	delete-window
	yank
	delete-buffer "[temp]"
	set $cwline %endpos
	set $discmd "TRUE"
	write-message "                    [Fixed paragraph]"
!endm


