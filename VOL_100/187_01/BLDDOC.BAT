copy \util\eject temp.bdc
arc v \c\csubrs.arc >>temp.bdc
grep -f ^..@@ *.a >>temp.bdc
grep -f ^..@@ *.c >>temp.bdc
chg ^File#.* \f&\n\n <temp.bdc >temp.bd2
chg ^.*$ ####& <temp.bd2 >>csubrs.doc
