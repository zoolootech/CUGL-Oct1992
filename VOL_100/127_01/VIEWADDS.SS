!---------------------------------------------------------------------------
! File V.SS: RAP SS file for the Adds Viewpoint Terminal
!---------------------------------------------------------------------------

!---------------------------------------------------------------------------
!					COMMANDS TO RAP
!		(This section MUST be the first in the SS file)
!---------------------------------------------------------------------------

.ou 1

!---------------------------------------------------------------------------
!				  HARDWARE INFORMATION
!---------------------------------------------------------------------------

!---------------------------------------------------------------------------
!			   USERS PERSONAL SUBSTITUTIONS
!---------------------------------------------------------------------------

{LH{
.nf
.ce 3
Department of Microbiology
University of Massachusetts
Amherst, MA 01003
.fi

!---------------------------------------------------------------------------
!					BUILT-IN SUBSTITUTIONS
!---------------------------------------------------------------------------

!INITIALIZATION

}init}
0P! Set reverse video attribute for tagged characters

!BOLDFACE

}b+}
}tag}

}b-}
}normal}

!FONT CHANGES

!}fn-open}


!}fn_close}


!JUSTIFICATION

!}forceprint} NONE

!}just1} NONE

!}just2} NONE

!MODE

!}center} NONE

!}left}! flush left = restore mode in RAP
!}normal}

!}right} NONE

!
!SUBSCRIPTS

}d1}
}tag}

}u2}
}normal}

!SUPERSCRIPTS

}u1}
}tag}

}d2}
}normal}

!UNDERLINING

}u+}
}tag}

}u-}
}normal}

!---------------------------------------------------------------------------
!						 GREEK LETTERS
!---------------------------------------------------------------------------
{alpha{
}tag}a}normal}

{beta{
}tag}b}normal}

{gamma{
}tag}g}normal}

{mu{
}tag}m}normal}

{lambda{
}tag}l}normal}

{PHI{
}tag}P}normal}
!---------------------------------------------------------------------------
!		  TERMINAL/PRINTER SPECIFIC (NOT BUILT-IN)
!---------------------------------------------------------------------------

}tag}


}normal}


}dotdot}
 . . . !

}space}
     !

!---------------------------------------------------------------------------
!			   USERS PERSONAL SUBSTITUTIONS
!---------------------------------------------------------------------------

{cr{
}u1}51}d2}Cr

!---------------------------------------------------------------------------
!					END OF SSV.AAA
!---------------------------------------------------------------------------


}u-}
}normal}

!-----------------------------