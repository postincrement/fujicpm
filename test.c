
//	zcc +cpm -v -O2 test.c --list -o test -create-app 

#pragma output nostreams 
#pragma output nofileio
#pragma output noprotectmsdos
#pragma output nogfxglobals 
#pragma output CRT_ENABLE_COMMANDLINE=0
#pragma output CRT_OPEN_MAX=0
#pragma output CLIB_OPEN_MAX=0
#pragma output CRT_INITIALIZE_BSS=0

#include <cpm.h>

static const char helloWorld[] = "hello, world$";

void main()
{
  bdos(9, &helloWorld);
}
