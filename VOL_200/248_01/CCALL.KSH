:
#	name:		ccall.ksh
#	purpose:	Compile all objects and programs for spell
#	author:		Roberto Artigas Jr
#	compiler:	MS C 5.1
#	enviroment:	MKS Toolkit - (UNIX under DOS)
#
echo "\n"
ccllib check		# Common routine
echo "\n"
ccllib common		# Common routine
echo "\n"
ccllib file		# Common routine
echo "\n"
ccllib ibmpc		# Common routine
echo "\n"
ccllib mdict		# Common routine
echo "\n"
ccllib quick		# Common routine
echo "\n"
ccllib source		# Common routine
echo "\n"
ccllib user		# Common routine
echo "\n"
ccllib matches		# Common routine
echo "\n"
cclcdict cdict		# Dictionary builder
echo "\n"
cclddict ddict		# Dictionary displayer
echo "\n"
cclmerge dmerge		# Dictionary merger
echo "\n"
cclrepl replace		# Word alternatives
echo "\n"
cclspell main		# Speller proper
