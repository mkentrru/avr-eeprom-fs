/*
 * kernel.c
 *
 * Created: 9/6/2021 8:57:07 PM
 *  Author: mkentrru
 */ 

#include "kernel.h"

//void init_pipe (pipe * p, void * s){
	//p->s = s;
//}

void init_task (task_id_t tid, task_exec * f, task_state TS_FLAGS){
	task * t = (tasks + tid);
	// Task function
	t->f = f;
	
	// Task state flags
	t->s = TS_FLAGS;
}


void init_bind (struct ibind * ib, ibind_counter ib_counter, ibind_handler * h){
	ib->c = ib_counter; // int counter
	ib->h = h; // int handler
}

void init_ihandler (ihandler_id_t hid, struct ibind * ib, ihandler_size hs){
	ihandler * h = (ihandlers + hid);
	h->b = ib; // handler binds
	h->c = hs; // binds count
}

void local_ihandler (ihandler * h){
	for (struct ibind * b = h->b; b < h->b + h->c; b++){
			// disabled: b->c == 0
			if (b->c && !--(b->c)) b->c = b->h();
		
	}
}


void enable_bind (struct ibind * b, ibind_counter c){
	b->c = c;
}

void disable_bind (struct ibind * b){
	b->c = bind_disabled;
}

//#include "uart.h"
task * current_task;
task_id_t current_task_id;
void run_kernel (task_count_t task_count){

	while (1){
		
		// bool running = false;
		for (current_task = tasks, current_task_id = 0;
		current_task < tasks + task_count;
		current_task++, current_task_id++
		){
			if (current_task->s & TS_FLAGS_ALLOWED) {
				current_task->f();
				// running = true;
				if (current_task->s & TS_FLAGS_ALLOWED_ONCE){
					current_task->s &= ~TS_FLAGS_ALLOWED;
					//put_string("Auto disabe: ", _false);
					//put_hex_byte (current_task_id);
					//new_line ();
				}
			}
		}
		// if (!running)	_SLEEP();?
	}
}

void global_ihandler (ihandler_id_t hid){
	breg cSREG = SREG;
	local_ihandler(ihandlers + hid);
	SREG = cSREG;
}

_bool interrupts__last_state = _false;

void enable_interrupts (_bool force){
	// if forcing to enable or last state
	if (force || interrupts__last_state) {
		sei();
		interrupts__last_state = _true;
	}
}


void disable_interrupts (){
	cli();
}


void lock_ext (struct ibind * b, ibind_counter c){
	ext_locked = _true;
	if (b){
		b->c = c;
	}
}

void unlock_ext (){
	ext_locked = _false;
}

void script__init (script * s, script_id_t size, void * data, script_result (** f) (struct script *)) {
	
	s->size = size;
	s->current = 0;
	s->data = data;
	s->f = f;
	
}

void script__step (script * s) {
	byte_t res = 0;
	if ( s->current < s->size ) {
		
		res = s->f[s->current](s);
		s->current += res;
	}
	
	if ( s->current >= s->size || res == script__exit ){
		s->current = 0;
		disable_current_task ();
	}
	
}


void disable_current_task () {
	//put_string("disabling: ", _false);
	//put_hex_byte (current_task_id);
	//new_line ();
	(tasks + current_task_id)->s &= ~TS_FLAGS_ALLOWED;
}

//#include "uart.h"

void allow_task (task_id_t id) {
	//put_string("allowing: ", _false);
	//put_hex_byte (id);
	//new_line ();

	(tasks + id)->s |= TS_FLAGS_ALLOWED;
}

void allow_script (task_id_t id, void (* script_task) ()) {
	//put_string("allowing: ", _false);
	//put_hex_byte (id);
	//new_line ();
	
	(tasks + id)->f = script_task;
	(tasks + id)->s |= TS_FLAGS_ALLOWED;
}



