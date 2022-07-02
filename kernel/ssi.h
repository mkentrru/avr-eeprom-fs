/*
 * ssi.h
 *
 * Created: 9/21/2021 1:47:40 PM
 *  Author: mkentrru
 */ 


#ifndef SSI_H_
#define SSI_H_

#include "_shared_macro.h"
#include <avr/io.h>

#define ssi_output			PORTC
#define ssi_output_mask		0xFF
#define ssi_control			PORTA
#define ssi_control_mask	0x0F

#define _SSI_SIZE 4
#define _SSI_SIZE_ASBYTE 2

#define _ssi_A	0
#define _ssi_B	1
#define _ssi_C	2
#define _ssi_D	3
#define _ssi_E	4
#define _ssi_F	5
#define _ssi_G	6
/*
		/	A	\
		F		B
		>	G	<
		E		C
		\	D	/
*/

#define _ssi_DP	7
#define _ssi_hex_0	(_sf(_ssi_A)|_sf(_ssi_B)|_sf(_ssi_C)|_sf(_ssi_D)|_sf(_ssi_E)|_sf(_ssi_F))
#define _ssi_hex_1	(_sf(_ssi_B)|_sf(_ssi_C))
#define _ssi_hex_2	(_sf(_ssi_A)|_sf(_ssi_B)|_sf(_ssi_D)|_sf(_ssi_E)|_sf(_ssi_G))
#define _ssi_hex_3	(_sf(_ssi_A)|_sf(_ssi_B)|_sf(_ssi_C)|_sf(_ssi_D)|_sf(_ssi_G))
#define _ssi_hex_4	(_sf(_ssi_B)|_sf(_ssi_C)|_sf(_ssi_F)|_sf(_ssi_G))
#define _ssi_hex_5	(_sf(_ssi_A)|_sf(_ssi_C)|_sf(_ssi_D)|_sf(_ssi_F)|_sf(_ssi_G))
#define _ssi_hex_6	(_sf(_ssi_A)|_sf(_ssi_C)|_sf(_ssi_D)|_sf(_ssi_E)|_sf(_ssi_F)|_sf(_ssi_G))
#define _ssi_hex_7	(_sf(_ssi_A)|_sf(_ssi_B)|_sf(_ssi_C))
#define _ssi_hex_8	(_sf(_ssi_A)|_sf(_ssi_B)|_sf(_ssi_C)|_sf(_ssi_D)|_sf(_ssi_E)|_sf(_ssi_F)|_sf(_ssi_G))
#define _ssi_hex_9	(_sf(_ssi_A)|_sf(_ssi_B)|_sf(_ssi_C)|_sf(_ssi_D)|_sf(_ssi_F)|_sf(_ssi_G))

#define _ssi_hex_A	(_sf(_ssi_A)|_sf(_ssi_B)|_sf(_ssi_C)|_sf(_ssi_E)|_sf(_ssi_F)|_sf(_ssi_G)|_sf(_ssi_DP))
#define _ssi_hex_B	(_sf(_ssi_A)|_sf(_ssi_B)|_sf(_ssi_C)|_sf(_ssi_D)|_sf(_ssi_E)|_sf(_ssi_F)|_sf(_ssi_G)|_sf(_ssi_DP))
#define _ssi_hex_C	(_sf(_ssi_A)|_sf(_ssi_E)|_sf(_ssi_F)|_sf(_ssi_DP))
#define _ssi_hex_D	(_sf(_ssi_A)|_sf(_ssi_B)|_sf(_ssi_C)|_sf(_ssi_D)|_sf(_ssi_E)|_sf(_ssi_F)|_sf(_ssi_DP))
#define _ssi_hex_E	(_sf(_ssi_A)|_sf(_ssi_D)|_sf(_ssi_E)|_sf(_ssi_F)|_sf(_ssi_G)|_sf(_ssi_DP))
#define _ssi_hex_F	(_sf(_ssi_A)|_sf(_ssi_E)|_sf(_ssi_F)|_sf(_ssi_G)|_sf(_ssi_DP))


#define _ssi_sym_top	(_sf(_ssi_A))
#define _ssi_sym_right	(_sf(_ssi_B)|_sf(_ssi_C))
#define _ssi_sym_bottom	(_sf(_ssi_D))
#define _ssi_sym_left	(_sf(_ssi_E)|_sf(_ssi_F))
#define _ssi_digital_point	(_sf(_ssi_DP))


//

enum _ssi_modes {
	single,
	complex
};

enum _ssi_description_types{
	hex,
	symbols	
};

extern byte_t output_ssi_description[4];
extern enum _ssi_modes _ssi_mode;

void _ssi__default_configuration ();

void _ssi__set_word (byte_t a, byte_t b);
void _ssi__update_description 
(enum _ssi_description_types t, byte_t * data, byte_t size, enum _ssi_modes m);
void _ssi_update ();


void _ssi__store_description ();
void _ssi__restore_description ();

void init_default_ssi ();

#endif /* SSI_H_ */