/*
 * _fs_script__info.c
 *
 * Created: 11/1/2021 10:23:03 PM
 *  Author: mkentrru
 */ 

#include "fs_scripts.h"

script_result fs__check__if_file () {
	if ( fs_context.current_obj.TYPE == obj_type__file )
	return script__next;
	
	// show dir info
	fs_obj_descr__print_info (& fs_context.current_obj);
	return 2;
}


script_result fs__info__show_file () {
	
	fs_obj_descr__print_info (& fs_context.current_obj);
	
	locking_output__prompt ();
	return script__exit;
}


byte_t fs__dir_info__obj_count = 0;

script_result fs__sub__setup () {
	
	// fs_context.tmp_obj.KID = fs_context.current_folder.KID;
	
	fs_context.search_level = fs__sub__root_level;
	
	fs_context.search_stack [0] = fs_context.current_folder.KID;
	fs_context.search_stack [fs__sub__root_level] = fs_context.current_obj.KID;
	fs_context.search_stack [fs__sub__root_level + 1] = 0;
	
	fs__dir_info__obj_count = 0;
	
	return script__next;
}

script_result fs__sub__entry () {
	// locking_output__string ("Going deeper: ", _false);
	
	fs_context.current_folder.KID =
	fs_context.search_stack [fs_context.search_level];
	
	fs_context.current_obj.KID = fs_context.current_folder.KID;
	
	//locking_output__hex_byte (fs_context.current_obj.KID);
	//locking_output__new_line ();
	
	return script__next;
}


script_result fs__sub__control () {
	
	//locking_output__hex_byte (fs_context.search_level);
	
	if ( fs_context.search_level <= fs__sub__root_level ) {
		
		// fs_context.current_folder.KID = fs_context.tmp_obj.KID;
		fs_context.current_folder.KID = fs_context.search_stack [0];
		
		P_locking_output__string (PSTR("Total: "), _false);
		locking_output__hex_byte (fs__dir_info__obj_count);
		
		locking_output__new_line ();
		locking_output__prompt ();
		return script__next;
	}
	
	fs_context.search_level--;
	// locking_output__string ("Loop.", _true);
	return fs__sub__loop;
}

void fs__info_dir__printf_obj (fs_obj_descr * o, byte_t l) {
	for ( byte_t a = 0; a < l; a++ ) {
		P_locking_output__string (PSTR(".."), _false);
	}
	
	locking_output__byte (o->nA);
	locking_output__byte (o->nB);
	
	if ( o->TYPE == obj_type__dir ) {
		P_locking_output__string (PSTR(":"), _false);
	}
	
	locking_output__new_line ();
}

script_result fs__info_dir__foreach () {
	
	// Obj from prev dive
	KID * last_sub_obj = fs_context.search_stack + fs_context.search_level + 1;
	fs_obj_descr * s = fs_context.current_obj_buf.as_directory.items;
	
	for (fs_obj_descr * o = fs_context.current_obj_buf.as_directory.items;
	o < s + (K_SIZE / sizeof(fs_obj_descr)); o++
	){
		
		if ( * last_sub_obj != 0 ) {
			
			if ( * last_sub_obj == o->KID ) {
				* last_sub_obj = 0;
			}
		}
		
		else if ( ! ( fs_obj_descr__empty(o) || fs_obj_descr__shared (o) ) ) {
			if ( fs_obj_descr__available (o) ) {
				fs__info_dir__printf_obj (o, fs_context.search_level);
				fs__dir_info__obj_count++;
				
				// if dir -> go deeper
				if ( o->TYPE == obj_type__dir ) {
					
					* last_sub_obj = o->KID;
					* (last_sub_obj + 1) = 0;
					fs_context.search_level++;
					
					return fs__sub__dive;
				}
			}
		}
	}
	// locking_output__new_line ();
	return script__next;
}


#define fs__info__script_size 13
script_result (* fs__info__script_f [fs__info__script_size]) (struct script *)  = {
	fs__ls_entry,			// 0: current_obj.KID = current_folder.KID
	fs__ls_load_kluster,	// 1: load current_obj.KID Kluster
	fs__find__by_name,		// 2: if found jumps to (5)
	fs__ls_check_chain,		// 3: if chain ended jumps to (4)
	fs__find__failure,		// 4: message about failure, exit
	
	fs__check__if_file,		// 5: if file: +1; else: +2;
	fs__info__show_file,	// 6: show file info
	
	fs__sub__setup,			// 7: setup sub search
	
	fs__sub__entry,			// 8: select search level - Dir
	fs__ls_load_kluster,	// 9: load Dir K-n
	fs__info_dir__foreach,		// 10: work around Dir
	fs__ls_check_chain,		// 11: check Dir K-chain
	fs__sub__control		// 12: check if finished
};


void fs__info__script__init () {
	script__init (& fs__info__script, fs__info__script_size, _null, fs__info__script_f );
}

void fs__info__script_task () {
	script__step (& fs__info__script);
}