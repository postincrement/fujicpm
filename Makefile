TEST.COM: test.c
	zcc +cpm -O2 test.c --list -o TEST -create-app

clean:
	rm -f TEST.COM TEST *.lis
