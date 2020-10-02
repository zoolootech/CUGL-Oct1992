/* HEADER: CUG120.14;
   TITLE: META4;
   VERSION: 1.0;
   DATE: 08/00/1981;
   DESCRIPTION: "Dr. W.A. Gale's META4 compiler-compiler from DDJ August 1981";
   KEYWORDS: compiler-compiler,programming languages;
   SYSTEM: CP/M;
   FILENAME: META40.C;
   CRC: FBB6;
   AUTHORS: W.A.Gale, Jan Larsson;
   COMPILERS: BDS C;
   REFERENCES: AUTHORS: W.A.Gale; TITLE: "META4 Compiler-Compiler";
	CITATION: "Doctor Dobb's Journal, August 1981" ENDREF;
*/

#include "meta40.h"



main( argc, argv )
int argc ;
char **argv ;
{
	spcharflag = eoflag = printflag = FALSE ;
    strcpy( m4file, argv[1] );
	strcpy( metfile, argv[2] );
	strcpy( outfile, argv[3] );
	newext( m4file, ".M4" );
    if(argc < 4){
			puts("Usage: A>M4 <m4_file> <source_file> <target_file>\n");
			exit();
			}
    iav[1] = m4file;
	iav[2] = metfile;
	iav[3] = outfile;
	iac = argc ;
	nl = '\n' ;
	puts("\n Meta4 Compiler-Compiler ver 1.0");
	puts("\n Copyright (c) 1981 W.A. Gale\n\n");
	fin();
	frc();
	if(argc > 4)printflag = TRUE ; else printflag = FALSE ;
	fli();
	ipc = 0 ;
loc00:	
	fgi();
	cc = ri[c0] ;
	switch (cc) {
	    case 'l' : 
			if(pi == c1)aa = TRUE ; else aa = FALSE ;
	 		if(aa){
			    if(fl){
					flw(); 
					ism++ ; 
					}
			    else pl = pm ; 
				}
			else { 
			    cc = ri[c1];
			    switch (cc) {
					case 'm' : 
					    fl = 0 ;
					    bb = 2 ;
					    while(TRUE){
							if(bb < pi)aa = TRUE ; else aa = FALSE ;
							if(!aa)break;
							aa = ri[bb] ;
							dd = ns[pl];
                            if(aa != dd)aa = TRUE ; else aa = FALSE;
							if(aa)goto loc99; 
							bb++;
							fla();
							}
					    fl = 1 ;
					    flb();
					    break ;
					case 'i' :  
					    fl = 0 ;
					    cc = ns[pl];
					    po = 0 ;
					    fza();
					    while(TRUE){
							if(!aa)break;
	    	   				os[po] = cc ;
							po++ ;
							fla();
							cc = ns[pl];
							fza();
							dd = aa ;
							fzn();
							aa = dd | aa ;
							}
					    if(po == c0)aa = TRUE ; else aa = FALSE ;
					    if(aa)goto loc99 ;
					    fms();
					    ipr[c0] = iaa ;
					    fl = 1 ;
					    break ;
					case 'n' : 
					    fl = 0 ;
					    iaa = i00 ;
					    while(TRUE){
							cc = ns[pl];
							fzn();
							if(!aa)break;
							fl = 1 ;
							iaa = iaa * 10 ;
							cc = cc - '0' ;
							ibb = cc ;
							iaa += ibb ;
							fla();
							}
					    ipr[c0] = iaa ;
					    break;
					case 'h' : 
					    fl = 0 ;
					    iaa = 0 ;
					    while(TRUE){
							cc = ns[pl];
							fzh();
							if(!aa)break;
							fl = 1 ;
							iaa *= 16 ;
							ibb = cc ;
							iaa += ibb ;
							fla();
							};
					    ipr[c0] = iaa ;
					    break ;
					case 'q' : 
					    dd = ri[c2];
					    cc = ns[pl];
					    po = 0 ;
					    if(cc == dd)aa = TRUE ; else aa = FALSE ;
					    if(aa){
							fla();
							while(TRUE){
							    cc = ns[pl];
							    if(cc != nl)aa=TRUE;else aa=FALSE;
							    if(cc != dd)bb=TRUE;else bb=FALSE;
							    aa &= bb ;
							    if(!aa)break;
							    os[po] = cc ;
							    po++ ;
							    fla();
							    }
							fla();
							if(cc == nl)aa = TRUE ; else aa = FALSE;
							if(aa){
							    iln++ ;
							    ism = i00 ;
							    }
							else ;
							fl = 1 ;
					        }
					    else fl = 0 ;				
					    break ;
					default :
						puts("Not lex: ");
						putchar(cc);putchar('\n');
					}
					
			    }
	        break;
		case 'f' : 
		    if(!fl)goto loc20;
		    break;
		case 'p' : 
		    bb = 1 ;
		    while(TRUE){
				if(bb < pi)aa = TRUE ; else aa = FALSE ;
				if(!aa)break ;
				cc = ri[bb];
				bo[pb] = cc ;
				pb++ ;
				bb++ ;
				}
		    break ;
		case 'o' : 
		    bb = 0 ;
		    while(TRUE){
				if(bb < pb)aa = TRUE ; else aa = FALSE ;
				if(!aa)break;
				cc = bo[bb];
				bb++;
				pchar( cc, f2 );
				}
		    pb = c0 ;
		    if(pi == c1)aa = TRUE ; else aa = FALSE ;
		    if(aa)pchar( '\n', f2 );
		    break ;
		case 'x' : 
		    if(pi == c1)aa = TRUE ; else aa = FALSE ;
		    if(aa){ 
				if(fl);else {
loc98:			    puts("Error at line num: ");
				    iaa = iln ;
				    fpn() ;
				    puts(" symbol  ");
				    iaa = ism ;
				    fpn();
					putchar(cb);
				    putchar('\n');
				    while(TRUE){
						cc = ns[pl];
						if(cc != zx)aa=TRUE;else aa=FALSE;
						if(cc != c0)bb=TRUE;else bb=FALSE;
						aa &= bb ;
						if(!aa)break;
						if(cc == nl)aa=TRUE;else aa=FALSE;
						if(aa){
						    iln++;
							ism = 0;	
							}
						else ;
						fla(); 
						flb(); 
						}
					if(cc == c0)bb = TRUE ; else bb = FALSE ;
					if(bb){   
						puts("\nEOF recognized\n");
						goto loc21 ;
						}
					else ;
					fla(); 
					flb();                    
					flw(); 
					ipc = izc ; 
					ipt = izt ; 
					fl = 1 ; 
					}   
				}
			else { 
				cc = ri[c1];
				switch (cc) {
					case 'n' : 
						iaa = iln ;
					   	fwn();
						break;
					case 'o' :
						bb = 0 ;
						while(TRUE){
							if(bb < pb)aa = TRUE ; else aa = FALSE ;
							if(!aa)break;
							cc = bo[bb];
							bb++ ;
							putchar( cc );
							}
						putchar('\n');
						pb = c0 ;
						break;
					case 'm' : 
						izc = ipc ; 
						izt = ipt ; 
						zx = ri[c2]; 
						break;
					default: ;
					}
				} 
            break;
		case 't' : 
			if(fl){
				goto loc20;
				}
			else ;
			break;
		case 'g' : 
			wa = ri[c1];
			wb = ri[c2];
			ipt += i03 ;
			iaa = 597 ; 
			if(iaa <= ipt)aa = TRUE ; else aa = FALSE ;
			if(aa){
				puts("stack overflow >>>>\n");
				goto loc98 ;
				}
			else ;
			ist[ipt] = ipc ;
			pack( &ipc, &wa, &wb );
			iaa = ipt ;
			iaa++;
			ist[iaa]=i00 ;
			iaa++;
			ist[iaa] = i00 ;
			break ;
		case 'r' : 
			ipc = ist[ ipt ];
			if(ipt < i03)aa = TRUE ; else aa = FALSE ;
			if(aa){
				puts("Stack underflow....\n");
				goto loc98 ;
				}
			else ;
			ipt = ipt - i03 ;
			break;
		case 's' : 
			if(pi == c1)aa = TRUE ; else aa = FALSE ;
			if(aa)fl = 1 ;
			else {
				cc = ri[c1];
				switch (cc) {
					case 'f' : 
						fl = 0 ;
						break;
					case 'c' : 
						fl = c1 - fl ;
						break;
					default:
						puts("set error \n");
					}
				}
			break;
		case 'u' : 
			if(pi == c1)aa = TRUE ; else aa = FALSE ;
			if(aa){ 
				iaa = ipt ;		
				iaa++;
loc10:
				ibb = ist[iaa]; 
				if(ibb < i01)aa = TRUE ; else aa = FALSE ;
				if(aa){
					iuu++;
					ibb = iuu ;
					ist[iaa] = iuu ;
					}
				else ; 
				iaa = ibb ;
   							ipr[c0] = iaa ;
				fwn(); 
				}
			else 
				goto loc22 ;
			break ;
		case 'c' : 
			bb = 0 ;
			while(TRUE){
				if(bb < po)aa = TRUE ; else aa = FALSE ;
				if(!aa)break;
				cc = os[bb];
				bo[pb] = cc ;
				pb++ ; bb++ ;
				}
			break;
		case 'v' : 
			if(pi == c1)aa = TRUE ;else aa = FALSE ;
			if(aa){
				iaa = ipt ;
				iaa++ ; iaa++ ;
				goto loc10 ; 
				}
			else 
				goto loc22 ;
			break;
		case 'm' : 
			cc = ri[c1];
			switch (cc) {
				case 's' : fmh() ; break ; 
				case 'p' : fmp() ; break ; 
				case 'e' : fme();ipr[c0]=iaa;break; 
				case 'q' : fms();ipr[c0]=iaa;break ; 
				case 'c' : fmc();ipr[c0]=iaa;break; 
				case 'd' : fmd();ipr[c0]=iaa;break; 
				case 'i' : 
						cc = ri[c2];
						fzn();
						if(aa)mk = cc - x0 ; 
						else mk = c2 ;
						fmi();
						break;
				default  : puts("Illegal memory operation.\n");
				}
			break;
		case 'j' : 
			aa = aa ;
loc20:
			aa = ri[c1];
			bb = ri[c2];
			pack( &ilb,&aa,&bb);
			ipc = ilt[ilb];
			break;
		case 'e' : 
			aa = aa ;
loc21:
			pchar( CPMEOF, f2 );
			fflush( f2 );
			xclose( f1 );
			xclose( f2 );
			iaa = imm ; 
			fpn();
			puts(" max memory used\n");
			exit();
		default : 
			aa = aa ;
loc22:					qi = 0 ;
			fft();
			ffi();
			fst();
		}
	goto loc00;
loc99:
	fl = 0 ;
	goto loc00;
}




		goto loc98 ;
				}
			else ;
			ipt = ipt - i03 ;
			break;