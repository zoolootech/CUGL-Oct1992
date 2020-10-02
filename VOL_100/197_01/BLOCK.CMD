;	BLOCK.CMD:	Rectangualar Cut/Paste macros

;	delete a rectangular block of text

store-procedure getblock
	;set up needed variables
	set $discmd FALSE
	delete-buffer "[block]"
	set %rcbuf $cbufname
	set %cline $cwline

	;save block boundries
	set %endpos $curcol
	set %endline $curline
	exchange-point-and-mark
	set %begpos $curcol
	set %begline $curline
	set %blwidth &sub %endpos %begpos

	;detab the region
	&add &sub %endline %begline 1 detab-line

	;scan through the block
	set $curline %begline
	!while &less $curline &add %endline 1
		;grab the part of this line needed
		!force set $curcol %begpos
		set-mark
		!force set $curcol %endpos
		kill-region

		;bring it back if this is just a copy
		!if %bkcopy
			yank
		!endif

		;put the line in the block buffer
		select-buffer "[block]"
		yank

		;and pad it if needed
		!if &less $curcol %blwidth
			&sub %blwidth $curcol insert-space
			end-of-line
		!endif
		forward-character

		;onward...
		select-buffer %rcbuf
		next-line
	!endwhile

        ;unmark the block
        select-buffer "[block]"
        unmark-buffer
        select-buffer %rcbuf
        previous-line
        %cline redraw-display
	set $discmd TRUE
!endm

;	insert/overlay a rectangular block of text

store-procedure putblock
	;set up needed variables
	set $discmd FALSE
	set %rcbuf $cbufname
	set %cline $cwline

	;save block boundries
	set %begpos $curcol
	set %begline $curline

	;scan through the block
	select-buffer "[block]"
	beginning-of-file
	set %endpos &add %begpos $lwidth
	!while &not &equ $lwidth 0

		;pad the destination if it is needed
		select-buffer %rcbuf
		beginning-of-line
		!if &not &equ $lwidth 0
			detab-line
			previous-line
		!endif
		!force set $curcol %begpos
		!if &less $curcol %begpos
			&sub %begpos $curcol insert-space
			end-of-line
		!endif

		;delete some stuff if this should overlay
		!if %bkcopy
			set-mark
			!force set $curcol %endpos
			kill-region
		!endif

		;grab the line from the block buffer
		select-buffer "[block]"
		beginning-of-line
		set-mark
		end-of-line
		copy-region
		forward-character

		;put the line in the destination position
		select-buffer %rcbuf
		yank
		next-line

		;onward...
		select-buffer "[block]"
	!endwhile

	select-buffer %rcbuf
	set $curline %begline
	set $curcol %begpos
	%cline redraw-display
	set $discmd TRUE
!endm


