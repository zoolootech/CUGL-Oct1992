/* HEADER: CUG120.15;
   TITLE: META4;
   VERSION: 1.0;
   DATE: 08/00/1981;
   DESCRIPTION: "Dr. W.A. Gale's META4 compiler-compiler from DDJ August 1981";
   KEYWORDS: compiler-compiler,programming languages;
   SYSTEM: CP/M;
   FILENAME: META41.C;
   CRC: 7C04;
   AUTHORS: W.A.Gale, Jan Larsson;
   COMPILERS: BDS C;
   REFERENCES: AUTHORS: W.A.Gale; TITLE: "META4 Compiler-Compiler";
	CITATION: "Doctor Dobb's Journal, August 1981" ENDREF;
*/

#include "meta40.h"

#define BOOL aa = TRUE ; else aa = FALSE ;





fds()
{
	

	if(iaa < i00)aa = TRUE ; else aa = FALSE ;
	if(aa){
		bb = 1 ;
		iaa = -iaa ;
		}
	else bb = 0 ;
	if(iaa == i00)aa = TRUE ; else aa = FALSE ;
	if(aa){
		nd = c1 ;
		ds[c0] = x0 ;
		}
	else {
		nd = c0 ;
		while(TRUE){
			if(i00 < iaa)aa = TRUE ; else aa = FALSE ;
			if(!aa)break;
			iyy = iaa / i10 ;
			ibb = i10 * iyy ;
			ixx = iaa - ibb ;
			iaa = iyy ;
			aa = ixx ;
			aa = aa + x0 ;
			ds[nd] = aa ;
			nd++;
			}
		}
	ds[nd] = cm ; 
	nd = nd + bb ; 
}

fck()
{
	if(er != c0)aa = TRUE ; else aa = FALSE ;
	if(aa){
		puts("Cant open ");
		iaa = ibk ;
		fpn();
		putchar('\n');
		exit();
		}
}


ffi()
{
	

	qi++;
	cc = ri[qi];
	switch (cc) {
		case 'm' : 
			qi++;
			cc = ri[qi];
			fzn();
			if(aa)bb = cc - x0 ;
			else {	
loc11:
				puts("Index mem cell\n");
				bb = 0 ;
				}
			if(bb < mk)aa = TRUE ; else aa = FALSE ;
			if(aa){
				iaa = bb ;
				iaa = iaa + itu ;
				itu = imi[iaa];
				return;
				}
			else {
				bb = bb - mk ;
				if(bb < mk)aa = TRUE ; else aa = FALSE ;
				if(aa){
					iaa = bb ;
					iaa = itu + iaa ;
					aa = mc[iaa];
					itu = aa ;
					}
				else goto loc11 ;
				}
			break ;
		case 's' : 
			aa = itu ;
			bb = os[aa];
			itu = bb ;
			break;
		default : qi-- ;
		}
}


fft()
{
	

	cc = ri[qi];
	switch (cc) {
		case'y' : 
			itu = iys[yp];
			break;
		case '!' : 
			itu = iys[yp];
			fpy();
			break;
		case 'z' : 
			itu = izs[zp];
			if(zp == c0)aa = TRUE ; else aa = FALSE ;
			if(aa){
				puts("Z stacker\n");
				zp = c1 ;
				fl = 0 ;
				}
			else ;
			zp-- ;
			break;
		case 'n' : 
			qi++;
			aa = ri[qi];
			qi++;
			bb = ri[qi];
			pack(&itu,&aa,&bb);
			break;
		case 'h' : 
			itu = iys[yp];
			unpack(&itu,&aa,&bb);
			itu = aa ;
			break;
		case 'b' : 
			itu = po ;
			break;
		case 'u' : 
			iaa = ipt ;
loc38:
			iaa++;
			itu = ist[iaa];
			break;
		case 'v' : 
			iaa = ipt ;
			iaa++;
			goto loc38;
			break;
		default:   
			fzn();
			if(aa)aa = cc - '0' ;
			else {
				puts("Illegal fetch\n");
				aa = c0 ;
				}
			itu = ipr[aa];
		}
}


fgi()
{
	

	pi = 0 ;
	li = ks[ipc];
	ipc++;
	while(TRUE){
		if(pi < li)aa=TRUE;else aa = FALSE ;
		if(!aa)break;
		aa = ks[ipc];
		ipc++;
		ri[pi] = aa ;
		pi++;
		}
}



fin()
{
	

	zx = yp = zp = izc = izt = 0 ;
	xa = 'a' ;
	xb = 'b' ;
	xc = 'c' ;
	xd = 'd' ;
	xe = 'e' ;
	xf = 'f' ;
	xg = 'g' ;
	xh = 'h' ;
	xi = 'i' ;
	xj = 'j' ;
	xk = 'k' ;
	xl = 'l' ;
	xm = 'm' ;
	xn = 'n' ;
	xo = 'o' ;
	xp = 'p' ;
	xq = 'q' ;
	xr = 'r' ;
	xs = 's' ;
	xt = 't' ;
	xu = 'u' ;
	xv = 'v' ;
	xw = 'w' ;
	xx = 'x' ;
	xy = 'y' ;
	xz = 'z' ;
	x0 = '0' ;
	x1 = '1' ;
	x2 = '2' ;
	x3 = '3' ;
	x9 = '9' ;
	c9 = 9 ;
	cv = 25 ;
	c0 = 0 ;
	c1 = 1 ;
	c2 = 2 ;
	c3 = 3 ;
	cb = ' ' ;
	cx = '!' ;
	cs = '*' ;
	cm = '-' ;
	cp = '+' ;
	cg = '>' ;
	cu = '=' ;
	cl = '<' ;
	sd = 80 ;
	ct = '\t' ;
	ce = '/' ;
	cd = '.' ;
	cq = '\'' ;
	i00 = 0 ;
	i01 = 1 ;
	i03 = 3 ;
	i10 = 10 ;
	i16 = 16 ;
	mn = 79 ; 
	ibk = iav[c3];
	xclose( f2 );
	xcreat( ibk, f2 );
	fl = pi = pb = ipc = po = ipt = ilb = pn = iuu = iln = ism = inl = 0 ;
	mk = 2 ;
	fmi(); 
}


