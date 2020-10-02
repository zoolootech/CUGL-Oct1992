rem  CLL.BAT copies to RAM disk all files needed for the CLL application
rem  and calls up dbase2. Upon returning to DOS any changed files are saved.
if exist c:mims.prg goto dbase2
copy b:mims.prg     e:
copy b:cll1417.prg  e:
copy b:updatecl.prg e:
copy b:sayget.prg   e:
copy b:saysay.prg   e:
copy b:refresh.prg  e:
copy b:compubun.prg e:
copy b:labelscl.prg e:
copy b:label.prg    e:
copy b:rostercl.prg e:
copy b:rostrfro.prg e:
copy b:filistcl.prg e:
copy b:cllname.ndx  e:
copy b:bunzip.ndx   e:
copy b:cll1417.dbf  e:
:dbase2
dir e:>e:old.dir
dbase e:mims
dir e:>e:new.dir
scanfils
savefils
