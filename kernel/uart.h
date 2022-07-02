/*
 * uart.h
 *
 * Created: 9/15/2021 1:29:24 AM
 *  Author: mkentrru
 */ 


#ifndef UART_H_
#define UART_H_

#include "_shared_macro.h"
#include "_buffer.h"
#include <avr/io.h>
#include <avr/pgmspace.h>

#define enable_udre_int		UCSRB setflag	_sf(UDRIE);
#define disable_udre_int	UCSRB dropflag	_df(UDRIE);

#define enable_rxc_int		UCSRB setflag	_sf(RXCIE);
#define disable_rxc_int		UCSRB dropflag	_df(RXCIE);

#define enable_input		enable_rxc_int
#define disable_input		disable_rxc_int

#define enable_output		enable_udre_int
#define disable_output		disable_udre_int


#define uart_input_size		0x100
#define uart_output_size	0x100
#define uart_error_size		0x10

extern _c_buffer uart_input;
extern _c_buffer uart_output;
extern _c_buffer uart_error;

#define _stdout		&uart_output
#define _stdin		&uart_input
#define _stderr		&uart_error

#define _stdin_buf	uart_input.buf

#define _stdin_empty (_c_buffer__empty_v(uart_input))

#define _stdin_buf_length(p1, p2) (_c_buffer__length(uart_input.mask, p1, p2))

#define _stdin_index(i) ((i) & uart_input.mask)


// #define _IBB 0x02 // input begin byte	(start of text)
#define _IEB '\r' // input end byte		(end of text)
// #define _OIBB 0x04 // output begin interrupt byte
#define _NLB '\n' // output end interrupt byte

/* commands */
#define _IOFFSET_CMD	0x01
#define _IOFFSET_DB		0x02
#define _CMD_NEW_STRING (byte_t) 'N'
#define _CMD_REM_STRING (byte_t) 'R'
#define _CMD_SET_SIZE	(byte_t) 'S'

/* errors */
#define _ERR_BUF	0x10
#define _ERR_BUF_INPUT_OVERFLOW		(_ERR_BUF + 1)
#define _ERR_BUF_OUTPUT_OVERFLOW	(_ERR_BUF + 2)
#define _ERR_BUF_EMPTY				(_ERR_BUF + 3)

#define _ERR_CMD	0x20
#define _ERR_CMD_NO_IBB				(_ERR_CMD + 1)
#define _ERR_CMD_NO_IEB				(_ERR_CMD + 2)
#define _ERR_CMD_UNDEF				(_ERR_CMD + 3)
#define _ERR_CMD_CB_JUNK			(_ERR_CMD + 4)
#define _ERR_CMD_REQ_DATA			(_ERR_CMD + 5)
#define _ERR_CMD_INVALID_DATA		(_ERR_CMD + 6)

#define _ERR_USER	0x30






void init_default_uart ();
void put_buffer (byte_t * buf, byte_t s);
void put_hex_buffer (byte_t * buf, byte_t s);
void put_string (const char * buf, _bool new_line);
void put_error (byte_t err);
void put_extra_error (byte_t err, byte_t * buf);
byte_t get_byte ();



#define MAX_ARGCOUNT 4
struct _args {
	byte_t argc;
	char * argv[4];
};
extern struct _args args;

void put_byte (byte_t b);
void put_hex_byte (byte_t b);
void new_line ();

#define locking_output(b) { \
	while ( _c_buffer__push(_stdout, b) ) { \
		enable_output \
	} \
}

void locking_output__byte (byte_t b);
void locking_output__new_line ();
void locking_output__hex_byte (byte_t b);
void locking_output__hex_buffer (byte_t * buf, byte_t s);
void locking_output__string (const char * buf, _bool new_line);


/*
	FOR PROGMEM
*/

void P__put__string (const char * buf, _bool new_line);
void P_locking_output__string (const char * buf, _bool new_line);

#endif /* UART_H_ */