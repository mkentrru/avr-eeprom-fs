/*
 * _fs_script__rm.c
 *
 * Created: 11/1/2021 10:49:02 PM
 *  Author: mkentrru
 */ 

#include "fs_scripts.h"

script_result fs__sync__intro () {
	memaddress offset = 0;
	memaddress addr = ktoa(INTRO_KID);
	memaddress size = 0;

	// if found no changes
	if ( fs_K__get_changed (& fs_context.intro__memsync, & offset, & size ) )
		return script__next;
	// P_locking_output__string (PSTR("Sync intro."), _true);
	mem__request (addr + offset, size, fs_context.intro.row + offset, current_task_id, _false);
	
	return script__stay;
}



#define rm__file__jump_to__sync 7
script_result fs__rm__file () {
	if ( fs_obj_descr__shared (& fs_context.current_obj) ) {
		P_locking_output__string (PSTR("Access denied."), _true);
		return 11;
	}
	
	else {
		remove__obj (fs_context.descr_pointer);
	}
	
	return script__next;
}

script_result sync__on_file_rm () {
	// P_locking_output__string(PSTR("Sync on file remove!"), _true);
	// locking_output__hex_byte (tmp_descr.KID);
	return sync__K (9, tmp_descr.KID);
}

byte_t removed_count = 0;

void remove__obj (fs_obj_descr * o) {
	
	// fs_obj_descr__print (o);
	
	if ( ! fs_obj_descr__removed (o) ) {
		fs_kpointer__mark_removed (o->KID);
		
		memaddress offset =
		(byte_t *) o - (byte_t *) fs_context.current_obj_buf.as_directory.items;
		
		// locking_output__hex_buffer ((byte_t *) & offset, sizeof(memaddress));
		
		o->KID = KP_NAN;
		// locking_output__hex_buffer ((byte_t *) & fs_context.current_obj_buf.row, sizeof (K_t));
		// fs_obj_descr__print (o);
		
		fs_K__change
		(& fs_context.current_obj_buf, & fs_context.current_obj__memsync, offset, o, sizeof(fs_obj_descr));
		
		// locking_output__hex_buffer ((byte_t *) & fs_context.current_obj_buf.row, sizeof (K_t));
		removed_count++;
	}
}


script_result fs__rm_dir__foreach () {
	
	// Obj from prev dive
	KID * last_sub_obj = fs_context.search_stack + fs_context.search_level + 1;
	fs_obj_descr * s = fs_context.current_obj_buf.as_directory.items;
	
	for (fs_obj_descr * o = fs_context.current_obj_buf.as_directory.items;
	o < s + (K_SIZE / sizeof(fs_obj_descr)); o++
	){
		
		if ( * last_sub_obj != 0 ) {
			
			if ( * last_sub_obj == o->KID ) {
				// found last object
				* last_sub_obj = 0;
				// remove
				remove__obj (o);
			}
		}
		
		else if ( fs_obj_descr__good (o) && 
			fs_context.search_level >= fs__sub__root_level ) {
			
			if ( fs_obj_descr__available (o) ) {
				
				if ( o->TYPE == obj_type__dir ) {
					
					* last_sub_obj = o->KID;
					* (last_sub_obj + 1) = 0;
					fs_context.search_level++;
					
					return 1;
				}
				else {
					
					// remove
					remove__obj (o);
				}
			}
			else {
				P__put__string (PSTR("Have no access to remove some Objects inside."), _true);
				fs_context.search_level = 0;
				return 4;
			}
		}
	}
	// locking_output__new_line ();
	return 2;
}

script_result fs__rm_check__if_file () {
	removed_count = 0;
	
	if ( fs_obj_descr__shared (& fs_context.current_obj) ) {
		P_locking_output__string (PSTR("Filesystem Obj."), _true);
		locking_output__prompt ();
		return script__exit;
	}
	
	if ( fs_context.current_obj.TYPE == obj_type__file )
	return script__next;
	
	return 3;
}

script_result fs__rm_sub__control () {
	
	if ( fs_context.search_level < fs__sub__root_level ) {
		
		fs_context.current_folder.KID = fs_context.search_stack [0];
		
		P_locking_output__string (PSTR("Removed: "), _false);
		locking_output__hex_byte (removed_count);
		
		locking_output__new_line ();

		return script__next;
	}
	
	fs_context.search_level--;
	// locking_output__string ("Loop.", _true);
	return -6;
}

