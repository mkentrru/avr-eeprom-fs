/*
 * uart.c
 *
 * Created: 9/15/2021 1:33:16 AM
 *  Author: mkentrru
 */ 

#include "uart.h"

// #define BAUD 9600

void init_default_uart (){
	UCSRB	|=	(1<<RXEN) | (1<<TXEN);
	UCSRC	|=	(1<<URSEL)| (1<<UCSZ0) | (1<<UCSZ1);
	UBRRL	=	51; //(8000000)/(16 * BAUD) - 1; // 51; // page 143
}

void put_buffer (byte_t * buf, byte_t s){
	// _c_buffer__push(_stdout, _IBB);
	
	for (byte_t pos = 0; pos < s; pos++){
		_c_buffer__push(_stdout, buf[pos]);
	}
	_c_buffer__push(_stdout, _IEB);
	
	enable_output
}

byte_t get_hex_char (byte_t b) {
	if (b > 0x10) return '.';
	if (b < 0x0A) return ('0' + b);
	return ('A' + (b - 0x0A));
}

void put_hex_buffer (byte_t * buf, byte_t s) {
	for (byte_t pos = 0; pos < s; pos++){
		byte_t b = buf[pos];
		_c_buffer__push(_stdout, get_hex_char( (b & 0xF0)>>4 ));
		_c_buffer__push(_stdout, get_hex_char( (b & 0x0F) ));
		_c_buffer__push(_stdout, ' ');
		if ( pos % 8 == 7) {
			_c_buffer__push(_stdout, '\n');
			_c_buffer__push(_stdout, '\r');
		}
	}
	
	_c_buffer__push(_stdout, '\n');
	_c_buffer__push(_stdout, '\r');
	
	enable_output
}

void put_string (const char * buf, _bool new_line){
	// _c_buffer__push(_stdout, _IBB);
	
	for (byte_t pos = 0;  buf[pos]; pos++){
		_c_buffer__push(_stdout, buf[pos]);
	}
	
	if (new_line) {
		_c_buffer__push(_stdout, '\n');
		_c_buffer__push(_stdout, '\r');
	}
	
	
	enable_output
}


//void put_error (byte_t err){
	//
	//// _c_buffer__push(_stderr, _OIBB);
	//// _c_buffer__push(_stderr, err);
	//// _c_buffer__push(_stderr, _OIEB);
	//
	//enable_output
//}
//
//void put_extra_error (byte_t err, byte_t * buf){
	//
	//_c_buffer__push(_stderr, _OIBB);
	//_c_buffer__push(_stderr, err);
	//
	//if (buf){
		//for (byte_t pos = 0;  buf[pos]; pos++){
			//_c_buffer__push(_stderr, buf[pos]);
		//}
	//}
	//_c_buffer__push(_stderr, _OIEB);
//
	//
	//enable_output
//}

byte_t get_byte (){
	return _c_buffer_slice (_stdin);
}



void put_byte (byte_t b) {
	_c_buffer__push(_stdout, b);
	enable_output
}

void put_hex_byte (byte_t b) {
	put_byte ( get_hex_char( (b & 0xF0)>>4 ) );
	put_byte ( get_hex_char( (b & 0x0F) ) );
	enable_output
}

void new_line () {
	_c_buffer__push(_stdout, '\n');
	_c_buffer__push(_stdout, '\r');
	enable_output
}

//void put_buffer (byte_t * buf, byte_t s){
	//// _c_buffer__push(_stdout, _IBB);
	//
	//for (byte_t pos = 0; pos < s; pos++){
		//_c_buffer__push(_stdout, buf[pos]);
	//}
	//_c_buffer__push(_stdout, _IEB);
	//
	//enable_output
//}




void locking_output__byte (byte_t b) {
	locking_output(b);
	enable_output
}

void locking_output__new_line () {
	locking_output__byte ('\n');
	locking_output__byte ('\r');
}

void locking_output__hex_byte (byte_t b) {
	locking_output__byte ( get_hex_char( (b & 0xF0)>>4 ) );
	locking_output__byte ( get_hex_char( (b & 0x0F) ) );
}

void locking_output__hex_buffer (byte_t * buf, byte_t s) {
	for (byte_t pos = 0; pos < s; pos++){
		byte_t b = buf[pos];
		locking_output__byte (get_hex_char( (b & 0xF0)>>4 ));
		locking_output__byte (get_hex_char( (b & 0x0F) ));
		locking_output__byte (' ');
		if ( pos % 8 == 7) locking_output__new_line ();
	}
	
	locking_output__new_line ();
	
	enable_output
}

void locking_output__string (const char * buf, _bool new_line) {
	// _c_buffer__push(_stdout, _IBB);
	
	for (byte_t pos = 0;  buf[pos]; pos++){
		locking_output__byte (buf[pos]);
	}
	
	if (new_line) locking_output__new_line ();
	
	enable_output
}


void P__put__string (const char * buf, _bool new_line) {
	byte_t a = 0;
	while ( (a = pgm_read_byte(buf)) != 0x00 ) {
		put_byte (a); buf++;
	}

	if (new_line) {
		_c_buffer__push(_stdout, '\n');
		_c_buffer__push(_stdout, '\r');
	}
	
	enable_output
}

void P_locking_output__string (const char * buf, _bool new_line) {
	byte_t a = 0;
	while ( (a = pgm_read_byte(buf)) != 0x00 ) {
		locking_output__byte (a); buf++;
	}
	
	if (new_line) locking_output__new_line ();
	
	enable_output
}

