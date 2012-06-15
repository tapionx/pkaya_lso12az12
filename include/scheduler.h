/* Esempio inizializzazione pcb_t 
	pcb_t *test = allocPcb();
	state_t *test_state = &(test->p_s);
	test_state->status = getSTATUS();
	test_state->status |= (STATUS_IEc|STATUS_TE)&~(STATUS_VMc|STATUS_KUc);
	test_state->reg_sp = RAMTOP-FRAME_SIZE;
	test_state->pc_epc = test_state->reg_t9 = (memaddr)test1;
	addReady(test); */

void addReady(pcb_t *proc);
void scheduler();
