#ifndef __MESSAGE_H__
#define __MESSAGE_H__

struct message {
    pid_t src;
    pid_t dest;
    uint_32 type;
    union{
	struct {
	    int_32 p1, p2, p3;
	} int_msg;
	struct {
	    pid_t pid;
	    void *buf;
	    uint_32 offeset, len;
	} dev_io;
    };
};


#endif