script_result sync__K (script_result on_exit, KID id) {
	memaddress offset = 0;
	memaddress addr = ktoa(id);
	memaddress size = 0;
	
	// if found no changes
	if ( fs_K__get_changed (& fs_context.current_obj__memsync, & offset, & size ) )
		return on_exit;
	
	//P_locking_output__string(PSTR("Address: "), _false);
	//locking_output__hex_buffer ((byte_t *) & addr, sizeof(memaddress));
	//P_locking_output__string(PSTR("Offset: "), _false);
	//locking_output__hex_buffer ((byte_t *) & offset, sizeof(memaddress));
	//P_locking_output__string(PSTR("Size: "), _false);
	//locking_output__hex_buffer ((byte_t *) & size, sizeof(memaddress));
	
	mem__request (addr + offset, size, fs_context.current_obj_buf.row + offset, current_task_id, _false);
	
	return script__stay;
}



script_result sync__on_dive () {
	// P_locking_output__string(PSTR("Sync on dive"), _true);
	//memaddress offset = 0;
	//memaddress addr = ktoa(fs_context.current_obj.KID);
	//memaddress size = 0;
	//
	//// if found no changes
	//if ( fs_K__get_changed (& fs_context.current_obj__memsync, & offset, & size ) )
		//return -3;
	//
	//mem__request (addr + offset, size, fs_context.current_obj_buf.row + offset, current_task_id, _false);
	//
	// return script__stay;
	
	return sync__K (-3, fs_context.current_obj.KID);
}

script_result sync__on_chain () {
	// P_locking_output__string(PSTR("Sync on loop"), _true);
	//memaddress offset = 0;
	//memaddress addr = ktoa(fs_context.current_obj.KID);
	//memaddress size = 0;
	//
	//// if found no changes
	//if ( fs_K__get_changed (& fs_context.current_obj__memsync, & offset, & size ) )
		//return 1;
	//
	//mem__request (addr + offset, size, fs_context.current_obj_buf.row + offset, current_task_id, _false);
	//
	// return script__stay;
	
	return sync__K (1, fs_context.current_obj.KID);
}

script_result fs__rm_check_chain () {
	KID nid = fs_kpointer__get_next_kid (fs_context.current_obj.KID);
	if ( nid != KP_END ) {
		fs_context.current_obj.KID = nid;
		return -4;
	}
	return script__next;
}

script_result fs__rm__outro () {
	locking_output__prompt ();
	return script__next;
}


memsync_control kpointers_in_use = 0;

script_result intro_fix__entry () {
	kpointers_in_use = 0;
	return script__next;
}

script_result intro_fix__check_empty () {
	//locking_output__hex_byte (fs_context.current_obj.KID);
	//locking_output__new_line ();
	KID cid = fs_context.current_obj.KID;
	memsync_control mask = 1;
	
	mask <<= cid;
	
	fs_obj_descr * s = fs_context.current_obj_buf.as_directory.items;
	for (fs_obj_descr * o = fs_context.current_obj_buf.as_directory.items;
	o < s + (K_SIZE / sizeof(fs_obj_descr)); o++
	){
		if ( ! fs_obj_descr__empty(o) ) {
			// fs_obj_descr__print (o);
			kpointers_in_use |= mask;
		}
	}

	//locking_output__hex_buffer((byte_t*) & kpointers_in_use, sizeof(memsync_control));
	//locking_output__hex_buffer((byte_t*) & mask, sizeof(memsync_control));
//
	//locking_output__new_line ();
	return script__next;
}

