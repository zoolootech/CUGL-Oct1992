!---------------------------------------------------------------------------
! File SSANDERS.682 : RAP SS file for the Sanders 12/7 Printer Serial 0682
!---------------------------------------------------------------------------

!---------------------------------------------------------------------------
!					COMMANDS TO RAP
!		(This section MUST be the first in the SS file)
!---------------------------------------------------------------------------

!.ou 8
.ou 4

!---------------------------------------------------------------------------
!				  HARDWARE INFORMATION
!---------------------------------------------------------------------------

! CCS 2710 PORT 1 OF 4

!baud
!CCS PORT 1 AT 2400 BAUD
!3, 131
!1, 0
!0, 48
!3, 26

!i/o port
!0, 5, 1, 32, 0, 0, 1

!---------------------------------------------------------------------------
!			   USERS PERSONAL SUBSTITUTIONS
!---------------------------------------------------------------------------

{six{
.fn 6
.rm 115
.ju 2
.fi
.in 0

{two{
.fn 2
.rm 85
.ju 2
.fi
.in 0
~SF2~~LS15,15~~WS40,75,150~

{zero{
.fn 0
.rm 78
.nj
.fi
.in 0

{LH{
.nf
.ce 3
}font2}_
Department of Microbiology
University of Massachusetts
Amherst, MA 01003
.rf
.fi

{cross{
}_-18}+}_2}|}_16}

}_-18}
~VF-18~

}_2}
~VF2~

}_16}
~VF16~

{2cross{
}_-25}+}_18<-84}+}_7}

}_-25}
~VF-25~

}_18<-84}
~VF18~~FE-84~

}_7}
~VF7~

{2bar{
}_-20<-20}|}<-60}|}_20}

}_-20<-20}
~VF-20~~FE-20~

}<-60}
~FE-60~

}_20}
~VF20~

!GREEK LETTERS

{alpha{
}font3}a}RF}

{beta{
}font3}b}RF}

{gamma{
}font3}c}RF}

{kappa{
}font3}k}RF}

{lambda{
}font3}l}RF}

{mu{
}font3}m}RF}

{PHI{
}font3}V}RF}

!------------------------

{cr{
}u1}51}d2}Cr

!---------------------------------------------------------------------------
!			    SUBSCRIPTS AND SUPERSCRIPTS
!---------------------------------------------------------------------------

}d1}! lead in to subscript
}font6}}d}! NO SPACE ALLOWED

}u2}! finish subscript
}u}}RF}

}u1}! lead in to superscript
}font6}}u}! NO SPACE ALLOWED

}d2}! finish superscript
}d}}RF}

!---------------------------------------------------------------------------
!				 BUILT-IN SUBSTITUTIONS
!---------------------------------------------------------------------------

!INITIALIZATION

! !tm0!!bm0! defeats auto top and bottom margins on new Sanders OS
! !lm1500! set margin to 1.5
! !ru1! drop ribbon usage
}init}
!!CD~!!~tm0~~bm0~~fw9500~~lm1500~~rm1000~

}deinit}
~CD!!~!!RS!!

!BOLDFACE

}b+}
~BB6~

}b-}
~EB~

!FONT CHANGES

}fn-open}
~SF

}fn-close}
~

}font0}
~SF0~

}font1}
~SF1~

}font2}
~SF2~

}font3}
~SF3~

}font4}
~SF4~

}font5}
~SF5~

}font6}
~SF6~

}font7}
~SF7~

}font8}
~SF8~

!JUSTIFICATION

}forceprint}
~PL~

}just1}
~WJ~

}just2}
~LJ~

!MODE

}center}
~CT~

}left}
~RR~

}right}
~FR~

!UNDERLINING

}u+}
~BU13~

}u-}
~EU~

!---------------------------------------------------------------------------
!		  TERMINAL/PRINTER SPECIFIC (NOT BUILT-IN)
!---------------------------------------------------------------------------

!}wide}
!eA}@! set Sanders line length to 8 inches (default is 7 inches)

}u}
~VF-12~

}d}
~VF12~

}rlf}! reverse line feed
~LF-1~

}dotdot}
~IS' .'~! must use }right} mode

}space}
~IB~

!---------------------------------------------------------------------------
!					END OF SS
!---------------------------------------------------------------------------

!BOLDFACE

}b+}
~BB6~

}b-}
~EB~

!FONT CHANGES

}fn-open}
~SF

}fn-close}
~

}font0