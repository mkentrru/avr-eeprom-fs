/*
 * memory.h
 *
 * Created: 11/1/2021 9:15:15 PM
 *  Author: mkentrru
 */ 


#ifndef MEMORY_H_
#define MEMORY_H_

#include "avr/io.h"

#include "../_shared_macro.h"
#include "../kernel.h"

typedef uint32_t memaddress;

// busy EEPROM
byte_t mem_busy__read (memaddress a);
void mem_busy__read_area (memaddress a, memaddress size, byte_t * d);
void mem_busy__write (memaddress a, byte_t d);
void mem_busy__write_area (memaddress a, memaddress size, byte_t * d);

// not busy EEPROM
#define MEM_TASK_ID	0
void mem_script__init ();
void mem_script_task ();

void mem__request (memaddress a, memaddress size, byte_t * d, task_id_t tid, _bool to_read);



#define enable_eeprom_ready_int		EECR setflag _sf(EERIE);
#define disable_eeprom_ready_int	EECR dropflag _df(EERIE);

#define start_mem		enable_eeprom_ready_int
#define stop_mem		disable_eeprom_ready_int


typedef struct {
/*
	on interrupt i: // (EEPROM Ready Interrupt)
		if to_read:
			EEAR = a;
			EECR |= (1<<EERE);
			d = EEDR;
		else:
			EEAR = a;
			EEDR = d;
			EECR |= (1<<EEMWE);
			EECR |= (1<<EEWE);
		
		disable interrupt i
		
		allow task waiting
*/	
	struct {
		byte_t to_read:	1;
	} descr;
	
	task_id_t waiting;
	
	memaddress a;
	byte_t d;
	
} _mem_quere;

typedef struct {
	byte_t to_read:		1;
	
	memaddress a;
	memaddress size;
	memaddress done;
	
	byte_t * d;
	
	task_id_t caller;
} _mem_task;



extern _mem_quere mem_quere;
extern _mem_task mem_task;

/*
		MEMORY SCRIPT
*/

extern script mem_script;
void mem_script__init ();
void mem_script_task ();

#endif /* MEMORY_H_ */