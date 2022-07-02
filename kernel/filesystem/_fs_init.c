/*
 * _fs_init.c
 *
 * Created: 11/1/2021 10:26:10 PM
 *  Author: mkentrru
 */ 

#include "fs_scripts.h"
#include "../ssi.h"
#include "../../lab/lab.h"


const char command_prompt [] = "> ";

int init_filesystem () {
	
	mem_busy__read_area (ktoa(INTRO_KID), K_SIZE, (byte_t *) & fs_context.intro.row);
	fs_context.intro__memsync = 0;
	
	fs_context.current_folder.KID = ROOT_KID;
	
	fs_context.current_obj.KID = fs_context.current_folder.KID;
	fs_context.current_obj__memsync = 0;
	
	fs_context.current_obj.nA = ':';
	fs_context.current_obj.nB = '/';
	fs_context.current_folder.nA = ':';
	fs_context.current_folder.nB = '/';
	
	
	fs_context.access_control.root = 0;
	fs_context.access_control.user = 0;
	fs_context.access_control.defined_root = 0;
	fs_context.access_control.defined_user = 0;
	
	fs_context.search_stack [0] = 0;
	fs_context.search_level = 0;
	
	fs_context.ssi_showing_name = _true;
	fs__ssi__update ();
	
	
	KID co = fs_context.current_obj.KID;
	fs_obj_descr * p = _null;
	
	do {
		
		mem_busy__read_area (ktoa(co), K_SIZE, (byte_t *) & fs_context.current_obj_buf.row);
		
		if ( ! fs_context.access_control.defined_root ) {
			p = fs_obj_descr__get_by_name ('R', 'P', obj_type__file);
			if ( p != _null && p->KID != KP_NAN ) {
				fs_context.access_control.rp = mem_busy__read ( ktoa(p->KID) );
				fs_context.access_control.defined_root = _true;
				P_locking_output__string (PSTR("Root passwd found: "), _false);
				locking_output__hex_buffer ((byte_t *) & fs_context.access_control.rp, sizeof(passwd_t));
			}
		}
		
		if ( ! fs_context.access_control.defined_user ) {
			p = fs_obj_descr__get_by_name ('U', 'P', obj_type__file);
			if ( p != _null && p->KID != KP_NAN ) {
				fs_context.access_control.up = mem_busy__read ( ktoa(p->KID) );
				fs_context.access_control.defined_user = _true;
				P_locking_output__string (PSTR("User passwd found: "), _false);
				locking_output__hex_buffer ((byte_t *) & fs_context.access_control.up, sizeof(passwd_t));
			}
		}
		
	} while ( (co = fs_kpointer__get_next_kid (co)) != KP_END &&
	! ( fs_context.access_control.defined_root && fs_context.access_control.defined_user ) );
	
	// if not first Kluster of ROOT_DIR loaded
	if ( fs_context.current_obj.KID != co ){
		mem_busy__read_area 
		(ktoa(fs_context.current_obj.KID), K_SIZE, (byte_t *) & fs_context.current_obj_buf.row);
	}
	
	if ( ! fs_context.access_control.defined_root )
		P_locking_output__string (PSTR("Root passwd NOT found!"), _true);
	if ( ! fs_context.access_control.defined_user )
		P_locking_output__string (PSTR("User passwd NOT found!"), _true);
	
	return exit_success;
}


void fs__ssi__update () {
	if ( fs_context.ssi_showing_name ) {
		_ssi__set_word
		(fs_context.current_folder.nA, fs_context.current_folder.nB);
	}
	else {
		_ssi__set_word (0, fs_context.current_folder.KID);
	}
}

void fs__ssi__switch () {
	if ( fs_context.ssi_showing_name == _true ) fs_context.ssi_showing_name = _false;
	else fs_context.ssi_showing_name = _true;
	
	fs__ssi__update ();
}



exit_status fs__check__cluster_chain () {
	memsync_control checked  = 0, mask = 1;
	KID * b = (KID *) & fs_context.intro.row, nid = ROOT_KID;
	
	for ( KID fid = ROOT_KID; fid < K_COUNT; fid++ ) {
		
		mask = 1; mask <<= fid;
		
		P_locking_output__string(PSTR("{"), _false);
		locking_output__hex_byte(fid);
		
		if ( * (b + fid) == KP_END || * (b + fid) == KP_NAN || * (b + fid) == KP_REM ) {
			P_locking_output__string(PSTR("}"), _false);
			checked |= mask;
			continue;
		}
		
		if ( ! (mask & checked) ) {
			
			checked |= mask;
			
			nid = fid;
			while ( * (b + nid) != KP_END ) {
				
				nid = * (b + nid);
				
				P_locking_output__string(PSTR(">"), _false);
				locking_output__hex_byte(nid);
				
				if ( nid == KP_REM || nid == KP_NAN ) {
					// using wrong Kluster !
					P_locking_output__string(PSTR("}"), _true);
					P_locking_output__string (PSTR("Chain contains removed/undefined Kluster!"), _true);
					return exit_failure;
				}
				
				mask = 1; mask <<= nid;
				
				if ( mask & checked ) {
					// Kluster is used twice
					P_locking_output__string(PSTR("}"), _true);
					P_locking_output__string (PSTR("Single Kluster is used in different chains!"), _true);
					return exit_failure;
				}
				
				checked |= mask;
				
			}
		}
	
		P_locking_output__string(PSTR("}"), _false);
		
	}
	locking_output__new_line ();
	
	P_locking_output__string(PSTR("Free: "), _false);
	byte_t t = fs_context.intro.as_intro.flags.as_flags.fs_free_amount;
	locking_output__hex_byte (t);
	locking_output__new_line ();
	
	return exit_success;
}

void put_prompt () {
	if ( fs_context.access_control.root ) put_byte ('r');
	else put_byte ('-');
	
	if ( fs_context.access_control.user ) put_byte ('u');
	else put_byte ('-');
	
	put_string (command_prompt, _false);
	fs__ssi__update ();
	enable_input
}

void locking_output__prompt () {
	if ( fs_context.access_control.root ) locking_output__byte ('r');
	else locking_output__byte ('-');
	
	if ( fs_context.access_control.user ) locking_output__byte ('u');
	else locking_output__byte ('-');
	
	locking_output__string (command_prompt, _false);
	fs__ssi__update ();
	enable_input
}