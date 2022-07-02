/*
 * user_tasks.c
 *
 * Created: 9/21/2021 2:12:33 PM
 *  Author: mkentrru
 */ 

#include "lab.h"



ibind_counter timer__ssi_cd_exec () {	
	_ssi_update ();
	return timer__ssi_cd_counter;
}

byte_t pin_b = 0x00;
#define passwd_input_pin PINB
#define passwd_input_pin__value pin_b

ibind_counter timer__ioport_check_exec () {
	passwd_input_pin__value &= passwd_input_pin;
	DDRB = ~passwd_input_pin__value;
	PORTB = passwd_input_pin__value;
	
	return timer__ioport_check_counter;
}


ibind_counter timer__animation__exec () {
	allow_task (inter_script_task);
	return timer__animation_counter;
}


#define passwd_input__animation_size _SSI_SIZE
byte_t passwd_input__animation_step = 0;

passwd_t entered_passwd = 0xFF;

byte_t ssi_animation_buffer [_SSI_SIZE];
#define ssi_animation_symbol 0x10


script_result passwd_input__entry () {
	
	passwd_input__animation_step = 0;
	passwd_input_pin__value = 0xFF;
	
	_ssi__store_description ();
	for ( byte_t a = 0; a < _SSI_SIZE; a++ ) {
		ssi_animation_buffer [a] = 0;
	}
	
	passwd_input_pin__value = 0xFF;
	DDRB = 0x00;
	PORTB = 0xFF;
	
	disable_int0
	
	put_string ("Enter passwd at PORTB.", _true);
	enable_bind (timer__ioport_check_bind, timer__ioport_check_counter);
	enable_bind (timer__animation_bind, timer__animation_counter);
	
	return script__next;
}

script_result passwd_input__animation_loop () {
	put_byte ('.');
	if ( passwd_input__animation_step < _SSI_SIZE) {
		ssi_animation_buffer [passwd_input__animation_step] = 
		ssi_animation_symbol;
		
		_ssi__update_description (symbols, ssi_animation_buffer, _SSI_SIZE, complex);
	}
	
	if (passwd_input__animation_step++ < passwd_input__animation_size) {
		disable_current_task ();
		return script__stay;
	}
	
	disable_bind (timer__ioport_check_bind);
	disable_bind (timer__animation_bind);
	
	return script__next;
}

script_result passwd_input__check () {
	entered_passwd = passwd_input_pin__value;
	new_line ();
	put_string ("You entered: ", _false);
	put_hex_byte (entered_passwd);
	new_line ();
	
	// login root
	if ( fs_context.access_control.current_try) {
		
		if ( ! fs_context.access_control.defined_root ) {
			// create RP
			
		}
		
		if ( fs_context.access_control.rp == entered_passwd ) {
			put_string ("Correct passwd.", _true);
			fs_context.access_control.root = _true;
			return script__next;
		}
	}
	// login user
	else 
	if ( ! fs_context.access_control.current_try ) {
		
		if ( ! fs_context.access_control.defined_user ) {
			// create UP
			
		}
		
		if ( fs_context.access_control.up == entered_passwd ) {
			put_string ("Correct passwd.", _true);
			fs_context.access_control.user = _true;
			return script__next;
		}
	}
	
	put_string ("Wrong passwd.", _true);
	
	return script__next;
}

script_result passwd_input__outro () {
	DDRB = 0xFF;
	PORTB = 0x00;
	
	_ssi__restore_description ();
	put_prompt ();
	enable_int0
	
	return script__next;
}

#define passwd_input__script_size 4
script_result (* passwd_input_f [passwd_input__script_size]) (struct script *)  = {
	passwd_input__entry,
	passwd_input__animation_loop,
	passwd_input__check,
	passwd_input__outro
};


void passwd_input__script__init () {
	script__init (& passwd_input__script, passwd_input__script_size, _null, passwd_input_f );
}

void passwd_input__script_task () {
	script__step (& passwd_input__script);
}

script_result passwd_change__setup () {
	entered_passwd = passwd_input_pin__value;
	new_line ();
	put_string ("You entered: ", _false);
	put_hex_byte (entered_passwd);
	new_line ();
	
	if ( fs_context.access_control.current_try ) {
		 fs_context.current_obj.iROOT = 1;
		 fs_context.current_obj.nA = 'R';
	}
	else 
	if ( ! fs_context.access_control.current_try ) {
		fs_context.current_obj.iUSER = 1;
		fs_context.current_obj.nA = 'U';
	}
	
	fs_context.current_obj.nB = 'P';
	fs_context.current_obj.TYPE = obj_type__file;
	
	
	fs_context.current_obj.KID = ROOT_KID;
	return script__next;
}


