/*
 * kernel.h
 *
 * Created: 9/6/2021 6:33:59 PM
 *  Author: mkentrru
 */ 


#ifndef KERNEL_H_
#define KERNEL_H_


#include "_shared_macro.h"
#include <avr/interrupt.h>
#include <avr/io.h>



//typedef enum{
	//MS_STOPPED,
	//MS_COUNTING
//} macro_state;



	// Tasks structures
#define TS_FLAGS_DISABLED		0x00
#define TS_FLAGS_ALLOWED		_sf(0)
#define TS_FLAGS_ALLOWED_ONCE	_sf(1)
#define TS_FLAGS_SELF_CONTROLED	_sf(2)
typedef byte_t task_state;

typedef void task_exec ();

typedef byte_t task_id_t;
typedef task_id_t task_count_t;
typedef struct {
	task_state s;
	task_exec * f;
} task;

// #define allow_task(id) (tasks + id)->s |= TS_FLAGS_ALLOWED;
extern task * current_task;
	// Interruption
typedef byte_t ihandler_id_t;

typedef uint16_t ibind_counter;

typedef byte_t ihandler_size;

typedef ibind_counter (ibind_handler) (void);
#define bind_disabled 0

struct ibind{
	ibind_counter c;	// count interruptions before handling
	ibind_handler * h;	// local handler
};

typedef struct{
	struct ibind * b;	// binds
	ihandler_size c;	// count of binds
} ihandler;

#define instant_bind ((ibind_counter) 0x01)
#define disabled_bind ((ibind_counter) 0x00)

// Scripts

typedef byte_t script_result;
#define script__next	1
#define script__prev	-1
#define script__stay	0
#define script__exit	0xFF
#define script__error	script__exit

typedef byte_t script_id_t;

typedef struct script{
	
	script_id_t size;
	script_id_t current;
	
	void * data;
	
	script_result (** f) (struct script *);
} script;

void script__init (script * s, script_id_t size, void * data, script_result (** f) (struct script *));
void script__step (script * s);



	// Kernel functions
	
void init_task (task_id_t, task_exec, task_state);
void allow_task (task_id_t id);
void allow_script (task_id_t id, void (* script_task) ());

void init_bind(struct ibind *, ibind_counter, ibind_handler *);
void init_ihandler(ihandler_id_t, struct ibind *, ihandler_size);

void local_ihandler(ihandler *);

extern _bool interrupts__last_state;
void enable_interrupts (_bool force);
void disable_interrupts ();

void enable_bind (struct ibind * b, ibind_counter c);
void disable_bind (struct ibind * b);

void lock_ext (struct ibind *, ibind_counter);
void unlock_ext ();

void run_kernel (task_count_t);



	/* User kernel configuration:
		1. Setup pipes
		2. Setup interruptions:
			2.1 Init binds
			2.2 Init handlers
		3. Setup tasks
	*/
	
	/*
	Kernel units defined by user
	*/
	
#define KERNEL_VAR
#define PIPE_VAR

// extern KERNEL_VAR macro_state MS;

extern KERNEL_VAR task tasks[];
extern KERNEL_VAR ihandler ihandlers [];

extern KERNEL_VAR _bool ext_locked; // to lock INT0 and etc.

	// User defined functions

void setup_environment ();

// void configure_kernel ();
void setup_kernel ();

// void configure_hardware ();
void setup_hardware ();

void reset_hardware ();

void get_predefined_configuration ();

void switch_macro_state ();

void global_ihandler (ihandler_id_t);


extern task_id_t current_task_id;
void disable_current_task ();

#endif /* KERNEL_H_ */