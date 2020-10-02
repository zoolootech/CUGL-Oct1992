/* HEADER: CUG120.17;
   TITLE: META4;
   VERSION: 1.0;
   DATE: 08/00/1981;
   DESCRIPTION: "Dr. W.A. Gale's META4 compiler-compiler from DDJ August 1981";
   KEYWORDS: compiler-compiler,programming languages;
   SYSTEM: CP/M;
   FILENAME: META43.C;
   CRC: 5347;
   AUTHORS: W.A.Gale, Jan Larsson;
   COMPILERS: BDS C;
   REFERENCES: AUTHORS: W.A.Gale; TITLE: "META4 Compiler-Compiler";
	CITATION: "Doctor Dobb's Journal, August 1981" ENDREF;
*/

#include "meta40.h"

#define BOOLA aa = TRUE ; else aa = FALSE
#define BOOLB bb = TRUE ; else bb = FALSE
#define BOOLE ee = TRUE ; else ee = FALSE



fra()
{
	
	os[c0]=cc;
	po = 1 ;
	while(TRUE){
		cc = gchar( f1 );
		fza();
		dd = aa ;
		fzn();
		aa = aa | dd ;
		if(!aa)break;
		os[po] = cc ;
		po++;
		}
	
	if(cc == nl)BOOLA;
	if(aa);else {
		while(TRUE){
			cc = gchar( f1 );
			if(cc != nl)BOOLA;
			if(!aa)break;
			}
		}
}



frc()
{
	
	xclose( f1 );
	ibk = iav[c1] ;
	xopen(ibk, f1);
	fck();
	ipc = 1 ;
	ll = ipl = 0 ;
	while(TRUE){
		rc = gchar( f1 );
loc33:
		if(er == c0)BOOLA;
		if(!aa)break;
		switch (rc) {
			case '/' : 
				cc = gchar( f1 );
				if(cc == '-')BOOLA;
				if(aa){ 
					cc = gchar( f1 );
					frn();
					irn = -irn ;
					goto loc37;
					}
				else ;
				fzn();
				if(aa){ 
					frn();
loc37:
					unpack(&irn,&aa,&bb);
					ks[ipc] = aa ;
					ipc++;
					ll++;
					ks[ipc] = bb ;
					ipc++;
					ll++;
					rc = cc ;
					if(rc == ' ')BOOLA;
					if(aa);else goto loc33;
					}
				else {
					ks[ipc] = '/' ;
					ipc++;
					ll++ ;
					rc = cc ;
					goto loc33;
					}
				break;
			case '\n' : 
				ks[ipl] = ll ;
				ipl = ipc ;
				ipc++;
				ll = c0 ;
				break;
			case '.' : 
				if(ll == c0)BOOLA;
				if(aa){
					frl();
					rc = '\n';
					ipc-- ;
					goto loc33;
					}
				else goto loc35;
				break;
			case 'g' : 
				if(ll == c0)BOOLA;
				if(aa){
					cc = gchar( f1 );
					fra();
					os[po] = c0 ;
					fme();
					irn = imi[iaa];
					if(irn == i00)BOOLA;
					if(aa){
						irn = iaa ;
						ks[ipc] = nl ;
						}
					else ks[ipc] = rc ;
					ipc++;
					unpack(&irn,&aa,&bb);
					ks[ipc] = aa ;
					ipc++;
					ks[ipc] = bb ;
					ipc++;
					ll = c3 ;
					rc = nl ;
					goto loc33;
					}
				else goto loc35;
				break;
			default:
				aa = aa ;
loc35:
				ks[ipc] = rc ;
				ipc++;
				ll++;
			}
		}
	ipc-- ;
	if(er != c1)BOOLA;
	if(aa){
		puts("Cant read commands.\n");
		exit();
		}
	else ;
	xclose( f1 );
	ibk = iav[ c2 ];
	xopen(ibk, f1 );
	fck();
	iaa = 0 ;
	while(TRUE){
		if(iaa < ipc)BOOLA;
		if(!aa)break;
		ll = ks[iaa];
		ibb = iaa + i01 ;
		aa = ks[ibb];
		if(aa == nl)BOOLA;
		if(aa){ 
			ks[ibb] = xg ;
			ibb++;
			aa = ks[ibb];
			ibb++;
			bb = ks[ibb];
			pack(&irn,&aa,&bb);
			bb = mc[irn];
			if(bb != c1)BOOLA;
			if(aa){ 
				icc = irn - i10 ;
				while(TRUE){
					if(icc < irn)BOOLA;
					if(!aa)break;
					bb = mc[icc];
					putchar( bb );
					icc++;
					}
				putchar( cb );
				puts("Subroutine undefined.\n");
				}
			else ;
			icc = imi[irn];
			unpack(&icc,&aa,&bb);
			ks[ibb] = bb ;
			ibb-- ;
			ks[ibb] = aa ;
			}
		else ;
		ibb = ll ;
		iaa = iaa + ibb ;
		iaa = iaa + i01 ;
		}
	fmp();
	iaa = ipc;
	fpn();
	puts("command bytes ");
	iaa = inl ;
	fpn();
	puts("number labels ");
	iaa = pn ;
	fpn();
	puts("subroutines ");
	putchar( '\n' );
}



frl()
{
	
	while(TRUE){
		cc = gchar( f1 );
		fza();
		if(aa)goto loc80; else ;
		fzn();
		if(aa)goto loc85; else ;
		if(cc != nl)BOOLA;
		if(!aa)break;
		}
	return;
loc80:
	fra();
	os[po] = c0 ;
	fme();
	imi[iaa] = ipl ;
	mc[iaa] = c1 ;
	pn++;
	return;
loc85:
	inl++;
	frn();
	ilt[irn] = ipl ;
}


			