script_result passwd_change__write_changes () {
	
	fs_K__change 
	(& fs_context.current_obj_buf, & fs_context.current_obj__memsync, 0,
	& entered_passwd, sizeof(passwd_t));
	
	return script__next;
}

script_result passwd_change__change_current () {
	if ( fs_context.access_control.current_try ) {
		fs_context.access_control.rp = entered_passwd;
	}
	else
	if ( ! fs_context.access_control.current_try ) {
		fs_context.access_control.up = entered_passwd;
	}
	
	P_locking_output__string (PSTR("Passwd successfully changed."), _true);
	
	return script__next;
}

script_result passwd_change__entry () {
	
	passwd_input__animation_step = 0;
	passwd_input_pin__value = 0xFF;
	
	_ssi__store_description ();
	for ( byte_t a = 0; a < _SSI_SIZE; a++ ) {
		ssi_animation_buffer [a] = 0;
	}
	
	passwd_input_pin__value = 0xFF;
	DDRB = 0x00;
	PORTB = 0xFF;
	
	disable_int0
	
	put_string ("Enter passwd at PORTB.", _true);
	enable_bind (timer__ioport_check_bind, timer__ioport_check_counter);
	enable_bind (timer__animation_bind, timer__animation_counter);
	
	return script__next;
}



#define passwd_change__script_size 13
script_result (* passwd_change_f [passwd_change__script_size]) (struct script *)  = {
	passwd_change__entry,			// 0: init input
	passwd_input__animation_loop,	// 1: input animation
	
	/*
		1. Find FP_descr at ROOT_KID
		2. Load FP_descr_KID
		3. Change passwd at FP K
		4. Sync K
	*/
	passwd_change__setup,			// 2: store passwd and configure searching
		
	fs__ls_load_kluster,			// 3: load ROOT_KID Klaster
	fs__find__by_name,				// 4: find FP at Klaster
	fs__ls_check_chain,				// 5: keep searching at next Kl if not found
	fs__find__failure,				// 6: if not found
	
	passwd_change__write_changes,	// 7: write new passwd
	fs__sync__current_K,			// 8: sync FP Kl
	passwd_change__change_current,	// 9: set new value at RAM
	fs__count_free,
	fs__sync__intro,
	passwd_input__outro				// 10: outro
};


void passwd_change__script__init () {
	script__init (& passwd_change__script, passwd_change__script_size, _null, passwd_change_f );
}

void passwd_change__script_task () {
	script__step (& passwd_change__script);
}

_bool init_failed = _false;
KID source_folder_keeper = 0;
script_result passwd_init__entry (script * s) {
	
	s->data = (void *) & entered_passwd;
	
	// login root
	if ( fs_context.access_control.current_try ) {
		fs_context.current_obj.iROOT = 1;
		fs_context.current_obj.iUSER = 0;
		fs_context.current_obj.nA = 'R';
	}
	else {
		fs_context.current_obj.iROOT = 0;
		fs_context.current_obj.iUSER = 1;
		fs_context.current_obj.nA = 'U';
	}
	
	fs_context.current_obj.nB = 'P';
	fs_context.current_obj.SIZE = 1;
	fs_context.current_obj.TYPE = obj_type__file;
	
	// to keep current dir
	
	//locking_output__string("Current dir: ", _false);
	//locking_output__hex_byte (fs_context.current_folder.KID);
	//locking_output__new_line ();
	
	source_folder_keeper = fs_context.current_folder.KID;
	fs_context.current_folder.KID = ROOT_KID;
	
	fs_context.descr_pointer = _null;
	return script__next;
}

script_result passwd_init__obj_exists () {
	P_locking_output__string(PSTR("Subject passwd file exists! But how?.."), _true);
	locking_output__prompt ();
	return script__exit;
}

script_result passwd_init__input_outro () {
	entered_passwd = passwd_input_pin__value;
	locking_output__new_line ();
	P_locking_output__string (PSTR("You entered: "), _false);
	locking_output__hex_byte (entered_passwd);
	locking_output__new_line ();
	
	DDRB = 0xFF;
	PORTB = 0x00;
	_ssi__restore_description ();
	
	enable_int0
	
	return script__next;
}

