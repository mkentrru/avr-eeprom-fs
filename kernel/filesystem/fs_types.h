/*
 * fs_types.h
 *
 * Created: 11/1/2021 9:25:54 PM
 *  Author: mkentrru
 */ 


#ifndef FS_TYPES_H_
#define FS_TYPES_H_

#include "memory.h"
#include "../kernel.h"

typedef struct{

	// byte_t name [UNIT_NAME_SIZE]; // 2 bytes
	
	byte_t nA; byte_t nB; // name
	
	KID KID:	5; // 1 byte
	byte_t TYPE:	1;
	byte_t iROOT:	1;
	byte_t iUSER:	1;
	
	byte_t SIZE;
	
} fs_obj_descr;



union intro_flags {
	struct {
		byte_t fs_type: 3;
		byte_t fs_free_amount: 5;
	} as_flags;
	
	byte_t as_byte;
};


typedef union{
	byte_t row [K_SIZE];
		
	struct{
		union intro_flags flags;
		
		KID kp [K_SIZE - 1];
			
	} as_intro;
		
	struct{
		fs_obj_descr items [K_SIZE / sizeof(fs_obj_descr)];
	} as_directory;
		
} K_t; // k(c)lascter


typedef struct {
	K_t intro;								// 00:20					
	K_t current_obj_buf;					// 20:20
	
	memsync_control intro__memsync;			// 40:04
	memsync_control current_obj__memsync;	// 44:04
	
	fs_obj_descr current_folder;			// 48:04
	fs_obj_descr current_obj;				// 4B:04
	
	
	KID search_stack [K_COUNT];				// 50:20
	byte_t search_level;					// 70:01
	fs_obj_descr * descr_pointer;			// 71:02
	
	struct {
		
		passwd_t up;
		passwd_t rp;
		
		_bool current_try: 1;
		
		_bool root: 1;
		_bool user: 1;
		_bool defined_root: 1;
		_bool defined_user: 1;
		
	} access_control;						//	73:3
	
	_bool ssi_showing_name;
} _fs_context;								//	76






#endif /* FS_TYPES_H_ */