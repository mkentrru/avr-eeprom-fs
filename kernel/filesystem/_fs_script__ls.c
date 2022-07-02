/*
 * fs_script__ls.c
 *
 * Created: 11/1/2021 9:34:49 PM
 *  Author: mkentrru
 */ 

#include "fs_scripts.h"


/*
	// ls - list directory items 
void ls ():
	KID id = current_folder_id;
	cid = get_first_kluster_id (id);
	
	do:
		load_kluster (id)
		foreach obj_descr o in current_obj_buf:
			print o
	while (get_next_kluster_id (id) != K_END)
*/


script_result fs__ls_entry () {
	
	if ( ! fs_obj_descr__available (& fs_context.current_folder) ) {
		put_prompt ();
		return script__error;
	}
	
	fs_context.current_obj.KID = 
		fs_kpointer__get_first_kid(fs_context.current_folder.KID);
	
	return script__next;
}

script_result fs__ls_info () {
	P_locking_output__string (PSTR("Type  Owner  Name  FirstKID  Size  Removed"), _true);
	return script__next;
}

script_result fs__ls_load_kluster () {
	
	load_kluster (fs_context.current_obj.KID, 
	(byte_t *) & fs_context.current_obj_buf.row, current_task_id);
	
	return script__next;
}

script_result fs__ls_display_obj () {
	// put_string ("display", _true);
	fs_obj_descr * s = fs_context.current_obj_buf.as_directory.items;
	for (fs_obj_descr * o = fs_context.current_obj_buf.as_directory.items;
	o < s + (K_SIZE / sizeof(fs_obj_descr)); o++
	){
		//if ( ! fs_obj_descr__empty(o) )
			fs_obj_descr__print (o);
	}
	return script__next;
}
#define fs__ls_chain_loop -2
script_result fs__ls_check_chain () {
	// put_string ("loop", _true);
	KID nid = fs_kpointer__get_next_kid (fs_context.current_obj.KID);
	if ( nid != KP_END ) {
		fs_context.current_obj.KID = nid;
		return fs__ls_chain_loop;
	}
	return script__next;
}

script_result fs__outro () {
	
	locking_output__prompt ();
	return script__next;
}

#define fs__ls__script_size 6
script_result (* fs__ls__script_f [fs__ls__script_size]) (struct script *)  = {
	fs__ls_entry,			// allowed by input_handler_exec (uart)
	fs__ls_info, 
	fs__ls_load_kluster,	// disabled till memory read
	fs__ls_display_obj,		// allowed by mem script
	fs__ls_check_chain,
	fs__outro
};

void fs__ls__script__init () {
	script__init (& fs__ls__script, fs__ls__script_size, _null, fs__ls__script_f );
}

void fs__ls__script_task () {
	
	script__step (& fs__ls__script);
}
