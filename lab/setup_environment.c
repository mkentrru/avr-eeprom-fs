/*
 * setup_environment.c
 *
 * Created: 9/15/2021 12:52:28 AM
 *  Author: mkentrru
 */ 

#include "lab.h"


// Kernel

KERNEL_VAR task tasks [K_TASK_COUNT];

KERNEL_VAR ihandler ihandlers [K_INT_COUNT];
KERNEL_VAR struct ibind timer_binds	[timer_binds_count];
// UART
byte_t uart_input_buf	[uart_input_size];
byte_t uart_output_buf	[uart_output_size];
byte_t uart_error_buf	[uart_error_size];

_c_buffer uart_input;
_c_buffer uart_output;
_c_buffer uart_error;


// FS
_fs_context		fs_context;
_mem_quere		mem_quere;
_mem_task		mem_task; // not kernel task

script mem_script;

script fs__ls__script;
script fs__cat__script;
script fs__cd__script;
script fs__rm__script;
script fs__info__script;
script fs__rm__script;
script fs__touch__script;
script fs__mkdir__script;

script passwd_input__script;
script passwd_change__script;
script passwd_init__script;

// configure variables and etc
void setup_environment (){
	interrupts__last_state = _false;
	// UART
	_c_buffer__init (&uart_input, uart_input_buf, uart_input_size - 1);
	_c_buffer__init (&uart_output, uart_output_buf, uart_output_size - 1);
	_c_buffer__init (&uart_error, uart_error_buf, uart_error_size - 1);
	
	// Scripts
	mem_script__init ();
}


void get_predefined_configuration (){
	
	// SSI
	_ssi__default_configuration ();
	
	// EEPROM and FS
	stop_mem
	
	// UART
	enable_input
}