script_result passwd_init__outro () {
	if ( ! init_failed ) {
		if ( fs_context.access_control.current_try ) {
			fs_context.access_control.rp = entered_passwd;
			fs_context.access_control.root = _true;
			fs_context.access_control.defined_root = _true;
		}
		else {
			fs_context.access_control.up = entered_passwd;
			fs_context.access_control.user = _true;
			fs_context.access_control.defined_user = _true;
		}
	}
	
	fs_context.current_folder.KID = source_folder_keeper;
	
	//locking_output__string("Current dir: ", _false);
	//locking_output__hex_byte (fs_context.current_folder.KID);
	//locking_output__new_line ();
	
	locking_output__prompt ();
	return script__exit;
}


script_result fs__passwd_init__build_new_descr () {
	P_locking_output__string(PSTR("Building new descriptor."), _true);
	
	init_failed = _true;
	
	KID nid = KP_NAN;
	if ( * fs_context.search_stack ) fs_context.current_folder.KID = * fs_context.search_stack;
	
	// means contains no free descr
	if ( fs_context.descr_pointer == _null ) {
		P_locking_output__string (PSTR("No free descriptors found."), _true);
		
		// 0: allocate new kluster for Dir
		//fs_context.current_folder.KID =
		//fs__K__allocate (fs_context.current_folder.KID);
		nid = fs__K__allocate (fs_context.current_folder.KID);
		if ( nid == KP_NAN ) {
			init_failed = _true;
			return 14;
		}
		
		fs_context.current_folder.KID = nid;
		// 1: take first descriptor
		fs_context.descr_pointer = (fs_obj_descr *) fs_context.current_obj_buf.as_directory.items;
	}
	
	// fs_obj_descr__print (fs_context.descr_pointer);
	
	tmp_descr.iROOT = fs_context.current_obj.iROOT;
	tmp_descr.iUSER = fs_context.current_obj.iUSER;
	nid = fs__K__allocate (KP_NAN);
	if ( nid == KP_NAN ) {
		init_failed = _true;
		return 14;
	}
	
	tmp_descr.KID = nid;
	
	tmp_descr.nA = fs_context.current_obj.nA;
	tmp_descr.nB = fs_context.current_obj.nB;
	tmp_descr.SIZE = fs_context.current_obj.SIZE;
	tmp_descr.TYPE = fs_context.current_obj.TYPE;
	// fs_obj_descr__print (& tmp_descr);
	
	fs_context.current_obj.KID = fs_context.current_folder.KID;
	
	return 2;
}

#define passwd_init__script_size 22
script_result (* passwd_init_f [passwd_init__script_size]) (struct script *)  = {
	passwd_init__entry,
	
	fs__ls_entry,					// 1: check access
	fs__ls_load_kluster,			// 2: load K
	fs__create__check_same,			// 3: if obj is same: +3; else: +1;
	fs__ls_check_chain,				// 4: if chain ended: +1; else: -2;
	
	fs__passwd_init__build_new_descr,	// 5: get free obj_descr and allocate K for Obj (+2 to skip error)
	passwd_init__obj_exists,		// 6: if passwd file exists
	
	// enter new passwd
	passwd_input__entry,			// 7: init input animation
	passwd_input__animation_loop,	// 8: animation loop step
	passwd_init__input_outro,		// 9: animation end
	
	// write new data to new file
	fs__ls_load_kluster,			// 10: load K witch contains free obj descriptor (dir)
	fs__create__write_new_descr,	// 11: write new descr to current dir
	fs__sync__intro,				// 12: sync intro
	fs__sync__current_K,			// 13: sync Dir
	
	fs__touch__write_data__setup,	// 14: get ready to write data
	fs__ls_load_kluster,			// 15: load current K
	fs__touch__write_data,			// 16: write data to current K
	fs__sync__current_K,			// 17: sync changes with memory
	fs__touch__write__check,		// 18: check if any data left
	fs__count_free,
	fs__sync__intro,				// 19: sync intro
	
	passwd_init__outro				// 20: store new passwd at context and set logged in
};


void passwd_init__script__init () {
	script__init (& passwd_init__script, passwd_init__script_size, _null, passwd_init_f );
}

void passwd_init__script_task () {
	script__step (& passwd_init__script);
}


void external_interrupts__cooldown () {
	disable_int0
	enable_bind (timer__ext_int_cd_bind, timer__ext_int_cd_counter);
}