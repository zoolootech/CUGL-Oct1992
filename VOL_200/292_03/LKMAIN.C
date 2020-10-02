/* lkmain.c */

/*
 * (C) Copyright 1989,1990
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include "aslink.h"

VOID
main(argc, argv)
char *argv[];
{
	register char *p;
	register c, i;
	FILE *afile();

	fprintf(stdout, "\n");

	pflag = 1;
	startp = (struct lfile *) new (sizeof (struct lfile));

	for (i=1; i<argc; ++i) {
		p = argv[i];
		if (*p == '-') {
			while (ctype[c = *(++p)] & LETTER) {
				switch(c) {

				case 'c':
				case 'C':
					startp->f_type = F_STD;
					break;

				case 'f':
				case 'F':
					startp->f_type = F_LNK;
					break;
					
				case 'n':
				case 'N':
					pflag = 0;
					break;

				case 'p':
				case 'P':
					pflag = 1;
					break;

				default:
					usage();
				}
			}
		} else {
			if (startp->f_type == F_LNK) {
				startp->f_idp = p;
			}
		}
	}
	if (startp->f_type == NULL)
		usage();
	if (startp->f_type == F_LNK && startp->f_idp == NULL)
		usage();

	cfp = NULL;
	sfp = NULL;
	filep = startp;
	while (1) {
		ip = ib;					
		if (getline() == 0)
			break;
		if (pflag && sfp != stdin)
			fprintf(stdout, "%s\n", ip);
		if (*ip == NULL || parse())
			break;
	}
	fclose(sfp);
	if (linkp == NULL)
		usage();

	syminit();
	for (pass=0; pass<2; ++pass) {
		cfp = NULL;
		sfp = NULL;
		filep = linkp;
		hp = NULL;
		radix = 10;

		while (getline()) {
			ip = ib;
			link();
		}
		if (pass == 0) {
			/*
			 * Set area base addresses.
			 */
			setbas();
			/*
			 * Link all area addresses.
			 */
			lnkarea();
			/*
			 * Process global definitions.
			 */
			setgbl();
			/*
			 * Check for undefined globals.
			 */
			symdef(stderr);
			/*
			 * Output Link Map.
			 */
			if (mflag)
				map();
			/*
			 * Open output file
			 */
			if (oflag == 1)
				ofp = afile(linkp->f_idp, "IHX", 1);
			if (oflag == 2)
				ofp = afile(linkp->f_idp, "S19", 1);
		} else {
			reloc('E');
		}
	}
}

VOID
link()
{
	register c;

	if ((c=endline()) == 0) { return; }
	switch (c) {

	case 'X':
		radix = 16;
		break;

	case 'D':
		radix = 10;
		break;

	case 'Q':
		radix = 8;
		break;

	case 'H':
		if (pass == 0) {
			newhead();
		} else {
			if (hp == 0) {
				hp = headp;
			} else {
				hp = hp->h_hp;
			}
		}
		sdp.s_area = NULL;
		sdp.s_areax = NULL;
		sdp.s_addr = 0;
		break;

	case 'M':
		if (pass == 0)
			module();
		break;

	case 'A':
		if (pass == 0)
			newarea();
		if (sdp.s_area == NULL) {
			sdp.s_area = areap;
			sdp.s_areax = areap->a_axp;
			sdp.s_addr = 0;
		}
		break;

	case 'S':
		if (pass == 0)
			newsym();
		break;

	case 'T':
	case 'R':
	case 'P':
		if (pass == 0)
			break;
		reloc(c);
		break;

	default:
		break;
	}
	if (c == 'X' || c == 'D' || c == 'Q') {
		if ((c = get()) == 'H') {
			hilo = 1;
		} else
		if (c == 'L') {
			hilo = 0;
		}
	}
}

