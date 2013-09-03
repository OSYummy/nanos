#include "kernel.h"

struct pcb *current_pcb = NULL;

/* You interrupt handler (use add_intr_handle to register)
   just need to set "need_sched" to be TRUE,
   schedule() will be called each time before interrupt return. */
boolean need_sched = TRUE;

void
schedule(void) {
    if(!need_sched) return;
    
    if(current_pcb != NULL){
		current_pcb->state=runnable;
		current_pcb=current_pcb->next;
		current_pcb->state=running;
	}

//	need_sched=FALSE;
}
