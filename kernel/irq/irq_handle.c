#include "kernel.h"

#define NR_IRQ_HANDLE 32

/* Structures below is a linked list of function pointers indicating the
   work to be done for each interrupt. Routines will be called one by
   another when interrupt comes.
   For example, the timer interrupts are used for alarm clocks, so
   device driver of timer will use add_irq_handle to attach a simple
   alarm checker. Also, the IDE driver needs to write back dirty cache
   slots periodically. So the IDE driver also calls add_irq_handle
   to register a handler. */

struct IRQ_t {
	void (*routine)(void);
	struct IRQ_t *next;
};

extern boolean need_sched;

static struct IRQ_t handle_pool[NR_IRQ_HANDLE];
static struct IRQ_t *handles[NR_IRQ];
static int handle_count = 0;

void
add_irq_handle( int irq, void (*func)(void) ) {
	struct IRQ_t *ptr;
	if (handle_count > NR_IRQ_HANDLE) {
		panic("Too many irq registrations!");
	}
	ptr = &handle_pool[handle_count ++]; /* get a free handler */
	ptr->routine = func;
	ptr->next = handles[irq]; /* insert into the linked list */
	handles[irq] = ptr;
}

void schedule();

void irq_handle(struct TrapFrame *tf) {
	int irq = tf->irq;
	current_pcb->esp = tf;

	if (irq < 0) {
		panic("Unhandled exception!");
	}
    
	if (irq < 1000 ) {
		panic("Unexpected exception #%d\n", irq);
	} else if (irq >= 1000) {
		int irq_id = irq - 1000;
		struct IRQ_t *f = handles[irq_id];

		while (f != NULL) { /* call handlers one by one */
			f->routine(); 
			f = f->next;
		}
	}

	schedule();
}

void
print_register(){
	uint_32 eax, ebx, ecx, edx, esp, ebp, esi, edi;

	asm volatile("movl %%eax, %0;":"=r"(eax));
	asm volatile("movl %%ebx, %0;":"=r"(ebx));
	asm volatile("movl %%ecx, %0;":"=r"(ecx));
	asm volatile("movl %%edx, %0;":"=r"(edx));
	asm volatile("movl %%esp, %0;":"=r"(esp));
	asm volatile("movl %%ebp, %0;":"=r"(ebp));
	asm volatile("movl %%esi, %0;":"=r"(esi));
	asm volatile("movl %%edi, %0;":"=r"(edi));

	printk("register eax: %p ebx: %p ecx: %p edx: %p esp: %p ebp: %p esi: %p edi: %p\n", eax, ebx, ecx, edx, esp, ebp, esi, edi);
}