VOID
map()
{
	register i;
	register struct head *hdp;

	/*
	 * Open Map File
	 */
	mfp = afile(linkp->f_idp, "MAP", 1);
	/*
	 * Output Map Area Lists
	 */
	page = 0;
	lop  = NLPP;
	slew(mfp);
	ap = areap;
	while (ap) {
		lstarea(ap);
		ap = ap->a_ap;
	}
	/*
	 * List Linked Files
	 */
	newpag(mfp);
	fprintf(mfp, "\nFiles Linked      [ module(s) ]\n\n");
	hdp = headp;
	filep = linkp;
	while (filep) {
		fprintf(mfp, "%-16s", filep->f_idp);
		i = 0;
		while (hdp->h_lfile == filep) {
			if (i % 5) {
			    fprintf(mfp, ", %8.8s", hdp->m_id);
			} else {
			    if (i) {
				fprintf(mfp, ",\n%20s%8.8s", "", hdp->m_id);
			    } else {
				fprintf(mfp, "  [ %8.8s", hdp->m_id);
			    }
			}
			hdp = hdp->h_hp;
			i++;
		}
		if (i)
			fprintf(mfp, " ]");
		fprintf(mfp, "\n");
		filep = filep->f_flp;
	}
	/*
	 * List Base Address Definitions
	 */
	if (basep) {
		newpag(mfp);
		fprintf(mfp, "\nUser Base Address Definitions\n\n");
		bsp = basep;
		while (bsp) {
			fprintf(mfp, "%s\n", bsp->b_strp);
			bsp = bsp->b_base;
		}
	}
	/*
	 * List Global Definitions
	 */
	if (globlp) {
		newpag(mfp);
		fprintf(mfp, "\nUser Global Definitions\n\n");
		gsp = globlp;
		while (gsp) {
			fprintf(mfp, "%s\n", gsp->g_strp);
			gsp = gsp->g_globl;
		}
	}
	fprintf(mfp, "\n\f");
	symdef(mfp);
}

int
parse()
{
	register c;
	char fid[NINPUT];

	while ((c = getnb()) != 0) {
		if ( c == '-') {
			while (ctype[c=get()] & LETTER) {
				switch(c) {

				case 'i':
				case 'I':
					oflag = 1;
					break;

				case 's':
				case 'S':
					oflag = 2;
					break;

				case 'm':
				case 'M':
					++mflag;
					break;

				case 'x':
				case 'X':
					xflag = 0;
					break;

				case 'q':
				case 'Q':
					xflag = 1;
					break;

				case 'd':
				case 'D':
					xflag = 2;
					break;

				case 'e':
				case 'E':
					return(1);

				case 'n':
				case 'N':
					pflag = 0;
					break;

				case 'p':
				case 'P':
					pflag = 1;
					break;

				case 'b':
				case 'B':
					bassav();
					return(0);

				case 'g':
				case 'G':
					gblsav();
					return(0);

				default:
					fprintf(stderr, "Invalid option\n");
					exit(1);
				}
			}
		} else
		if (ctype[c] & (LETTER|DIGIT)) {
			if (linkp == NULL) {
				linkp = (struct lfile *)
					new (sizeof (struct lfile));
				lfp = linkp;
			} else {
				lfp->f_flp = (struct lfile *)
						new (sizeof (struct lfile));
				lfp = lfp->f_flp;
			}
			getfid(fid, c);
			lfp->f_idp = (char *) new (strlen(fid)+1);
			strcpy(lfp->f_idp, fid);
			lfp->f_type = F_REL;
		} else
		if (c == ';') {
			return(0);
		} else
		if (c == ',') {
			;
		} else {
			fprintf(stderr, "Invalid input");
			exit(1);
		}
	}
	return(0);
}

/*
 * Base string save
 */
VOID
bassav()
{
	if (basep == NULL) {
		basep = (struct base *)
			new (sizeof (struct base));
		bsp = basep;
	} else {
		bsp->b_base = (struct base *)
				new (sizeof (struct base));
		bsp = bsp->b_base;
	}
	unget(getnb());
	bsp->b_strp = (char *) new (strlen(ip)+1);
	strcpy(bsp->b_strp, ip);
}
	
