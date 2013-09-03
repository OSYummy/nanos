#ifndef __PROCESS_H__
#define __PROCESS_H__

#define NR_PROC 64
#define KSTACKSIZE 1024

enum proc_state {unused=0, runnable, running, block};

struct pcb {
    void *esp;
    char kstack[KSTACKSIZE];	//kernel stack
    void *ustack;				//user statck
    pid_t pid;
    enum proc_state state;
    struct semaphore *mutex;
    struct message *msg;
    struct pcb* next;
    char name[16];				//process name(for debugging)
};

struct {
    struct pcb pcb[NR_PROC];
} pcb_table;

extern struct pcb *current_pcb;

extern boolean need_sched;

int create_kthread(void (*proc)(void), const char *t);

struct pcb* alloc_proc();

void init_proc();

int getpid();

#endif
