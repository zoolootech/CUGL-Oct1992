flags = /AS /W3 /DMSC=1 /c

.c.obj	:
	 cl $(flags) $*.c
	 lib  uclib -+$*.obj ;


a_circle.obj	 :	 a_circle.c

a_cylind.obj	 :	 a_cylind.c

a_ellips.obj	 :	 a_ellips.c

actdsply.obj	 :	 actdsply.c

act_page.obj	 :	 act_page.c

al_cone.obj	 :	 al_cone.c

alt_eq_t.obj	 :	 alt_eq_t.c

a_parall.obj	 :	 a_parall.c

a_rect.obj	 :	 a_rect.c

a_sphere.obj	 :	 a_sphere.c

at_cylin.obj	 :	 at_cylin.c

a_trap.obj	 :	 a_trap.c

at_cone.obj	 :	 at_cone.c

a_triang.obj	 :	 a_triang.c

a_tri3s.obj	 :	 a_tri3s.c

atsay.obj	 :	 atsay.c

bdr_clr.obj	 :	 bdr_clr.c

bios_dot.obj	 :	 bios_dot.c

blk_let.obj	 :	 blk_let.c

blk_str.obj	 :	 blk_str.c

blnkstr.obj	 :	 blnkstr.c

cgetchar.obj	 :	 cgetchar.c

c_gets.obj	 :	 c_gets.c

chng_cas.obj	 :	 chng_cas.c

clr_eol.obj	 :	 clr_eol.c

clr_kbuf.obj	 :	 clr_kbuf.c

clr_page.obj	 :	 clr_page.c

clr_win.obj	 :	 clr_win.c

cls.obj 	 :	 cls.c

cls_clr.obj	 :	 cls_clr.c

countch.obj	 :	 countch.c

c_printf.obj	 :	 c_printf.c

cputchar.obj	 :	 cputchar.c

cur_col.obj	 :	 cur_col.c

cur_off.obj	 :	 cur_off.c

cur_on.obj	 :	 cur_on.c

cur_rel.obj	 :	 cur_rel.c

cur_rest.obj	 :	 cur_rest.c

cur_row.obj	 :	 cur_row.c

cur_save.obj	 :	 cur_save.c

cur_star.obj	 :	 cur_star.c

cur_stop.obj	 :	 cur_stop.c

d2mmddyy.obj	 :	 d2mmddyy.c

day.obj 	 :	 day.c

day2jul.obj	 :	 day2jul.c

dayofwk.obj	 :	 dayofwk.c

day_of_y.obj	 :	 day_of_y.c

del_blnk.obj	 :	 del_blnk.c

del_ch.obj	 :	 del_blnk.c

del_lwh.obj	 :	 del_lwh.c

del_subs.obj	 :	 del_subs.c

del_twh.obj	 :	 del_twh.c

dx10cpi.obj	 :	 dx10cpi.c

dx12cpi.obj	 :	 dx12cpi.c

dx15cpi.obj	 :	 dx15cpi.c

dx8ln.obj	 :	 dx8ln.c

dx12ln.obj	 :	 dx12ln.c

dx120bs.obj	 :	 dx120bs.c

dxback.obj	 :	 dxback.c

dxbi.obj	 :	 dxbi.c

dxboldst.obj	 :	 dxboldst.c

dxbsend.obj	 :	 dxbsend.c

dxchwide.obj	 :	 dxchwide.c

dxclr1tb.obj	 :	 dxclr1tb.c

dxclrtab.obj	 :	 dxclrtab.c

dxdefwid.obj	 :	 dxdefwid.c

dxeject.obj	 :	 dxeject.c

dx_esc_y.obj	 :	 dx_esc_y.c

dx_esc_z.obj	 :	 dx_esc_z.c

dxff.obj	 :	 dxff.c

dxfor.obj	 :	 dxfor.c

dxhlf.obj	 :	 dxhlf.c

dxinit.obj	 :	 dxinit.c

dxlinsp.obj	 :	 dxlinsp.c

dxlmarg.obj	 :	 dxlmarg.c

dxpglen.obj	 :	 dxpglen.c

