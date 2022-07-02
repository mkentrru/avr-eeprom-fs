/*
 * setup_interrupts.c
 *
 * Created: 9/15/2021 1:27:44 AM
 *  Author: mkentrru
 */ 

#include "lab.h"

ISR(USART_RXC_vect){	
	byte_t a = UDR;
	
	if (a == 0x08 || a == 0x7F) { // backspace
		_c_buffer__undo_input (_stdin);
	}
	else {
		_c_buffer__push(_stdin, a);
		
		if (a == _IEB) {
			_c_buffer__push (_stdout, _NLB);
			disable_input
			allow_task(t_input_handler);
		}
	}
	
	if(! _c_buffer__push(_stdout, a)){
		enable_output
	}

}

ISR(USART_UDRE_vect){
	byte_t a = 0;
	
	if (_c_buffer__pop(_stderr, & a) && _c_buffer__pop(_stdout, & a))
		disable_output
	else
		UDR = a;
	
}

ISR(TIMER0_OVF_vect){
	global_ihandler(timer);
}

ISR(INT0_vect){
	external_interrupts__cooldown ();
	fs__ssi__switch ();
	//locking_output__hex_byte (output_ssi_description[3]);
	//locking_output__hex_byte (output_ssi_description[2]);
	//locking_output__hex_byte (output_ssi_description[1]);
	//locking_output__hex_byte (output_ssi_description[0]);
	//locking_output__new_line ();
	
	// locking_output__hex_buffer (output_ssi_description, _SSI_SIZE);
}

ibind_counter timer__ext_int_cd__exec () {
	enable_int0
	return bind_disabled;
}


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

ISR(EE_RDY_vect){
	
	if ( mem_quere.descr.to_read ) {
		EEAR = mem_quere.a;
		EECR |= (1<<EERE);
		mem_quere.d = EEDR;
	}
	else {
		EEAR = mem_quere.a;
		EEDR = mem_quere.d;
		EECR |= (1<<EEMWE);
		EECR |= (1<<EEWE);
	}
	stop_mem
	allow_task (mem_quere.waiting);
}

