/*
 * spbpu-4s-asvt-l11-new-kernel.c
 *
 * Created: 9/14/2021 11:35:04 PM
 * Author : mkentrru
 */ 

#include "lab/lab.h"




int main (void)
{
	setup_environment ();
	
	setup_kernel ();
	
	setup_hardware ();
	
	// Filesystem
	init_filesystem ();
	
	P__put__string (PSTR("Filesystem was initialized."), _true);
	
	get_predefined_configuration ();
	
	// ready to run...
	enable_interrupts (_true);
	
	
	if ( fs__check__cluster_chain () )
		P_locking_output__string (PSTR("Chain is broken!"), _true);
	else
		P_locking_output__string (PSTR("No chain problems detected."), _true);
	
	P_locking_output__string (PSTR("Let us begin."), _true);
	
	locking_output__prompt ();
	
	
	run_kernel (K_TASK_COUNT);
	
	
	//// DDRA = 0xFF;
	//
	//DDRB = 0xFF;
	//PORTB = 0;
	//uint16_t d = 0;
	//while (1){
		//if(d++ == 0){
			//PORTB++;
			//if ( PORTB % 0 x10 == 0 ) {
				//locking_output__hex_buffer ((byte_t *) & fs_context, sizeof(_fs_context));
			//}
		//}
	//}
}

