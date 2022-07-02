/*
 * fs_memory.c
 *
 * Created: 11/1/2021 9:11:25 PM
 *  Author: mkentrru
 */ 

#include "memory.h"

// Eeprom memory
byte_t mem_busy__read (memaddress a) {
	while ( EECR & (1<<EEWE) );
	
	EEAR = a;
	EECR |= (1<<EERE);
	
	return EEDR;
}

void mem_busy__read_area (memaddress a, memaddress size, byte_t * d) {
	while ( size ) {
		* d = mem_busy__read (a);
		d++; a++; size--;
	}
}

void mem_busy__write (memaddress a, byte_t d) {
	while(EECR & (1<<EEWE));
	
	EEAR = a;
	EEDR = d;
	
	disable_interrupts ();
	EECR |= (1<<EEMWE);
	EECR |= (1<<EEWE);
	enable_interrupts (_false);
}

void mem_busy__write_area (memaddress a, memaddress size, byte_t * d) {
	while ( size ) {
		mem_busy__write (a, * d);
		d++; a++; size--;
	}
}


void mem__read (memaddress a, task_id_t tid) {
	mem_quere.a = a;
	
	mem_quere.descr.to_read = 1;
	mem_quere.waiting = tid;
	
	start_mem
}
void mem__write (memaddress a, byte_t d, task_id_t tid) {
	mem_quere.a = a;
	mem_quere.d = d;
	mem_quere.descr.to_read = 0;
	mem_quere.waiting = tid;
	
	start_mem
}

_bool is_mem_task_done () {
	if ( mem_task.done >= mem_task.size ) return _true;
	return _false;
}


script_result mem_task_step__control () {
	if ( is_mem_task_done() ) {
		// awake the one who asked
		allow_task (mem_task.caller);
		return script__exit;
	}
	return script__next;
}

script_result mem_task_step__prepare () {
	// may be allowed instantly after mem_start
	disable_current_task ();
	if (mem_task.to_read == 1) {
		mem__read (mem_task.a, MEM_TASK_ID);
	}
	else {
		mem__write (mem_task.a, mem_task.d [mem_task.done], MEM_TASK_ID);
	}
	return script__next;
}

#define mem_script__check_done -2
script_result mem_task_step__cover () {
	if (mem_task.to_read) {
		mem_task.d [mem_task.done] = mem_quere.d;
	}
	
	mem_task.done++;
	mem_task.a++;
	return mem_script__check_done;
}

void mem__request (memaddress a, memaddress size, byte_t * d, task_id_t tid, _bool to_read) {
	if ( to_read ) mem_task.to_read = 1;
	else mem_task.to_read = 0;
	
	mem_task.a = a;
	mem_task.d = d;
	mem_task.done = 0;
	mem_task.size = size;
	mem_task.caller = tid;
	
	allow_task (MEM_TASK_ID);
	disable_current_task ();
}

#define mem__script_size 3
script_result (* mem_script_f [mem__script_size]) (struct script *)  = {
	mem_task_step__control,
	mem_task_step__prepare,
	mem_task_step__cover
};

void mem_script__init () {
	script__init (& mem_script, mem__script_size, _null, mem_script_f );
}

void mem_script_task () {
	script__step (& mem_script);
}

