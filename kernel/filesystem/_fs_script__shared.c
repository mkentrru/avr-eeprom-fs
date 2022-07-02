/*
 * _fs_script__shared.c
 *
 * Created: 11/1/2021 9:37:28 PM
 *  Author: mkentrru
 */ 

#include "fs_scripts.h"

/*
		MEMORY
*/

void load_kluster (KID id, byte_t * d, task_id_t tid) {
	mem__request (ktoa(id), K_SIZE, d, tid, _true);
}

void store_kluster (KID id, byte_t * d, task_id_t tid) {
	mem__request (ktoa(id), K_SIZE, d, tid, _false);
}


/*
		OBJ DESCR
*/

_bool fs_obj_descr__empty (fs_obj_descr * o) {
	if (o->KID == 0) return _true;
	return _false;
}

_bool fs_obj_descr__shared (fs_obj_descr * o) {
	if ( o->TYPE == obj_type__dir && o->nA == '.' && o->nB == '.' ) 
		return _true;
	if ( fs_context.current_folder.KID == ROOT_KID &&
		o->TYPE == obj_type__file &&	
		(o->nA == 'R' || o->nA == 'U') 
		&& o->nB == 'P' ) 
		return _true;
	
	return _false;
}

_bool fs_obj_descr__good (fs_obj_descr * o) {
	// _bool fs_obj_descr__removed (fs_obj_descr * o)
	if (
	fs_obj_descr__empty (o) ||
	fs_obj_descr__shared (o) ||
	fs_obj_descr__removed (o) ) return _false;
	
	return _true;
}

_bool fs_obj_descr__free (fs_obj_descr * o) {
	if (
	fs_obj_descr__empty (o) ||
	fs_obj_descr__removed (o) ) return _true;
	
	return _false;
}

void fs_obj_descr__print (fs_obj_descr * o) {
	// "Type Owner Name FirstKID Size Removed"
	if ( o == _null ) return;
	if ( o->TYPE == obj_type__file) P_locking_output__string (PSTR("File  "), _false);
	else P_locking_output__string (PSTR("Dir   "), _false);
	
	if ( o->iROOT && o->iUSER ) P_locking_output__string(PSTR("????   "), _false);
	else if ( o->iROOT ) P_locking_output__string (PSTR("Root   "), _false);
	else if ( o->iUSER ) P_locking_output__string (PSTR("User   "), _false);
	else P_locking_output__string (PSTR("All    "), _false);
	
	if ( valid_cmd_symbol(o->nA) && valid_cmd_symbol(o->nB) ) {
		locking_output__byte (o->nA);
		locking_output__byte (o->nB);
	}
	else {
		P_locking_output__string (PSTR("__"), _false);
	}
	
	P_locking_output__string (PSTR("    "), _false);
	locking_output__hex_byte (o->KID);
	
	P_locking_output__string (PSTR("        "), _false);
	locking_output__hex_byte (o->SIZE);
	
	P_locking_output__string (PSTR("     "), _false);
	if ( fs_obj_descr__removed (o) ) {
		P_locking_output__string (PSTR(" * "), _false);
	}
	
	locking_output__new_line ();
}

_bool fs_obj_descr__access_deny (fs_obj_descr * o) {
	
	if ( o->iUSER ) {
		if ( ! fs_context.access_control.defined_user ) {
			locking_output__new_line ();
			P_locking_output__string(PSTR("User passwd undefined."), _true);
			return _true;
		}
		else if (! fs_context.access_control.user) {
			return _true;
		}
	}
	
	if ( o->iROOT ) {
		if ( ! fs_context.access_control.defined_root ) {
			locking_output__new_line ();
			P_locking_output__string(PSTR("Root passwd undefined."), _true);
			return _true;
		}
		else if (! fs_context.access_control.root) {
			return _true;
		}
	}
	
	return _false;
}

_bool fs_obj_descr__removed (fs_obj_descr * o) {
	
	if ( o->KID == KP_NAN && 
	valid_cmd_symbol(o->nA) && 
	valid_cmd_symbol(o->nB) )
		return _true;	
	
	return _false;
}

_bool fs_obj_descr__available (fs_obj_descr * o) {
	if ( fs_obj_descr__access_deny (o) ) {
		locking_output__new_line ();
		P_locking_output__string (PSTR("Access to Object denied."), _true);
		return _false;
	}
	if ( fs_obj_descr__removed (o) ) {
		locking_output__new_line ();
		P_locking_output__string (PSTR("Object is removed."), _true);
		return _false;
	}
	
	return _true;
}

fs_obj_descr * fs_obj_descr__get_by_name (byte_t nA, byte_t nB, byte_t TYPE) {
	fs_obj_descr * o = fs_context.current_obj_buf.as_directory.items;
	for ( byte_t i = 0; i < (K_SIZE / sizeof(fs_obj_descr)); i++, o++ ) {
		
		if ( o->nA == nA && o->nB == nB && o->TYPE == TYPE ) {
			return o;
		}
	}
	return _null;
}