frn()
{
	
	irn = 0 ;
	while(TRUE){
		cc = cc - x0 ;
		iaa = cc ;
		irn = irn * 10 ;
		irn = irn + iaa ;
		cc = gchar( f1 );
		fzn();
		if(!aa)break;
		}
}




fst()
{
	
	qi++;
	cc = ri[qi];
	switch (cc ) {
		case 'y' : 
			yp++;
			if(sd <= yp)BOOLA;
			if(aa){
				puts("Y overflow.\n");
				yp = sd ;
				fl = 0 ;
				}
			else ;
			iys[yp] = itu ;
			break;
		case 'z' : 
			zp++;
			if(sd <= zp)BOOLA;
			if(aa){
				puts("Z overflow.\n");
				zp = sd ;
				fl = 0 ;
				}
			else ;
			izs[zp] = itu ;
			break;
		case '+' : 
			iaa = iys[yp];
			iaa = iaa + itu ;
			iys[yp] = iaa ;
			break;
		case '-' : 
			iaa = iys[yp];
			iaa = iaa - itu ;
			iys[yp] = iaa ;
			break ;
		case '*' : 
			iaa = iys[yp];
			iaa = iaa * itu ;
			iys[yp] = iaa ;
			break ;
		case '>' : 
			iaa = iys[yp];
			if(iaa < itu)BOOLA;
loc12:
			if(aa)fl = 1 ; else fl = 0 ;
			fpy();
			break;
		case '<' :
			iaa = iys[yp];
			if(itu < iaa)BOOLA;
			goto loc12;
			break;
		case '=' :
			iaa = iys[yp];
			if(iaa == itu)BOOLA;
			goto loc12;
		case 'i' : 
			qi++;
			dd = ri[qi];
			ibb = itu;
			qi++;
			fft();
			cc = dd ;
			fzn();
			if(aa)bb = cc - x0 ; 
			else {
loc13:
				puts("Bad indirect index.\n");
				bb = 0 ;
				}
			if(bb < mk)BOOLA;
			if(aa){
				iaa = bb ;
				iaa = iaa + itu ;
				imi[iaa] = ibb ;
				return;
				}
			else {
				bb = bb - mk ;
				if(bb < mk)BOOLA;
				if(aa){
					iaa = bb ;
					iaa = itu + iaa ;
					aa = ibb ;
					mc[iaa] = aa ;
					}
				else goto loc13;
				}
			break;
		case 'c' : 
			iaa = itu ;
			fwn();
			break;
		case 'l' : 
			aa = itu;
			bo[pb] = aa ;
			pb++;
			break;
		case 'a' : 
			aa = itu ;
			os[po] = aa ;
			po++;
			os[po] = c0 ;
			break;
		case 'b' : 
			po = itu ;
			os[po] = c0 ;
			break;
		case 'g' : 
			iuu = itu ;
			break;
		case 'u' : 
			iaa = ipt ;
loc39:
			iaa++;
			ist[iaa] = itu ;
			break;
		case 'v' : 
			iaa = ipt ;
			iaa++;
			goto loc39;
			break;
		case 'd' : 
			break;
		case 'h' : 
			aa = itu ;
			itu = aa ; 
			iaa = itu / 16 ;
			ibb = iaa * 16 ;
			ibb = itu - ibb ;
			cc = iaa ;
			fwh();
			cc = ibb ;
			fwh();
			break;
		default : 
			cc = ri[qi];
			fzn();
			if(aa)aa = cc - x0 ;
			else {
				puts("Illegal store.\n");
				aa = c0 ;
				}
			ipr[aa] = itu ;
		}
}



fwh()
{
	
	if(cc <= 9)BOOLA;
	if(aa)cc = cc + '0' ;
	else cc = cc + 'a' + 7 ;
	bo[pb] = cc ;
	pb++;
}


fwn()
{
	
	fds();
	while(TRUE){
		ibb = nd ;
		if(i00 < ibb)BOOLA;
		if(!aa)break;
		nd--;
		aa = ds[nd];
		bo[pb] = aa ;
		pb++;
		}
}


fza()
{
	
	aa = cc - 'a' ;
	bb = 'z' - cc ;
	if(aa <= cv)BOOLA;
	if(bb <= cv)BOOLB;
	aa = aa & bb ;
}


fzh()
{
	
	if('0' <= cc)BOOLA;
	if(cc <= '9')BOOLB;
	aa = aa & bb ;
	if(aa){
		cc = cc - '0' ;
		return;
		}
	else ;
	if('a' <= cc)BOOLA;
	if(cc <= 'f')BOOLB;
	aa = aa & bb ;
	if(aa){
		cc = cc - 'a' ;
		bb = 10 ;
		cc = cc + bb ;
		return;
		}
	else ;
}




fzn()
{
	
	aa = cc - '0' ;
	bb = '9' - cc ;
	if(aa <= 9)BOOLA;
	if(bb <= 9)BOOLB;
	aa = aa & bb ;
}


fzw()
{
	
	if(cc == ' ')BOOLA;
	if(cc == '\t')BOOLB;
	aa = aa | bb ;
	if(cc == '\n')BOOLB;
	aa = aa | bb ;
}


	
				}
			if(bb < mk)BOOLA;
			if(aa){
				iaa = bb ;
				iaa = iaa + itu ;
				imi[