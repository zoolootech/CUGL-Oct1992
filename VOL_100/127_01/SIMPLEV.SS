!---------------------------------------------------------------------------
! File SSV: RAP SS View file for a simple CRT terminal
!---------------------------------------------------------------------------

!---------------------------------------------------------------------------
!					COMMANDS TO RAP
!		(This section MUST be the first in the SS file)
!---------------------------------------------------------------------------

.ou 20! OUTPUT TO (24 LINE) SCREEN 20 LINES AT A TIME

!---------------------------------------------------------------------------
!				  HARDWARE INFORMATION
!---------------------------------------------------------------------------

!Put yours here if you need any.

!---------------------------------------------------------------------------
!					BUILT-IN SUBSTITUTIONS
!---------------------------------------------------------------------------

!INITIALIZATION

!}init}
!Put initialization escape sequence(s) here if needed.

!BOLDFACE

}b+}
!Delete; or supply proper escape codes for your CRT.

}b-}
}normal}

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
!Delete; or supply suitable escape sequence for your CRT.

}u-}
}normal}

!---------------------------------------------------------------------------
!		  TERMINAL/PRINTER SPECIFIC (NOT BUILT-IN)
!---------------------------------------------------------------------------

}bli-rev}! Blinking reverse video.
!Delete; or supply suitable escape sequence for your CRT.

}normal}! Normal video.
!Delete; or supply suitable escape sequence for your CRT.

}reverse}! Reverse video.
!Delete; or supply suitable escape sequence for your CRT.

!---------------------------------------------------------------------------
!			   USERS PERSONAL SUBSTITUTIONS
!---------------------------------------------------------------------------

!Put yours here.

!---------------------------------------------------------------------------
!					END OF SSV
!---------------------------------------------------------------------------


!SUPER