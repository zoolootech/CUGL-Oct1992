/* m11ext.c */

/*
 * (C) Copyright 1989,1990
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <setjmp.h>
#include "asm.h"
#include "m6811.h"

char	*cpu	= "Motorola 6811";
int	hilo	= 1;
char	*dsft	= "ASM";