VOID
setbas()
{
	register v;
	char id[NCPS];

	bsp = basep;
	while (bsp) {
		ip = bsp->b_strp;
		getid(id, -1);
		if (getnb() == '=') {
			v = expr(0);
			for (ap = areap; ap != NULL; ap = ap->a_ap) {
				if (symeq(id, ap->a_id))
					break;
			}
			if (ap == NULL) {
				fprintf(stderr,
				"No definition of area %s\n", id);
			} else {
				ap->a_addr = v;
			}
		} else {
			fprintf(stderr, "No '=' in base expression");
		}
		bsp = bsp->b_base;
	}
}

/*
 * Global string save
 */
VOID
gblsav()
{
	if (globlp == NULL) {
		globlp = (struct globl *)
			new (sizeof (struct globl));
		gsp = globlp;
	} else {
		gsp->g_globl = (struct globl *)
				new (sizeof (struct globl));
		gsp = gsp->g_globl;
	}
	unget(getnb());
	gsp->g_strp = (char *) new (strlen(ip)+1);
	strcpy(gsp->g_strp, ip);
}
	
VOID
setgbl()
{
	register v;
	register struct sym *sp;
	char id[NCPS];

	gsp = globlp;
	while (gsp) {
		ip = gsp->g_strp;
		getid(id, -1);
		if (getnb() == '=') {
			v = expr(0);
			sp = lkpsym(id, 0);
			if (sp == NULL) {
				fprintf(stderr,
				"No definition of symbol %s\n", id);
			} else {
				if (sp->s_flag & S_DEF) {
					fprintf(stderr,
					"Redefinition of symbol %s\n", id);
					sp->s_axp = NULL;
				}
				sp->s_addr = v;
				sp->s_type |= S_DEF;
			}
		} else {
			fprintf(stderr, "No '=' in global expression");
		}
		gsp = gsp->g_globl;
	}
}

FILE *
afile(fn, ft, wf)
char *fn;
char *ft;
{
	register char *p1, *p2, *p3;
	register c;
	FILE *fp;
	char fb[FILSPC];

	p1 = fn;
	p2 = fb;
	p3 = ft;
	while ((c = *p1++) != 0 && c != FSEPX) {
		if (p2 < &fb[FILSPC-4])
			*p2++ = c;
	}
	*p2++ = FSEPX;
	if (*p3 == 0) {
		if (c == FSEPX) {
			p3 = p1;
		} else {
			p3 = "REL";
		}
	}
	while ((c = *p3++) != 0) {
		if (p2 < &fb[FILSPC-1])
			*p2++ = c;
	}
	*p2++ = 0;
	if ((fp = fopen(fb, wf?"w":"r")) == NULL) {
		fprintf(stderr, "%s: cannot %s.\n", fb, wf?"create":"open");
		exit(1);
	}
	return (fp);
}

char *usetxt[] = {
	"Startup:",
	"  -c                           Command line input",
	"  -f   file[LNK]               File input",
	"Usage: [-Options] file [file ...]",
	"  -p   Prompt and echo of file[LNK] to stdout (default)",
	"  -n   No echo of file[LNK] to stdout",
	"Relocation:",
	"  -b   area base address = expression",
	"  -g   global symbol = expression",
	"Map format:",
	"  -m   Map output generated as file[MAP]",
	"  -x   Hexidecimal (default)",
	"  -d   Decimal",
	"  -q   Octal",
	"Output:",
	"  -i   Intel Hex as file[IHX]",
	"  -s   Motorola S19 as file[S19]",
	"End:",
	"  -e   or null line terminates input",
	"",
	0
};

VOID
usage()
{
	register char	**dp;

	fprintf(stderr, "\nASxxxx Linker %s\n\n", VERSION);
	for (dp = usetxt; *dp; dp++)
		fprintf(stderr, "%s\n", *dp);
	exit(1);
}
