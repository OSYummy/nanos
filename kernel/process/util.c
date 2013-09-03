#include "kernel.h"

int nextpid=0;
extern struct pcb* current_pcb;

void a();
void b();
void c();

int
create_kthread(void (*func)(void), const char* t){
    struct pcb* p=NULL;
    struct pcb* tail=current_pcb;
	struct TrapFrame* tf=NULL;

    if((p=alloc_proc()) == 0){
		panic("alloc_proc error!\n");
		return -1;
    }

    p->ustack=NULL;
    p->pid=nextpid++;
    p->state=runnable;
    
    memset(p->kstack, 0, KSTACKSIZE);
    p->esp=p->kstack+KSTACKSIZE;
    p->esp-=sizeof(struct TrapFrame);
    tf=(struct TrapFrame*)(p->esp);
	//pushal
    tf->eflags=0x01<<9;
    tf->cs=0x08;
    tf->eip=(uint_32)func;
    tf->irq=0x03e8;
	//pushl %esp
	p->esp-=4;
	p->esp=(void*)tf;
   
    p->next=current_pcb;
    strcpy(p->name, t);

    //insert thread into runnable list;
	while(tail->next != current_pcb)
	    tail=tail->next;
	tail->next=p;
    p->next=current_pcb;

    printk("%s is created.\n", p->name);

    return p->pid;
}

int 
create_idle(){
    struct pcb* p=NULL;

    if((p=alloc_proc()) == 0){
		panic("allocproc error!\n");
		return 0;
    }

    p->ustack=NULL;
    p->pid=nextpid++;
    p->state=runnable;
    strcpy(p->name, "idle");

	current_pcb=p;
	current_pcb->next=current_pcb;

    printk("idle is created.\n");
	return p->pid;
}

void
init_proc() {
	create_idle();
    create_kthread(a, "a");
    create_kthread(c, "c");
    create_kthread(b, "b");
}

struct pcb*
alloc_proc(void){
    struct pcb* p;

    for(p=pcb_table.pcb; p < &(pcb_table.pcb)[NR_PROC]; p++)
		if(p->state == unused) break;
    
    if(p == pcb_table.pcb+NR_PROC)
		return 0;
    else
		return p;
}

void
a(){
    while(1){
		printk("a");
		idle_cpu();
    }
}

void
b(){
    while(1){
		printk("b");
		idle_cpu();
    }
}

void
c(){
    while(1){
		printk("c");
		idle_cpu();
    }
}

int
getpid(){
    return current_pcb->pid;
}
