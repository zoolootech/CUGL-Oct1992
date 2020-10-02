
#   Sample AWK program showing the modification of field variables

BEGIN			{ FS = OFS = "\t" }

$4 == "North America"	{ $4 = "NA" }

$4 == "South America"	{ $4 = "SA" }

			{ print }