dxrhlf.obj	 :	 dxrhlf.c

dxrlf.obj	 :	 dxrlf.c

dxsetht.obj	 :	 dxsetht.c

dxshast.obj	 :	 dxshast.c

dxundend.obj	 :	 dxundend.c

dxundst.obj	 :	 dxundst.c

dxuni.obj	 :	 dxuni.c

dy_in_mo.obj	 :	 dy_in_mo.c

first_ch.obj	 :	 first_ch.c

fld_add.obj	 :	 fld_add.c

fld_all.obj	 :	 fld_all.c

fld_clea.obj	 :	 fld_clea.c

fld_dspl.obj	 :	 fld_dspl.c

fld_err.obj	 :	 fld_err.c

fld_hlp.obj	 :	 fld_hlp.c

fld_init.obj	 :	 fld_init.c

fld_make.obj	 :	 fld_make.c

fld_mess.obj	 :	 fld_mess.c

fld_read.obj	 :	 fld_read.c

fld_rjsp.obj	 :	 fld_rjsp.c

fld_rjz.obj	 :	 fld_rjz.c

fld_term.obj	 :	 fld_term.c

fld_valu.obj	 :	 fld_valu.c

fm_clear.obj	 :	 fm_clear.c

fm_proc.obj	 :	 fm_proc.c

fsub_str.obj	 :	 fsub_str.c

gcd.obj 	 :	 gcd.c

get_ca.obj	 :	 get_ca.c

getcha.obj	 :	 getcha.c

get_curs.obj	 :	 get_curs.c

get_mem.obj	 :	 get_mem.c

getmode.obj	 :	 getmode.c

get_psp.obj	 :	 get_psp.c

get_str.obj	 :	 get_str.c

getticks.obj	 :	 getticks.c

gliss.obj	 :	 gliss.c

gotorc.obj	 :	 gotorc.c

gr_bckgn.obj	 :	 gr_bckgn.c

gregoria.obj	 :	 gregoria.c

heap_map.obj	 :	 heap_map.c

inactdsp.obj	 :	 inactdsp.c

indexch.obj	 :	 indexch.c

indexr.obj	 :	 indexr.c

ins_ch.obj	 :	 ins_ch.c

insert_c.obj	 :	 insert_c.c

ins_item.obj	 :	 ins_item.c

ins_spc.obj	 :	 ins_spc.c

inv_att.obj	 :	 inv_att.c

isalt.obj	 :	 isalt.c

isblanks.obj	 :	 isblanks.c

iscaplk.obj	 :	 iscaplk.c

isctrl.obj	 :	 isctrl.c

isega.obj	 :	 isega.c

isleapyr.obj	 :	 isleapyr.c

islshift.obj	 :	 islshift.c

isnumlk.obj	 :	 isnumlk.c

isrshift.obj	 :	 isrshift.c

isscrlk.obj	 :	 isscrlk.c

is_space.obj	 :	 is_space.c

itmdsply.obj	 :	 itmdsply.c

jul2day.obj	 :	 jul2day.c

julian.obj	 :	 julian.c

kstate.obj	 :	 kstate.c

left_str.obj	 :	 left_str.c

line.obj	 :	 line.c

lprintf.obj	 :	 lprintf.c

lputchar.obj	 :	 lputchar.c

lputs.obj	 :	 lputs.c

make_itm.obj	 :	 make_itm.c

make_mnu.obj	 :	 make_mnu.c

mid_str.obj	 :	 mid_str.c

mk_att.obj	 :	 mk_att.c

m_btnpos.obj	 :	 m_btnpos.c

m_choice.obj	 :	 m_choice.c

m_flagde.obj	 :	 m_flagde.c

m_flagin.obj	 :	 m_flagin.c

m_ghpos.obj	 :	 m_ghpos.c

m_grbcnt.obj	 :	 m_grbcnt.c

m_glbcnt.obj	 :	 m_glbcnt.c

m_gvpos.obj	 :	 m_gvpos.c

m_llphpo.obj	 :	 m_llphpo.c

m_llpvpo.obj	 :	 m_llpvpo.c

m_llrhpo.obj	 :	 m_llrhpo.c

