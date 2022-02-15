test: test.c
	zcc +cpm -O2 test.c --list -o test -create-app
