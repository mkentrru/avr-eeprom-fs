/*
 * lab.h
 *
 * Created: 9/15/2021 12:50:29 AM
 *  Author: mkentrru
 */ 


#ifndef LAB_H_
#define LAB_H_

#include "../kernel/kernel.h"

#include "../kernel/filesystem/filesystem.h"

#include "../kernel/uart.h"
#include "../kernel/ssi.h"

#include "../kernel/_buffer.h"

#include "user_tasks.h"

#include <avr/pgmspace.h>

// kernel
#define HARDWARE_TASKS	(1) // MEM_TASK_ID - fs task

#define KERNEL_TASKS	(3) //:
#define t_input_handler			0 + HARDWARE_TASKS
	void input_handler_exec ();
#define fs_script_task			1 + HARDWARE_TASKS // fs scripts
#define inter_script_task		2 + HARDWARE_TASKS // interaction scripts

#define K_TASK_COUNT (HARDWARE_TASKS + KERNEL_TASKS)

// 
#define TIMER0_HZ 488
#define HZTOC(a,t) (t/a)
#define K_INT_COUNT 1
#define timer			0
extern struct ibind timer_binds[];
	#define timer_binds_count	4
	
		#define timer__ssi_cd				0
		#define timer__ssi_cd_counter		HZTOC(200, TIMER0_HZ)
		ibind_counter						timer__ssi_cd_exec ();
		#define timer__ssi_cd_bind (timer_binds + timer__ssi_cd)
		
		#define timer__ioport_check			1
		#define timer__ioport_check_counter	HZTOC(15, TIMER0_HZ)
		ibind_counter						timer__ioport_check_exec ();
		#define timer__ioport_check_bind (timer_binds + timer__ioport_check)
		
		#define timer__animation			2
		#define timer__animation_counter	HZTOC(0.7, TIMER0_HZ)
		ibind_counter						timer__animation__exec ();
		#define timer__animation_bind (timer_binds + timer__animation)
		
		#define timer__ext_int_cd			3
		#define timer__ext_int_cd_counter	HZTOC(1, TIMER0_HZ)
		ibind_counter						timer__ext_int_cd__exec ();
		#define timer__ext_int_cd_bind (timer_binds + timer__ext_int_cd)

void external_interrupts__cooldown ();

#define enable_int0		GICR	setflag		_sf(INT0);
#define disable_int0	GICR	dropflag	_df(INT0);

#endif /* LAB_H_ */