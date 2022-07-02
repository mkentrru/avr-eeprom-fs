/*
 * _fs_script__cat.c
 *
 * Created: 11/1/2021 10:01:32 PM
 *  Author: mkentrru
 */ 

#include "fs_scripts.h"

script_result fs__find__by_name () {
	fs_obj_descr * o =
	fs_obj_descr__get_by_name
	(fs_context.current_obj.nA, fs_context.current_obj.nB, fs_context.current_obj.TYPE);
	
	if ( o != _null ) {
		
		if (! fs_obj_descr__available (o) ) {
			put_prompt ();
			return script__error;
		}
		
		fs_context.current_obj.iROOT = o->iROOT;
		fs_context.current_obj.iUSER = o->iUSER;
		
		// keep id where obj was found
		tmp_descr.KID = fs_context.current_obj.KID;
		
		fs_context.current_obj.KID = o->KID;
		
		fs_context.current_obj.SIZE = o->SIZE;
		
		fs_context.descr_pointer = o;
		
		return 3; // to skip chain check and failure msg
	}
	
	return 1; // keep going
}

script_result fs__find__by_KID () {
	fs_obj_descr * o =
	fs_obj_descr__get_by_KID
	(fs_context.current_obj.KID, fs_context.current_obj.TYPE);
	
	if ( o != _null ) {
		
		if (! fs_obj_descr__available (o) ) {
			put_prompt ();
			return script__error;
		}
		
		fs_context.current_obj.iROOT = o->iROOT;
		fs_context.current_obj.iUSER = o->iUSER;
		
		fs_context.current_obj.KID = o->KID;
		
		fs_context.current_obj.SIZE = 0;
		return 3; // to skip chain check and failure msg
	}
	return 1; // keep going
}


script_result fs__find__failure () {
	locking_output__new_line ();
	P_locking_output__string (PSTR("There is no such Object at this Dir."), _true);
	locking_output__prompt ();
	return script__error;
}

script_result fs__cat__display () {
	for (byte_t i = 0; i < K_SIZE; i++) {
		locking_output__byte (fs_context.current_obj_buf.row [i]);
	}
	// locking_output__new_line ();
	return script__next;
}

script_result fs__cat__outro () {
	locking_output__new_line ();
	locking_output__prompt ();
	return script__next;
}


#define fs__cat__script_size 9
script_result (* fs__cat__script_f [fs__cat__script_size]) (struct script *)  = {
	fs__ls_entry,			// 0: current_obj.KID = current_folder.KID
	fs__ls_load_kluster,	// 1: load current_obj.KID Kluster
	fs__find__by_name,		// 2: if found jumps to (5)
	fs__ls_check_chain,		// 3: if chain ended jumps to (4)
	fs__find__failure,		// 4: message about failure, exit
	// found, displaying: current_obj_kid = file fkid
	fs__ls_load_kluster,	// 5: load file cluster
	fs__cat__display,		// 6: display loop
	fs__ls_check_chain,		// 7: if file chain ended jumps to (8)
	fs__cat__outro			// 8: end msg
};


void fs__cat__script__init () {
	script__init (& fs__cat__script, fs__cat__script_size, _null, fs__cat__script_f );
}

void fs__cat__script_task () {
	script__step (& fs__cat__script);
}
