!----------------------------------------------------------------------
!        File DEFAULT.SS : RAP SS file for Diablo Printer
!----------------------------------------------------------------------

!----------------------------------------------------------------------
!                COMMANDS TO RAP
!        (This section MUST be the first in the SS file)
!----------------------------------------------------------------------

.cf 0!Single sheets
.cc 1!compress control codes

!----------------------------------------------------------------------
!           FONTS
!----------------------------------------------------------------------

}norm}
}"Normal Printwheel, <space>.}_
}wait}

}gensci}
}"General Scientific Printwheel, <space>.}_
}wait}

}apl}
}"APL Printwheel, <space>.}_
}wait}

!----------------------------------------------------------------------
!        Subscripts and Superscripts
!----------------------------------------------------------------------

}d1}!leadin to subscripts
}u}!half linefeed

}u2}!finish subscript
}d}!negative half linefeed

}u1}!leadin to superscript
}d}!negative half linefeed

}d2}!finish superscript
}u}!half linefeed

!----------------------------------------------------------------------
!           Rap Utilized Substitutions
!----------------------------------------------------------------------

!Initialization

!send 2xCR and two ETX codes.  The two ETXs serve to init. the 1200
!baud BIOS code.

}init}


!Font change already defined

!----------------------------------------------------------------------
!           Terminal Specific
!----------------------------------------------------------------------

}u}!Reverse half linefeed
U

}d}!Half linefeed.
D

}rlf}!Reverse linefeed
^J

!----------------------------------------------------------------------
!            END OF DEFAULT.SS
!----------------------------------------------------------------------
-----------------------

}d1}!