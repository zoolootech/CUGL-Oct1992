.SUFFIXES : .st .tst

TESTS= basic.st blocks.st fork.st new.st super.st copy.st num.st \
	file.st primes.st collect.st 4queen.st phil.st sim1.st sim2.st


.st.tst:
	\lst\sources\lst -m $*.st <in 

install:
	echo Performing Self Checking Tests
        make basic.tst
	make blocks.tst
	make fork.tst
	make new.tst
	make super.tst
	make copy.tst
	make num.tst
	make file.tst
	make primes.tst
	make collect.tst
	make 4queen.tst
	echo The following produce cycles, thus have nonzero differences
	make phil.tst
	echo Differences in random numbers may change results in following
	make sim1.tst
	make sim2.tst
	echo Finished Self Checking Tests
	
