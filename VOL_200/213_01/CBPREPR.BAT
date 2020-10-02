;;CBPREPR.BAT       VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:02 PM
;;
;;Submit file for filtering C code through cbprepr then cbr
;;
;;Usage:  batchr  cbprepr  input_file  output_file
;;
;;By J.A. Rupley, Tucson Arizona
;;Standard .bat = .sub = .zex command code
;;
cbprepr <$1.c +$2.c
cbr <$2.c +$2.c
cr $2
;;diff $1.com,$2.com
 