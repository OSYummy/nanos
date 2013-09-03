#include "kernel.h"

void*
memset(void* s, int c, uint_32 n){
    const unsigned char uc=c;
    unsigned char *su;
    for(su=s; 0<n; ++su, --n)
	*su=uc;
    return s;
}

char*
strcpy(char *s, const char *t){
    char *os;
    os=s;
    while((*s++ = *t++) !=0 );
    return os;
}