fla()
{
	

	if(pl == mn)aa = TRUE ; else aa = FALSE ;
	if(aa)pl = c0 ; else pl++;
}


flb()
{
	

	while(TRUE){
		if(pl != pm)BOOL
		if(!aa)break;
		cc = gchar( f1 );
		if(er != c0)BOOL
		if(aa)cc = 0 ;
		ns[pm] = cc ;
		if(pm == mn)BOOL
		if(aa)pm = 0 ; else pm++;
		}
}


fli()
{
	

	pm = pl = bb = 0 ;
	while(TRUE){
		if(bb <= mn)BOOL
		if(er == c0)cc = TRUE ; else cc = FALSE ;
		aa = aa & cc ;
		if(!aa)break;
		cc = gchar( f1 );
		ns[bb] = cc ;
		bb++;
		}
}


flw()
{
	

	cc = ns[pl];
	while(TRUE){
		if(cc == '\n')BOOL
		if(aa){
			iln++;
			ism = 0 ;
			}
		else ;
		if(cc == ' ')bb = TRUE ; else bb = FALSE ;
		aa = aa | bb ;
		if(cc == '\t')bb = TRUE ; else bb = FALSE ;
		aa = aa | bb ;
		if(!aa)break;
		fla();
		cc = ns[pl];
		}
	flb();
}



fmc()
{
	
	iaa = mk ;
	imt = imt - iaa ;
	fmo();
	iaa = imt ;
	fmz();
}



fmd()
{
	
	iaa = mk ;
	imt = imt + iaa ;
	if(imd < imt)BOOL
	if(aa)puts("Destroy cell error\n");
	else ;
	iaa = imt ;
}



fme()
{
	
	
	fml();
	if(iaa != i00)ee = TRUE ; else ee = FALSE ;
	if(ee)return;
	imi[ibb] = imf ;
	while(TRUE){
		mc[imf] = cc ;
		imi[imf] = imx ;
		imf++;
		fmo();
		if(cc != c0)BOOL
		if(!aa)break;
		bb++;
		cc = os[bb];
		}
	iaa = imf ;
	iaa = imf ;
	idd = mk ;
	imf = imf + idd ;
	fmo();
	fmz();
}



fmh()
{
	
	imi[imf] = imb ;
	imb = imf ;
	imf = imf + iml ;
	mc[imf] = c0 ;
	imi[imf] = i00 ;
}



fmi()
{
	
	imm = imb = 0 ;
	imd = 3000 ; 
	imt = imd ;
	iml = 1 ;
	imf = imb + iml ;
	imx = i00 ;
	imi[imb] = i00 ;
	mc[imf] = c0 ;
	imi[imf] = i00 ;
}


fml()
{
	
	ibb = imb + iml ;
	bb = 0 ; 
	while(TRUE){
		cc = os[bb];
		dd = mc[ibb];
		if(cc == dd)ee = TRUE ; else ee = FALSE ;
		if(ee){ 
			if(cc == c0)ee = TRUE ; else ee = FALSE ;
			if(ee){ 
				iaa = ibb + i01 ;
				goto loc77 ;
				}
			else ;
			ibb++;
			bb++;
			}
		else { 
			iaa = imi[ibb];
			if(iaa == imx)ee = TRUE ; else ee = FALSE ;
			if(ee){ 
				iaa = i00 ;
				goto loc77;
				}
			else ;
			ibb = iaa ;
			}
		if(ibb < imf)ee = TRUE ; else ee = FALSE ;
		if(!ee)break;
		}
	iaa = i00 ; 
loc77:
	aa = aa ;
}



fmo()		
{	
	
	if(imt < imf)BOOL
	if(aa){
		puts("NO Memory space left, increase array sizes.\n");
		exit();
		}
	else ;
	if(imm < imf)BOOL
	if(aa)imm = imf ;
}



fmp()
{
	
	if(imb != i00)BOOL
	if(aa){
		imf = imb ;
		imb = imi[imb];
		}
	else {
		imf = iml ;
		mc[imf] = c0 ;
		imi[imf] = i00 ;
		}
}


fms()
{
	
	os[po] = c0 ;
	imz = imb ;
	while(TRUE){
		fml();
		if(iaa == i00)ee = TRUE ; else ee = FALSE ;
		imb = imi[imb] ;
		if(imb != i00)dd = TRUE ; dd = FALSE ;
		cc = ee & dd ;
		if(!cc)break ;
		}
	imb = imz ;
}




fmz()
{
	
	bb = 0 ;
	idd = iaa ;
	while(TRUE){
		if(bb < mk)BOOL
		bb++;
		if(!aa)break;
		mc[idd] = c0 ;
		imi[idd] = i00 ;
		idd++;
		}
}




fpn()
{
	
	fds();
	while(TRUE){
		ibb = nd ;
		if(i00 < ibb)BOOL
		if(!aa)break;
		nd-- ;
		aa = ds[nd];
		putchar( aa );
		}
	putchar(' ');
}



fpy()
{
	
	if(yp == c0)BOOL
	if(aa){
		puts("Y stacker\n");
		yp = c1 ;
		fl = 0 ; 
		}
	else ;
	yp-- ;
}






o();
		if(cc != c0)BOOL
		if(!aa)break;
		bb++;
		cc = os[bb];