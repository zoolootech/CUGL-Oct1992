* Bit field instructions' tests.
        org     $1000
        bra.s   label
        bfchg   label.w{d1:d2}
        bfclr   ([5,a0,d0.l*8],$55){d1:d2}
        bfclr   ([$55555,a0,d0.l*8],$aaaaa){d1:d2}
        bfexts  d0{7:11},d1
label   bfextu  d0{7:11},d3
        bfffo   label.l{7:11},d5
        bfins   d7,d0{7:11}
        bftst   d0{5:3}
        bftst   d0{$12345678:$3333}
        bftst   d7{d5:3}
        bftst   d5{5:d3}
        bra     label

* Entries with errors
        bfexts  d0{7:11}            missing destination register
        bfins   d0{7:11}            missing source register
        bfchg   (128,PC{d1:d2}      PC relative not allowed
        bfchg   d0{1:2},d1          no destination allowed for bfchg
        bfchg   d0                  missing field specification
        bfchg   d0{1,2}             invalid offset/width separator
        bfchg   d0{a1,d0}           address register must not ne used
        bfchg   {2:3}               missing effective address
        end
