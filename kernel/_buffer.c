/*
 * _buffer.c
 *
 * Created: 9/6/2021 11:34:16 PM
 *  Author: mkentrru
 */ 

#include "_buffer.h"

void _c_buffer__init (_c_buffer * c, byte_t * buf, byte_t mask){ // mask = size - 1
	c->buf = buf;
	c->mask = mask;
	
	c->in = 0;
	c->out = 0;
	c->state.full = _false;
}

exit_status _c_buffer__push (_c_buffer * c, byte_t d){
	if (c->state.full) return exit_failure;
	
	c->buf[c->in] = d;
	c->in = inc_id(c->in, c->mask);
	
	if (c->in == c->out) c->state.full = _true;
	
	return exit_success;
}


exit_status _c_buffer__pop (_c_buffer * c, byte_t * d){
	if ( _c_buffer__empty(c) )	// empty
		return exit_failure;
	
	if (d) * d = c->buf[c->out];
	c->out = inc_id(c->out, c->mask);
	
	c->state.full = _false;
	
	return exit_success;
}

exit_status _c_buffer__undo_input (_c_buffer * c) {
	if (c->in != c->out) {
		c->in = dec_id(c->in, c->mask);
		return exit_success;
	}
	return exit_failure;
}

exit_status _c_buffer__seek (_c_buffer * c, byte_t s, _c_buffer_pos * fpos){
	if ( _c_buffer__empty(c) )
		return exit_failure;
	
	_c_buffer_pos pos = c->out;
	
	do{
		if (c->buf[pos] == s){
			* fpos = pos;
			return exit_success;
		}
		pos = inc_id(pos, c->mask);
	} while (pos != c->in);
	
	return exit_failure;
}

exit_status _c_buffer__parse_string
(_c_buffer * c, byte_t d, _c_buffer_pos * fpos, char * s) {
	
	if ( _c_buffer__empty(c) )
	return exit_failure;
	
	_c_buffer_pos pos = c->out;
	char a = 0;
	do{
		a = c->buf[pos];
		if (a == d || a == '\n' || a == '\0'){
			* fpos = pos;
			* s = '\0';
			return exit_success;
		}
		*(s++) = a;
		pos = inc_id(pos, c->mask);
	} while (pos != c->in);
	
	return exit_failure;
	
}


exit_status _c_buffer__pop_break (_c_buffer * c, byte_t * d, byte_t b){
	if (c->buf[c->out] == b) {
		return exit_failure;
	}
	
	* d = c->buf[c->out];
	
	c->out = inc_id (c->out, c->mask);
	c->state.full = _false;
	
	return exit_success;
}

byte_t _c_buffer__get (_c_buffer * c, _c_buffer_pos pos){
	return c->buf[pos & c->mask];
}

void _c_buffer__fix (_c_buffer * c){
	c->out = c->in;
	c->state.full = _false;
}

void _c_buffer__jump (_c_buffer * c, _c_buffer_pos pos){
	c->out = (pos & c->mask);
}

void _c_buffer__next (_c_buffer * c){
	c->out = inc_id(c->out, c->mask);
}


exit_status _c_buffer_check (
		_c_buffer * c, 
		_c_buffer_pos pos, _c_buffer_pos p_end, 
		exit_status (*f) (byte_t))
{
	while (pos != p_end){
		
		if (f(c->buf[pos]))
			return exit_failure;
		
		pos = inc_id(pos, c->mask);
	}
	return exit_success;
}

byte_t _c_buffer_slice (_c_buffer * c){
	byte_t d = c->buf[c->out];
	
	c->out = inc_id (c->out, c->mask);
	c->state.full = _false;
	
	return d;
}

void _c_buffer__skip (_c_buffer * c, byte_t pos){
	c->out = (c->out + pos) & c->mask;
}
