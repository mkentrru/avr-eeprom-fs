/*
 * setup_kernel.c
 *
 * Created: 9/15/2021 1:22:05 AM
 *  Author: mkentrru
 */ 

#include "lab.h"

void setup_kernel (){
	
	// to run mem tasks
	// init_task (MEM_TASK_ID, mem_script_task, TS_FLAGS_SELF_CONTROLED);
	init_task (MEM_TASK_ID, mem_script_task, TS_FLAGS_DISABLED);
	
	// default input
	init_task (t_input_handler,	input_handler_exec,	TS_FLAGS_ALLOWED_ONCE);
	
	// Filesystem
	// init_task (fs_script_task, _null, TS_FLAGS_SELF_CONTROLED);
	init_task (fs_script_task, _null, TS_FLAGS_DISABLED);
	
	// interactions: animations on passwd input and etc
	// init_task (inter_script_task, _null, TS_FLAGS_SELF_CONTROLED);
	init_task (inter_script_task, _null, TS_FLAGS_DISABLED);
	
	
	
	init_ihandler(timer, timer_binds, timer_binds_count);
		init_bind(timer_binds + timer__ssi_cd, timer__ssi_cd_counter, timer__ssi_cd_exec);
		init_bind(timer_binds + timer__ioport_check, bind_disabled, timer__ioport_check_exec);
		init_bind(timer_binds + timer__animation, bind_disabled, timer__animation__exec);
		init_bind(timer_binds + timer__ext_int_cd, bind_disabled, timer__ext_int_cd__exec);
	
}
