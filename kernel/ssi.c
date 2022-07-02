/*
 * ssi.c
 *
 * Created: 9/21/2021 1:48:07 PM
 *  Author: mkentrru
 */ 

#include "ssi.h"

byte_t output_ssi_description [_SSI_SIZE];
byte_t output_ssi_description_backup [_SSI_SIZE];
enum _ssi_modes _ssi_mode = single;
enum _ssi_modes _ssi_mode_backup;

byte_t _ssi__port_hex_code (byte_t v){
	
	switch (v){
		case 0:
		return _ssi_hex_0;
		break;
		
		case 1:
		return _ssi_hex_1;
		break;
		
		case 2:
		return _ssi_hex_2;
		break;
		
		case 3:
		return _ssi_hex_3;
		break;
		
		case 4:
		return _ssi_hex_4;
		break;
		
		case 5:
		return _ssi_hex_5;
		break;
		
		case 6:
		return _ssi_hex_6;
		break;
		
		case 7:
		return _ssi_hex_7;
		break;
		
		case 8:
		return _ssi_hex_8;
		break;
		
		case 9:
		return _ssi_hex_9;
		break;
		//
		case 0x0A:
		return _ssi_hex_A;
		break;
		
		case 0x0B:
		return _ssi_hex_B;
		break;
		
		case 0x0C:
		return _ssi_hex_C;
		break;
		
		case 0x0D:
		return _ssi_hex_D;
		break;
		
		case 0x0E:
		return _ssi_hex_E;
		break;
		
		case 0x0F:
		return _ssi_hex_F;
		break;
		
		default:
		return _ssi_digital_point;
		break;
	}
}

byte_t _ssi__port_simbols_code (byte_t v){
	switch (v){
		
		case 0x00:
		return 0x00;
		break;
		
		case 0x01:
		return _ssi_sym_top;
		break;
		
		case 0x02:
		return _ssi_sym_right;
		break;
		
			case 0x03:
			return _ssi_sym_top | _ssi_sym_right;
			break;
		
		case 0x04:
		return _ssi_sym_bottom;
		break;
		
			case 0x07:
			return _ssi_sym_top | _ssi_sym_right | _ssi_sym_bottom;
			break;
		
		case 0x08:
		return _ssi_sym_left;
		break;
		
			case 0x0F:
			return _ssi_sym_top | _ssi_sym_right | _ssi_sym_bottom | _ssi_sym_left;
			break;
		
		case 0x10:
		return _ssi_digital_point;
		break;
		
		default:
		return _ssi_digital_point;
		break;
	}
	
}


void _ssi__update_description 
(enum _ssi_description_types t, byte_t * data, byte_t size, enum _ssi_modes m){
	switch (t){
		case hex:
			for (byte_t a = 0; a < size; a++){
				if (a % 2)
					output_ssi_description [a] = _ssi__port_hex_code(data[a >> 1] >> 4);
				else
					output_ssi_description [a] = _ssi__port_hex_code(data[a >> 1] & 0x0F);
			}
		break;
		
		default:
			for (byte_t a = 0; a < size; a++){
				output_ssi_description [a] = _ssi__port_simbols_code(data[a]);
			}
		break; 
	};
	_ssi_mode = m;
}

void _ssi__set_word (byte_t a, byte_t b) {
	
	output_ssi_description [0] = _ssi__port_hex_code (b & 0x0F);
	output_ssi_description [1] = _ssi__port_hex_code (b >> 4);
	
	output_ssi_description [2] = _ssi__port_hex_code (a & 0x0F);
	output_ssi_description [3] = _ssi__port_hex_code (a >> 4);
	
	_ssi_mode = complex;
}


void _ssi_update (){
	static byte_t current_ssi_node = 0;
	ssi_control &= ~ssi_control_mask;
	ssi_control |= (1<<current_ssi_node);
	
	if ( _ssi_mode == complex ) {
		ssi_output = output_ssi_description [current_ssi_node];
	}
	else {
		ssi_output = * output_ssi_description;
	}
	
	current_ssi_node = (current_ssi_node + 1) % 4;
}


void init_default_ssi (){
	DDRA setflag	ssi_control_mask;
	DDRC setflag	ssi_output_mask;
}

void _ssi__default_configuration () {
	_ssi_mode = complex;
}

void _ssi__store_description (){
	_ssi_mode_backup = _ssi_mode;
	for (byte_t a = 0; a < _SSI_SIZE; a++){
		output_ssi_description_backup[a] = output_ssi_description[a];
		output_ssi_description[a] = 0;
	}
}

void _ssi__restore_description (){
	_ssi_mode = _ssi_mode_backup;
	for (byte_t a = 0; a < _SSI_SIZE; a++){
		output_ssi_description[a] = output_ssi_description_backup[a];
	}
}
