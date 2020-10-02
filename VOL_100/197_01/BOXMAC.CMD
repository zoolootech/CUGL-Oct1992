;	BOXMAC.CMD:	Standard Micro Startup Box drawing macroes
;			for MicroEMACS 3.9
;			(C)opyright 1987 by Suresh Konda and Daniel M Lawrence
;			Last Update: 07/12/87

write-message "[Loading Box Macroes]"

;this macro inserts enough characters at the end of line to lineup with point
store-procedure mvtopcol
;set $debug TRUE
	end-of-line
	set %temp &sub %pcol $curcol
	!if &less $curcol %pcol
	;;	current position to left of point -- blank fill to point
	;	!if &gre %temp 0
			%temp	insert-string " "
	;	!endif
	!else
		!if &less %temp 0
			set %temp &neg %temp
			%temp backward-character
		!else
			%temp forward-character
		!endif
	!endif
!endm
;this macro inserts enough characters at the end of line to lineup with mark
store-procedure mvtomcol
;set $debug TRUE
	end-of-line
	!if &less %mcol $curcol
	;;	current position to right of mark -- move to mark
;		insert-string &cat &cat %mcol " " $curcol
		beginning-of-line
		%mcol forward-character
	!else
	;;      current position is to left of mark -- blank fill
		set %temp &sub %mcol $curcol
		!if &gre %temp 0
			%temp insert-string " "
		!endif
	!endif
!endm

store-procedure inschar
	!if &equal %char 205
		insert-string &chr %c1
	!else
		!if &equal %char 196
			insert-string &chr %c2
		!else
			insert-string &chr %c3
		!endif
	!endif
!endm
store-procedure box2
;remember point
	set %pcol &add $curcol 1
	set %pline $curline
	exchange-point-and-mark
;remember mark
;	set %mcol &add $curcol 1
	set %mcol $curcol
	set %mline $curline
;draw top horizontal line
	insert-string "É"
	set %width &sub &sub %pcol %mcol 1
	%width insert-string "Í"
 	insert-string "»"
	newline-and-indent
;	insert-string " "
;draw bottom horizontal line
	%pline goto-line
; we are now one line above old last line because of insertion of top line
	next-line
	end-of-line
	newline
	run mvtomcol
	insert-string "È"
	%width insert-string "Í"
	insert-string "¼"
; bump pline 
set %pline &add %pline 1
;draw verticals
	%mline goto-line
;we are at top -- draw verticals
*lp1
	next-line
	run mvtomcol
	insert-string "º"
	run mvtopcol
	insert-string "º"
	!if &less $curline %pline
		!goto lp1
	!endif
;return to point
	%pline goto-line
	next-line
	beginning-of-line
	%width forward-character
	2 forward-character
!endm
store-procedure setpoints
;; procedure will set pcol, pline, mcol and mline
set %pcol $curcol
set %pline $curline
exchange-point-and-mark
set %mcol $curcol
set %mline $curline
exchange-point-and-mark
!endm

;; user procedure to draw a double line from mark to point making spaces for
;; the characters.
store-procedure line2
run setpoints
!if &equal %pcol %mcol
	run vert2
!else
	!if &equal %pline %mline
		run hor2
	!else
		write-message "Illegal point and mark for lines"
	!endif
!endif
!endm

;; user procedure to draw line from mark to point making spaces for
;; the characters.
store-procedure line1
run setpoints
!if &equal %pcol %mcol
	run vert1
!else
	!if &equal %pline %mline
		run hor1
	!else
		write-message "Illegal point and mark for lines"
	!endif
!endif
!endm

store-procedure hor2
;; procedure to draw a double line from beginning of line to point
;; assume that the current line is to be double underlined.  pcol,mcol,pline,
;; mline already set by calling macro
!if &equal %mcol %pcol
	write-message "in hor equal cols"
	!return
!endif
!if &less %pcol %mcol
;	then point was to left of mark.  exchange and reset variables
	exchange-point-and-mark
	run setpoints
!endif
end-of-line
newline
;; move to under mark
!if &greater %mcol 1
	%mcol insert-string " "
!endif
;; see if first char is a vertical line
previous-line
set %char  $curchar
next-line
!if &equ %char 186
		insert-string "Ì"
!else
	!if &equ %char 179 
		insert-string "Æ"
	!else
		insert-string "Í"
	!endif
!endif

