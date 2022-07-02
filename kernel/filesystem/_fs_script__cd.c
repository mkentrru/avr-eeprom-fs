/*
 * _fs_script__cd.c
 *
 * Created: 11/1/2021 10:05:53 PM
 *  Author: mkentrru
 */ 

#include "fs_scripts.h"


script_result fs__cd_entry () {
	//locking_output__hex_byte (fs_context.current_folder.iROOT);
	//locking_output__hex_byte (fs_context.current_folder.iROOT);
	//locking_output__new_line ();
	
	if ( ! fs_obj_descr__shared (& fs_context.current_obj) &&
	! fs_obj_descr__available (& fs_context.current_folder) ) {
		put_prompt ();
		return script__error;
	}
	
	fs_context.current_obj.KID =
	fs_kpointer__get_first_kid (fs_context.current_folder.KID);
	
	return script__next;
}

script_result fs__cd__found () {
	fs_context.current_folder.KID = fs_context.current_obj.KID;
	fs_context.current_folder.iROOT = fs_context.current_obj.iROOT;
	fs_context.current_folder.iUSER = fs_context.current_obj.iUSER;
	
	fs_context.current_folder.nA = fs_context.current_obj.nA;
	fs_context.current_folder.nB = fs_context.current_obj.nB;
	
	fs_context.current_folder.SIZE = 0;
	fs_context.current_folder.TYPE = obj_type__dir;
	
	if ( fs_context.current_folder.nA == '.' &&
	fs_context.current_folder.nB == '.' ) {
		//// look for '..' at current folder
		fs_context.current_folder.nA = ':';
		fs_context.current_folder.nB = '/';
		
		return script__next;
	}
	
	locking_output__new_line ();
	locking_output__prompt ();
	return script__exit;
}

script_result fs__cd__preparent_found () {
	
	fs_context.current_obj.KID =
	fs_kpointer__get_first_kid (fs_context.current_obj.KID);
	
	return script__next;
}


script_result fs__cd__parent_outro () {
	
	locking_output__prompt ();
	return script__exit;
}

script_result fs__cd_find_at_preparent () {
	
	fs_obj_descr * o =
	fs_obj_descr__get_by_KID
	(fs_context.current_folder.KID, obj_type__dir);
	
	if ( o != _null ) {
		
		//if (! fs_obj_descr__available (o) ) {
		//put_prompt ();
		//return script__error;
		//}
		
		fs_context.current_folder.nA = o->nA;
		fs_context.current_folder.nB = o->nB;
		fs_context.current_folder.iROOT = o->iROOT;
		fs_context.current_folder.iUSER = o->iUSER;
		
		
		P_locking_output__string (PSTR(" -> "), _false);
		locking_output__byte (fs_context.current_folder.nA);
		locking_output__byte (fs_context.current_folder.nB);
		
		locking_output__new_line ();
		locking_output__prompt ();
		return script__exit; // to skip chain check and failure msg
	}
	return 1; // keep going
}

script_result fs__cd__no_pre () {
	P_locking_output__string (PSTR(" -> "), _false);
	locking_output__byte (fs_context.current_folder.nA);
	locking_output__byte (fs_context.current_folder.nB);
	
	locking_output__new_line ();
	locking_output__prompt ();
	return script__exit;
}

#define fs__cd__script_size 16
script_result (* fs__cd__script_f [fs__cd__script_size]) (struct script *)  = {
	fs__cd_entry,			// 0: current_obj.KID = current_folder.KID
	fs__ls_load_kluster,	// 1: load current_obj.KID Kluster
	fs__find__by_name,		// 2: if found jumps to (5)
	fs__ls_check_chain,		// 3: if chain ended jumps to (4)
	fs__find__failure,			// 4: message about failure, exit
	// current_folder = current_obj (parent)
	fs__cd__found,			// 5: save found object KID
	
	fs__cd_entry,			// 6: if '..' look for name at parent
	fs__ls_load_kluster,	// 7: load parent Kluster
	fs__find__by_name,		// 8: find '..' at parent Kluster
	fs__ls_check_chain,		// 9: loop for Kluster chains
	fs__cd__no_pre,			// 10: cannot find '..' at parent dir
	// current_obj = '..' -> preparent (current_obj->KID = preparent->KID)
	fs__cd__preparent_found,	// 11: preparent KID found! -> current_obj
	
	fs__ls_load_kluster,		// 12: find descriptor of parent at preparent
	fs__cd_find_at_preparent,	// 13: find '..' at parent Kluster
	fs__ls_check_chain,		// 14: loop for Kluster chains
	failure_dumb,			// 15
};

void fs__cd__script__init () {
	script__init (& fs__cd__script, fs__cd__script_size, _null, fs__cd__script_f );
}

void fs__cd__script_task () {
	script__step (& fs__cd__script);
}