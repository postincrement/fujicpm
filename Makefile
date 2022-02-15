all: cpm unix

cpm: TEST.COM FUJI.COM

unix: fuji tnfsc

#######################################

TEST.COM: test.c
	zcc +cpm -O2 test.c --list -o TEST -create-app
	rm -f FUJI

FUJI.COM: fuji.c
	zcc +cpm -O2 fuji.c --list -o FUJI -create-app
	rm -f FUJI

#######################################

fuji: fuji.c
	cc -o fuji fuji.c

tnfsc: tnfsc.c
	cc -o tnfsc tnfsc.c

#######################################

clean:
	rm -f TEST.COM TEST 
	rm -f FUJI.COM FUJI
	rm -f *.lis