fs_obj_descr * fs_obj_descr__get_by_KID (KID id, byte_t TYPE) {
	fs_obj_descr * o = fs_context.current_obj_buf.as_directory.items;
	for ( byte_t i = 0; i < (K_SIZE / sizeof(fs_obj_descr)); i++, o++ ) {
		if ( o->KID == id && o->TYPE == TYPE ) {
			return o;
		}
	}
	return _null;
}

void fs_obj_descr__print_info (fs_obj_descr * o) {
	KID nid =  fs_kpointer__get_first_kid (o->KID);
	KID * kp = fs_context.intro.as_intro.kp;
	P_locking_output__string (PSTR("Name: "), _false);
	locking_output__byte (o->nA);
	locking_output__byte (o->nB);
	locking_output__new_line ();
	
	P_locking_output__string (PSTR("Kluster chain: "), _false);
	
	if ( kp [nid - 1] == KP_REM ) {
		P_locking_output__string (PSTR("removed"), _true);
	}
	
	else {
		do {
			locking_output__hex_byte (nid);
			locking_output__byte ('>');
			nid = kp [nid - 1];
		} while ( nid != KP_REM && nid != KP_END );
		
		locking_output__new_line ();
		if ( o->TYPE == obj_type__file ) {
			P_locking_output__string (PSTR("Size: "), _false);
			locking_output__hex_byte (fs_context.current_obj.SIZE);
			locking_output__new_line ();
		}
	}
}

/*
		INTRO KLUSTER POINTERS
*/

_bool fs_kpointer__obj_deleted (KID id) {
	if (* (((KID *) fs_context.intro.row) + id) == KP_REM)
	return _true;
	return _false;
}

KID fs_kpointer__get_first_kid (KID id) {
	if (id == INTRO_KID) return INTRO_KID;
	
	KID * p = (KID *) fs_context.intro.row;
	for ( byte_t i = ROOT_KID; i < K_COUNT; i++ ) {
		if ( p [i] == id ){
			return fs_kpointer__get_first_kid (p [i]);
		}
	}
	
	return id;
}

KID fs_kpointer__get_last_kid (KID id) {
	KID * kp = (KID *) fs_context.intro.as_intro.kp;
	KID res = id, cid = id;
	do{
		res = kp [cid - 1];
		if ( res == KP_END ) return cid;
		cid = res;
	} while (res != KP_REM && res != KP_NAN);
	
	return KP_NAN;
}

KID fs_kpointer__get_next_kid (KID id) {
	if ( id == KP_END ) return KP_END;
	return * (((KID *) fs_context.intro.row) + id);
}

void fs_kpointer__mark_removed (KID id) {
	KID * p = (KID *) fs_context.intro.as_intro.kp;
	KID pid = fs_kpointer__get_first_kid (id),
	nid = 0;
	byte_t buf = KP_REM;
	do {
		
		nid = p [pid - 1];
		fs_K__change
		(& fs_context.intro, & fs_context.intro__memsync, pid, & buf, sizeof(buf));
		
	} while ( nid != KP_END && nid != KP_REM);
	
	union intro_flags f;
	f.as_byte = fs_context.intro.as_intro.flags.as_byte;
	if (f.as_flags.fs_free_amount < K_COUNT) {
		f.as_flags.fs_free_amount++;
		fs_K__change
		(& fs_context.intro, & fs_context.intro__memsync, 0, & f.as_byte, sizeof(byte_t));
	}
}

/*
		KLUSTER CHANGES SYNC
*/

void fs_K__change (K_t * k, memsync_control * ms, kaddr_t addr, void * data, byte_t s) {
	
	byte_t * p = k->row + addr;
	byte_t * d = (byte_t *) data;
	memsync_control mask = 1;
	for ( byte_t a = 0; a < s; a++, p++ ) {
		// locking_output__hex_byte((addr - a));
		* p = d [a];
		mask <<= (addr + a);
		* ms |= mask;
		mask = 1;
	}
}

void fs_K__mark_changed (memsync_control * ms) {
	* ms = (memsync_control) (-1);
}

exit_status fs_K__get_changed (memsync_control * ms, memaddress * addr, memaddress * size) {
	memaddress s = 0;
	memaddress a = 0;
	memsync_control flag = 1;
	
	// skip '\0' at memsync
	while ( flag && ! (flag & * ms) ) {
		a++;
		flag <<= 1;
	}
	
	// no sense if flag has been turned to '0' with <<
	while ( flag & * ms ) {
		s++;
		* ms &= ~flag;
		flag <<= 1;
	}
	
	// if no '1' has been found
	if ( ! s ) {
		return exit_failure;
	}
	
	* addr += a;
	* size = s;
	
	return exit_success;
}

/*
		SIMPLE SCRIPT STEPs
*/

script_result failure_dumb () {
	put_prompt ();
	return script__exit;
}
