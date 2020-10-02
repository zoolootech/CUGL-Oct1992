:
#	name:		word.ksh
#	purpose:	Outputs a sorted list of each word in a text file
#
USAGE='Usage: word < text > wordlist'

tr -cs "[A-z]" "\012" |
sort |
uniq