m_llrvpo.obj	 :	 m_llrvpo.c

m_lrphpo.obj	 :	 m_lrphpo.c

m_lrpvpo.obj	 :	 m_lrpvpo.c

m_lrrhpo.obj	 :	 m_lrrhpo.c

m_lrrvpo.obj	 :	 m_lrrvpo.c

mnudsply.obj	 :	 mnudsply.c

mnu_proc.obj	 :	 mnu_proc.c

m_2_row.obj	 :	 m_2_row.c

mk_offse.obj	 :	 mk_offse.c

month.obj	 :	 month.c

month_st.obj	 :	 month_st.c

monthyr.obj	 :	 monthyr.c

m_reset.obj	 :	 m_reset.c

m_setpos.obj	 :	 m_setpos.c

m_txtscu.obj	 :	 m_txtscu.c

nbr2wrds.obj	 :	 nbr2wrds.c

nbr_prns.obj	 :	 nbr_prns.c

numcols.obj	 :	 numcols.c

palette.obj	 :	 palette.c

phasor.obj	 :	 phasor.c

prn_6_ln.obj	 :	 prn_6_ln.c

prn_8_ln.obj	 :	 prn_8_ln.c

prn_10_c.obj	 :	 prn_10_c.c

prn_12_c.obj	 :	 prn_12_c.c

prn_bell.obj	 :	 prn_bell.c

prn_can.obj	 :	 prn_can.c

prncomof.obj	 :	 prncomof.c

prncomon.obj	 :	 prncomon.c

prndblof.obj	 :	 prndblof.c

prndblon.obj	 :	 prndblon.c

prndwof.obj	 :	 prndwof.c

prndwon.obj	 :	 prndwon.c

prnempof.obj	 :	 prnempof.c

prnempon.obj	 :	 prnempon.c

prn_ends.obj	 :	 prn_ends.c

prn_ff.obj	 :	 prn_ff.c

prn_read.obj	 :	 prn_read.c

prn_sub.obj	 :	 prn_sub.c

prn_sup.obj	 :	 prn_sup.c

prn_unof.obj	 :	 prn_unof.c

prn_unon.obj	 :	 prn_unon.c

prnwidof.obj	 :	 prnwidof.c

prnwidon.obj	 :	 prnwidon.c

put_ch.obj	 :	 put_ch.c

put_cha.obj	 :	 put_cha.c

putchr.obj	 :	 putchr.c

read_att.obj	 :	 read_att.c

read_ch.obj	 :	 read_ch.c

replacec.obj	 :	 replacec.c

right_st.obj	 :	 right_st.c

rjust.obj	 :	 rjust.c

scr_1ln.obj	 :	 scr_1ln.c

scrn_off.obj	 :	 scrn_off.c

scrn_on.obj	 :	 scrn_on.c

setcursh.obj	 :	 setcursh.c

setfreq.obj	 :	 setfreq.c

setmode.obj      :       setmode.c

setpage.obj	 :	 setpage.c

set_pixe.obj	 :	 set_pixe.c

sf_activ.obj	 :	 sf_activ.c

sf_att.obj	 :	 sf_att.c

sf_begfu.obj	 :	 sf_begfu.c

sf_endfu.obj	 :	 sf_endfu.c

sf_hlpta.obj	 :	 sf_hlpta.c

sf_inact.obj	 :	 sf_inact.c

sf_on_ke.obj	 :	 sf_on_ke.c

sf_messa.obj	 :	 sf_messa.c

sf_patt.obj	 :	 sf_patt.c

sf_promp.obj	 :	 sf_promp.c

sf_prot.obj	 :	 sf_prot.c

sf_usr_v.obj	 :	 sf_usr_v.c

sf_val.obj	 :	 sf_val.c

siren.obj	 :	 siren.c

shr_str.obj	 :	 shr_str.c

str_beg.obj	 :	 str_beg.c

strblnk.obj	 :	 strblnk.c

str_cent.obj	 :	 str_cent.c

str_cjst.obj	 :	 str_cjst.c

str_date.obj	 :	 str_date.c

str_end.obj	 :	 str_end.c

