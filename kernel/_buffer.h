/*
 * _buffer.h
 *
 * Created: 9/6/2021 11:34:07 PM
 *  Author: mkentrru
 */ 


#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "_shared_macro.h"

typedef byte_t _c_buffer_pos;


typedef struct{
	
	byte_t * buf;
	byte_t mask;
	
	_c_buffer_pos in;
	_c_buffer_pos out;
	
	struct{
		_bool full: 1;
	}state;
	
} _c_buffer;


void _c_buffer__init (_c_buffer * c, byte_t * buf, byte_t mask);

void _c_buffer__fix (_c_buffer * c);

exit_status _c_buffer__push		(_c_buffer * c, byte_t d);

exit_status _c_buffer__pop		(_c_buffer * c, byte_t * d);

byte_t _c_buffer_slice (_c_buffer * c);

exit_status _c_buffer__seek		(_c_buffer * c, byte_t s, _c_buffer_pos * fpos);
exit_status _c_buffer__parse_string (_c_buffer * c, byte_t d, _c_buffer_pos * fpos, char * s);


exit_status _c_buffer__pop_break (_c_buffer * c, byte_t * d, byte_t b);

void _c_buffer__jump (_c_buffer * c, _c_buffer_pos pos);

void _c_buffer__next (_c_buffer * c);
exit_status _c_buffer__undo_input (_c_buffer * c);
void _c_buffer__skip (_c_buffer * c, byte_t pos);

exit_status _c_buffer_check (
	_c_buffer * c, _c_buffer_pos pos, _c_buffer_pos p_end,
	exit_status (*f) (byte_t));

#define inc_id(a, m) ((a + 1) & m)
#define dec_id(a, m) ((a - 1) & m)

#define _c_buffer__empty(b) (b->in == b->out && !(b->state.full))
#define _c_buffer__empty_v(b) (b.in == b.out && !(b.state.full))

#define _c_buffer__length(m, p1, p2) ((p2 - p1) & m)

#endif /* _BUFFER_H_ */