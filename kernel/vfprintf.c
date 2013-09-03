#include "kernel.h"

void
putc(void (*printer)(char), char c){
    printer(c);
}

void
printint(void (*printer)(char), int xx, int base, int sgn){
    static char digits[]="0123456789ABCDEF";
    char buf[16];
    int i, neg=0;
    uint_32 x;

    if(sgn && xx < 0){
	neg=1;
	x=-xx;
    }
    else x=xx;

    i=0;
    
    do{
	buf[i++]=digits[x % base];
    }while((x /= base) != 0);

    if(neg)
	buf[i++]='-';

    while(--i >= 0)
	putc(printer, buf[i]);
}

void
vfprintf(void (*printer)(char), const char *ctl, void **args) {
    int i=0;
    int c=0;
    int state=0;
    char *s;

    for(i=0; ctl[i]; i++){
	c=ctl[i] & 0xff;
	if(state == 0){
	    if(c == '%')
		state='%';
	    else putc(printer, c);
	}
	else if(state == '%'){
	    if(c == 'd'){
		printint(printer, (int)*args, 10, 1);
		args++;
	    }
	    else if(c == 'x' || c == 'p'){
		printint(printer, (int)*args, 16, 0);
		args++;
	    }
	    else if(c == 's'){
		s=(char*)*args;
		args++;
		while(*s != 0){
		    putc(printer, *s);
		    s++;
		}
	    }
	    else if(c == 'c'){
		putc(printer, *(char*)args);
		args++;
	    }
	    else if(c == '%')
		putc(printer, c);
	    else {
		putc(printer, '%');
		putc(printer, c);
	    }
	    state=0;
	}
    }
}