strip_nl.obj	 :	 strip_nl.c

str_left.obj	 :	 str_left.c

str_ljst.obj	 :	 str_ljst.c

str_mask.obj	 :	 str_mask.c

str_mon.obj	 :	 str_mon.c

strpad.obj	 :	 strpad.c

str_rev.obj	 :	 str_rev.c

str_righ.obj	 :	 str_righ.c

str_rjst.obj	 :	 str_rjst.c

str_save.obj	 :	 str_save.c

str_time.obj	 :	 str_time.c

strtolow.obj	 :	 strtolow.c

strtoup.obj	 :	 strtoup.c

sw_att.obj	 :	 sw_att.c

sw_ch.obj	 :	 sw_ch.c

sw_cha.obj	 :	 sw_cha.c

sw_str.obj	 :	 sw_str.c

sw_stra.obj	 :	 sw_stra.c

uc_beep.obj	 :	 uc_beep.c

ucdelay.obj	 :	 ucdelay.c

uc_error.obj	 :	 uc_error.c

uc_hline.obj	 :	 uc_hline.c

uc_init.obj	 :	 uc_init.c

uckey.obj	 :	 uckey.c

uc_ok.obj	 :	 uc_ok.c

ucpeek.obj	 :	 ucpeek.c

ucpeekb.obj	 :	 ucpeekb.c

ucpoke.obj	 :	 ucpoke.c

ucpokeb.obj	 :	 ucpokeb.c

uc_today.obj	 :	 uc_today.c

uc_tone.obj	 :	 uc_tone.c

uc_vline.obj	 :	 uc_vline.c

ucvpeek.obj	 :	 ucvpeek.c

ucvpoke.obj	 :	 ucvpoke.c

ucvpokeb.obj	 :	 ucvpokeb.c

unmk_att.obj	 :	 unmk_att.c

val_date.obj	 :	 val_date.c

val_mon.obj	 :	 val_mon.c

val_stat.obj	 :	 val_stat.c

v_cone.obj	 :	 v_cone.c

v_sphere.obj	 :	 v_sphere.c

vwarble.obj	 :	 vwarble.c

warble.obj	 :	 warble.c

w_ch.obj	 :	 w_ch.c

w_cha.obj	 :	 w_cha.c

wkday.obj	 :	 wkday.c

wkday_st.obj	 :	 wkday_st.c

w_strc.obj	 :	 w_strc.c

wn_addbg.obj	 :	 wn_addbg.c

wn_adden.obj	 :	 wn_adden.c

wn_addr.obj	 :	 wn_addr.c

wn_bclr.obj	 :	 wn_bclr.c

wn_bdr.obj	 :	 wn_bdr.c

wn_clear.obj	 :	 wn_clear.c

wn_check.obj	 :	 wn_check.c

wn_csr.obj	 :	 wn_csr.c

wn_char.obj	 :	 wn_char.c

wnclrerr.obj	 :	 wnclrerr.c

wn_delet.obj	 :	 wn_delet.c

wn_back.obj	 :	 wn_back.c

wn_dsply.obj	 :	 wn_dsply.c

wn_err.obj	 :	 wn_err.c

wn_getch.obj	 :	 wn_getch.c

wn_hide.obj	 :	 wn_hide.c

wn_inser.obj	 :	 wn_inser.c

wn_make.obj	 :	 wn_make.c

wn_margi.obj	 :	 wn_margi.c

wn_mkbdr.obj	 :	 wn_mkbdr.c

wn_move.obj	 :	 wn_move.c

wn_pchar.obj	 :	 wn_pchar.c

wn_posit.obj	 :	 wn_posit.c

wnprintf.obj	 :	 wnprintf.c

wn_front.obj	 :	 wn_front.c

wn_rposi.obj	 :	 wn_rposi.c

wn_scrol.obj	 :	 wn_scrol.c

wn_swap.obj	 :	 wn_swap.c

wn_unlis.obj	 :	 wn_unlis.c

wn_wnclr.obj	 :	 wn_wnclr.c

wv_str.obj	 :	 wv_str.c

year.obj	 :	 year.c

zeller.obj	 :	 zeller.c