script_result intro_fix__remove_unused () {
	
	KID fid = fs_kpointer__get_first_kid (fs_context.current_folder.KID),
	nid = fs_kpointer__get_next_kid (fid),
	LAST_ALIVE = fid, buf = 0;
	
	//P_locking_output__string(PSTR("Current folder and fid: "), _false);
	//locking_output__hex_byte (fs_context.current_folder.KID);
	//locking_output__hex_byte (fid); 
	//locking_output__new_line ();
	
	memsync_control mask = 1;
	
	P_locking_output__string (PSTR("Fixing unused Klusters."), _true);
	
	while ( fid != KP_END && fid != KP_REM ) {
		mask = 1;
		mask <<= fid;
		// locking_output__hex_buffer((byte_t*) & mask, sizeof(memsync_control));
		
		// Kluster is unused
		if ( ! (kpointers_in_use & mask) ) {
			if ( LAST_ALIVE ) {
				P_locking_output__string (PSTR("Not in use: "), _false);
				locking_output__hex_byte (fid); locking_output__new_line ();
				
				// fs_context.intro.as_intro.kp [LAST_ALIVE - 1] = KP_END;
				buf = KP_END;
				fs_K__change
				(& fs_context.intro, & fs_context.intro__memsync, LAST_ALIVE, & buf, sizeof(KID));
				buf = KP_REM;
				fs_K__change
				(& fs_context.intro, & fs_context.intro__memsync, fid, & buf, sizeof(KID));
				
				//locking_output__hex_buffer ((byte_t *) & LAST_ALIVE, sizeof(KID));
				//locking_output__hex_buffer ((byte_t *) & fs_context.intro.row, sizeof (K_t));
			}
		}
		else {
			
			kpointers_in_use &= ~(mask);
			if ( LAST_ALIVE ) {
				//fs_context.intro.as_intro.kp [LAST_ALIVE - 1] = fid;
				buf = fid;
				fs_K__change
				(& fs_context.intro, & fs_context.intro__memsync, LAST_ALIVE, & buf, sizeof(KID));
			}
			LAST_ALIVE = fid;
		}
		
		fid = nid;
		nid = fs_kpointer__get_next_kid (fid);
	}
	
	//fs_context.intro.as_intro.kp [LAST_ALIVE - 1] = KP_END;
	buf = KP_END;
	fs_K__change
	(& fs_context.intro, & fs_context.intro__memsync, LAST_ALIVE, & buf, sizeof(KID));
	
	return script__next;
}

script_result fs__count_free () {
	union intro_flags f;
	f.as_flags.fs_free_amount = 0;
	f.as_flags.fs_type = FS_CONST;
	
	KID * kp = (KID *) & fs_context.intro.row;
	for ( KID id = ROOT_KID; id < K_COUNT; id++ ) {
		if ( kp [id] == KP_NAN || kp [id] == KP_REM )
		f.as_flags.fs_free_amount++;
	}
	fs_K__change (& fs_context.intro, & fs_context.intro__memsync, 
	0, & f, sizeof(union intro_flags));
	
	return script__next;
}

#define fs__rm__script_size 24
script_result (* fs__rm__script_f [fs__rm__script_size]) (struct script *)  = {
	fs__ls_entry,			// 0: current_obj.KID = current_folder.KID
	fs__ls_load_kluster,	// 1: load current_obj.KID Kluster
	fs__find__by_name,		// 2: if found jumps to (5)
	fs__ls_check_chain,		// 3: if chain ended jumps to (4)
	fs__find__failure,		// 4: message about failure, exit
	
	fs__rm_check__if_file,		// 5: if file: +1; else: +2;
	fs__rm__file,			// 6: show file info
	sync__on_file_rm,
	
	fs__sub__setup,			// 7: setup sub search
	
	fs__sub__entry,			// 8: select search level - Dir
	fs__ls_load_kluster,	// 9: load Dir K-n
	fs__rm_dir__foreach,	// 10: work around Dir
	
	sync__on_dive,
	
	sync__on_chain,
	fs__rm_check_chain,		// 11: check Dir K-chain
	
	// memsync and intro fix
	fs__rm_sub__control,		// 12: check if finished
	
	fs__ls_entry,
	fs__ls_load_kluster,
	intro_fix__check_empty,
	fs__ls_check_chain,
	
	intro_fix__remove_unused,
	fs__count_free,
	
	fs__sync__intro,		// 13: sync RAM intro and EEPROM intro
	fs__rm__outro			// 14: simple outro
};


void fs__rm__script__init () {
	script__init (& fs__rm__script, fs__rm__script_size, _null, fs__rm__script_f );
}

void fs__rm__script_task () {
	script__step (& fs__rm__script);
}