; now for all chars but the last character i.e., char at point
*lp1
	previous-line
	set %char  $curchar
	next-line
	!if &equ %char 186
		insert-string "Î"
        !else 
		!if &equ %char 179
			insert-string "Ø"
		!else
			insert-string "Í"
		!endif
	!endif
	!if &less $curcol %pcol
		!goto lp1
	!endif
;; see if last char is a vertical line
previous-line
set %char  $curchar
next-line
!if &equ %char 186
		insert-string "¹"
!else
	!if &equ %char 179 
		insert-string "µ"
	!else
		insert-string "Í"
	!endif
!endif
!endm

store-procedure hor1
;; procedure to draw a single line from beginning of line to point
!if &equal %mcol %pcol
	write-message "in hor equal cols"
	!return
!endif
!if &less %pcol %mcol
;	then point was to left of mark.  exchange and reset variables
	exchange-point-and-mark
	run setpoints
!endif
end-of-line
newline
;; move to under mark
!if &greater %mcol 1
	%mcol insert-string " "
!endif
;; see if first char is a vertical line
previous-line
set %char  $curchar
next-line
!if &equ %char 186
		insert-string "Ç"
!else
	!if &equ %char 179 
		insert-string "Ã"
	!else
		insert-string "Ä"
	!endif
!endif

; now for all chars but the last character i.e., char at point
*lp1
	previous-line
	set %char  $curchar
	next-line
	!if &equ %char 186
		insert-string "×"
        !else 
		!if &equ %char 179
			insert-string "Å"
		!else
			insert-string "Ä"
		!endif
	!endif
	!if &less $curcol %pcol
		!goto lp1
	!endif
;; see if last char is a vertical line
previous-line
set %char  $curchar
next-line
!if &equ %char 186
		insert-string "¶"
!else
	!if &equ %char 179 
		insert-string "´"
	!else
		insert-string "Ä"
	!endif
!endif
!endm

store-procedure vert2
;; procedure to draw a line from mark to point.  mark should be above point
;; this will insert a column of double lines
!if &equal %mline %pline
	!return
!endif
!if &less %pline %mline
;	then point was above mark.  exchange and reset variables
	exchange-point-and-mark
	run setpoints
!endif
;top line
%mline goto-line
beginning-of-line
run mvtopcol
backward-character
set %char $curchar
forward-character

!if &equal %char 205
	insert-string "Ë"
!else
	!if &equal %char 196
		insert-string "Ò"
	!else
		insert-string "º"
	!endif
!endif
;all but pline
*lp1
	next-line
	beginning-of-line
	run mvtopcol
	backward-character
	set %char $curchar
	forward-character
	!if &equal %char 205
		insert-string "Î"
	!else
		!if &equal %char 196
			insert-string "×"
		!else
			insert-string "º"
		!endif
	!endif
	!if &less $curline &sub %pline 1
		!goto lp1
	!endif
; bottom line
next-line
beginning-of-line
run mvtopcol
backward-character
set %char $curchar
forward-character
!if &equal %char 205
	insert-string "Ê"
!else
	!if &equal %char 196
		insert-string "Ð"
	!else
		insert-string "º"
	!endif
!endif
!if &less $curcol &sub %pcol 1
	!goto lp1
!endif
!endm

store-procedure vert1
;; procedure to draw a line from mark to point.  mark should be above point
;; this will insert a column of double lines
!if &equal %mline %pline
	!return
!endif
!if &less %pline %mline
;	then point was above mark.  exchange and reset variables
	exchange-point-and-mark
	run setpoints
!endif
;top line
%mline goto-line
beginning-of-line
run mvtopcol
backward-character
set %char $curchar
forward-character

!if &equal %char 205
	insert-string "Ñ"
!else
	!if &equal %char 196
		insert-string "Â"
	!else
		insert-string "³"
	!endif
!endif
;all but pline
*lp1
	next-line
	beginning-of-line
	run mvtopcol
	backward-character
	set %char $curchar
	forward-character
	!if &equal %char 205
		insert-string "Ø"
	!else
		!if &equal %char 196
			insert-string "Å"
		!else
			insert-string "³"
		!endif
	!endif
	!if &less $curline &sub %pline 1
		!goto lp1
	!endif
; bottom line
next-line
beginning-of-line
run mvtopcol
backward-character
set %char $curchar
forward-character
!if &equal %char 205
	insert-string "Ï"
