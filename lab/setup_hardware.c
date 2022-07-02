/*
 * setup_hardware.c
 *
 * Created: 9/15/2021 12:57:23 AM
 *  Author: mkentrru
 */ 
#include "lab.h"

void init_buttons (){
	DDRD &= ~(1<<PD2);
	PORTD |= (1<<PD2);
	
	// DDRD	dropflag	_df(PD2);
	// PORTD	setflag		_sf(PD2);
	
	MCUCR	setflag		_sf(ISC01); // from 1 to 0 is int
	GICR	setflag		_sf(INT0);
}


void init_timer0 (){
	// Timer0 // TIMER0_OVF_vect
	// Normal mode: TOV0 is set on TOP (0xFF)
	//WGM00 = 0;
	//WGM01 = 0;
	//TCCR0	dropflag	_df(WGM00) & _df(WGM01);
	// Compare Output Mode: OC0 is disconnected
	//COM01 = 0;
	//COM00 = 0;
	//TCCR0	dropflag	_df(COM01) & _df(COM00);
	// Clock Select:
	//CS02 = 1;
	//CS01 = 0;
	//CS00 = 0;
	// 010 - /8 prescaler
	// 011 - /64 pre => 488 HZ
	// 100 - /256 pre => 122 Hz
	// 101 - /1024 pre => 30 Hz
	TCCR0	setflag		_sf(CS01);
	TCCR0	setflag		_sf(CS00);
	
	
	// Interrupt mask
	//OCIE0 = 0; // Compare Match Interrupt Enable
	//TOIE0 = 1; // Overflow Interrupt Enable
	TIMSK	setflag		_sf(TOIE0);
	//TIMSK	dropflag	_df(OCIE0);
}

void set_default_ports () {
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
}

void setup_hardware (){
	set_default_ports ();
	init_buttons ();
	init_default_ssi ();
	init_timer0 ();
	init_default_uart ();
}