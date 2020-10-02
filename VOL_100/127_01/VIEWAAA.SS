!---------------------------------------------------------------------------
! File SSV.AAA: RAP SS file for the Ann Arbor Ambassador CRT Terminal
!---------------------------------------------------------------------------

!---------------------------------------------------------------------------
!					COMMANDS TO RAP
!		(This section MUST be the first in the SS file)
!---------------------------------------------------------------------------

.ou 24! OUTPUT TO SCREEN 26 LINES AT A TIME

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
[0m_! normal
[0J! erase from cursor to end of display

}deinit}
[7m! reverse

!FONT CHANGES

}fn-open}
[0m!normal

}fn-close}
! to wipe off font number

}font0}
}b-}

}font1}
}b+}

}font2}
}b+}

}font3}
}b+}

}font4}
}b+}

}font5}
}b+}

}font6}
}b+}

}font7}
}b+}

}font8}
}b+}

!BOLDFACE

}b+}
[1m

}b-}
}normal}

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
}bli-rev}

}u2}
}normal}

!SUPERSCRIPTS

}u1}
}reverse}

}d2}
}normal}

!UNDERLINING

}u+}
[4m

}u-}
}normal}

!---------------------------------------------------------------------------
!		  TERMINAL/PRINTER SPECIFIC (NOT BUILT-IN)
!---------------------------------------------------------------------------

}bli-rev}
[5;7m

}bol-rev}
[1;7m

}normal}
[0m

}reverse}
[7m

}dotdot}
 . . . !

}space}
     !

!---------------------------------------------------------------------------
!			   USERS PERSONAL SUBSTITUTIONS
!---------------------------------------------------------------------------

{cr{
}u1}51}d2}Cr

{alpha{
}bol-rev}a}normal}

{beta{
}bol-rev}b}normal}

{gamma{
}bol-rev}g}normal}

{kappa{
}bol-rev}k}normal}

{lambda{
}bol-rev}l}normal}

{mu{
}bol-rev}u}normal}

{PHI{
}bol-rev}P}normal}

!---------------------------------------------------------------------------
!					END OF SSV.AAA
!---------------------------------------------------------------------------
TS

}u1}
}reverse}

}d2}
}normal}

!UNDERLINING

}u+}
[4m

}u-}
}normal}

!-------------