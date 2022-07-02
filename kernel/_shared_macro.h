/*
 * _shared_macro.h
 *
 * Created: 9/6/2021 6:35:11 PM
 *  Author: mkentrru
 */ 


#ifndef _SHARED_MACRO_H_
#define _SHARED_MACRO_H_


#include <stdint.h>

typedef uint8_t byte_t;
typedef uint8_t breg;

typedef uint16_t word_t;

#define _sf(a)		(1 << a)
#define _df(a)		(~(1 << a))
#define _dm(a, m)	(a & ~m)

#define setflag		|=
#define dropflag	&=

typedef uint8_t _bool;
#define _true	(_bool) 0x01
#define _false	(_bool) 0x00

#define loop while(1);

typedef uint8_t exit_status;

#define exit_success	0
#define exit_failure	1

#define _null 0

#define valid_cmd_symbol(c) ((c >= 'A' && c <= 'Z') ||\
 (c >= 'a' && c <= 'z') ||\
 (c >= '0' && c <= '9') || (c == '.'))

#endif /* _SHARED_MACRO_H_ */