!else
	!if &equal %char 196
		insert-string "Á"
	!else
		insert-string "³"
	!endif
!endif
!if &less $curcol &sub %pcol 1
	!goto lp1
!endif
!endm


;; user procedure to insert blanks from mark to point making spaces for
store-procedure blank
run setpoints
!if &equal %pcol %mcol
	run vblank
!else
	!if &equal %pline %mline
		run hblank
	!else
		write-message "Illegal point and mark for blanking"
	!endif
!endif
!endm

store-procedure chkh2
;; procedure to check if the horizontal blanking routine should insert a
;; double vertical line.  Sets a global variable yes to true if yes
	set %yes &greater &sindex "¶·¹º»ÇÉËÌÎÒÖ×" %char 1
!endm

store-procedure chkh1
;; procedure to check if the horizontal blanking routine should insert a
;; single vertical line.  Sets a global variable yes to true if yes
	!if &greater &sindex %temp "³´µ¸¿ÂÅÆÕØÚÑ" 1
		set %yes TRUE
	!else
		set %yes FALSE
!endm

store-procedure hblank
;; procedure to insert blanks horizontally from mark to point
;; assume that the current line is to be double underlined.  pcol,mcol,pline,
;; mline already set by calling macro
!if &equal %mcol %pcol
	write-message "NULL Space to Fill"
	!return
!endif
!if &less %pcol %mcol
;	then point was to left of mark.  exchange and reset variables
	exchange-point-and-mark
	run setpoints
!endif
end-of-line
newline
;; move to under mark
!if &greater %mcol 1
	%mcol insert-string " "
!endif
;; increment %pcol for loop counter
set %pcol &add %pcol 1
;; loop through to point
*lp1
	previous-line
	set %char &chr $curchar
	next-line
	execute-procedure chkh2
	!if %yes
		insert-string "º"
        !else 
		execute-procedure chkh1
		!if %yes
			insert-string "³"
		!else
			insert-string " "
		!endif
	!endif
	!if &less $curcol %pcol
		!goto lp1
	!endif
!endm

store-procedure chkv2
;; procedure to check if the vertical blanking routine should insert a
;; double horizontal line.  Sets a global variable yes to true if yes
	set %temp 1
*lp1  
	!if &seq &mid "ÆÈÉÊËÌÍÎÏÑÔÕØ" %temp 1 %char
		set %yes TRUE
		!return
	!else
		set %temp &add %temp 1
;	check if %temp is >= 1+ length of check string
		!if &gre %temp 14
			set %yes FALSE
			!return
		!endif
		!goto lp1
	!endif
!endm


store-procedure chkv1
;; procedure to check if the vertical blanking routine should insert a
;; single horizontal line.  Sets a global variable yes to true if yes
	set %temp 1
*lp1  
	!if &seq &mid "ÀÁÂÃÄÅÇÐÒÓÖ×" %temp 1 %char
		set %yes TRUE
		!return
	!else
		set %temp &add %temp 1
;	check if %temp is >= 1+ length of check string
		!if &gre %temp 13
			set %yes FALSE
			!return
		!endif
		!goto lp1
	!endif
!endm


store-procedure vblank
;; procedure to vertical blanks from mark to point.  mark should be above point
!if &equal %mline %pline
	!return
!endif
!if &less %pline %mline
;	then point was above mark.  exchange and reset variables
	exchange-point-and-mark
	run setpoints
!endif
;top line
%mline goto-line
beginning-of-line
run mvtopcol
backward-character
set %char $curchar
forward-character

!if &equal %char 205
	insert-string "Í"
!else
	!if &equal %char 196
		insert-string "Ä"
	!else
		insert-string " "
	!endif
!endif
;all but pline
*lp1
	next-line
	beginning-of-line
	run mvtopcol
	backward-character
	set %char $curchar
	forward-character
	!if &equal %char 205
		insert-string "Í"
	!else
		!if &equal %char 196
			insert-string "Ä"
		!else
			insert-string " "
		!endif
	!endif
	!if &less $curline &sub %pline 1
		!goto lp1
	!endif
; bottom line
!if &equal $curline %pline
	!return
!endif
next-line
beginning-of-line
run mvtopcol
backward-character
set %char $curchar
forward-character
!if &equal %char 205
	insert-string "Í"
!else
	!if &equal %char 196
		insert-string "Ä"
	!else
		insert-string " "
	!endif
!endif
!endm

clear-message-line
