/*
 * _fs_script__create.c
 *
 * Created: 11/2/2021 12:46:03 AM
 *  Author: mkentrru
 */ 

#include "fs_scripts.h"

// !! at least 1 is needed !!
#define btoks(a) (1 + (a / K_SIZE))
fs_obj_descr tmp_descr;

script_result fs__sync__current_K () {
	
	memaddress offset = 0;
	memaddress addr = ktoa(fs_context.current_obj.KID);
	memaddress size = 0;
	
	// if found no changes
	if ( fs_K__get_changed (& fs_context.current_obj__memsync, & offset, & size ) )
	return script__next;
	
	mem__request (addr + offset, size, fs_context.current_obj_buf.row + offset, current_task_id, _false);
	
	return script__stay;
}

#define valid_file_symbol(c) ((c >= 0x20 && c <= 0x7E))

script_result fs__touch__entry (struct script * s) {
	
	byte_t size = 0;
	
	P_locking_output__string (PSTR("Create file with data: "), _false);
	char * d = (char *) s->data;
	if ( d != _null ) {
		while ( valid_file_symbol (* d) && size < MAX_OBJ_SIZE ) {
			locking_output__byte (* d);
			d++;
			size++;
		}
	}
	
	P_locking_output__string (PSTR(" ("), _false);
	locking_output__hex_byte (size);
	locking_output__byte (')');
	locking_output__new_line ();
	if ( btoks(size) > 
	fs_context.intro.as_intro.flags.as_flags.fs_free_amount ) {
		P__put__string (PSTR("No free space for this size."), _true);	
	}
	fs_context.current_obj.SIZE = size;
	
	fs_context.current_folder.KID = 
	fs_kpointer__get_first_kid (fs_context.current_folder.KID);
	
	* fs_context.search_stack = 0;
	
	// to restore current folder after all
	* (fs_context.search_stack + 1) = fs_context.current_folder.KID;
	
	fs_context.descr_pointer = _null;
	
	return script__next;
}

script_result fs__create__check_same () {
	fs_obj_descr * s = fs_context.current_obj_buf.as_directory.items;
	for (fs_obj_descr * o = fs_context.current_obj_buf.as_directory.items;
	o < s + (K_SIZE / sizeof(fs_obj_descr)); o++
	){
		if ( fs_obj_descr__good(o) ) {
			if ( 
			o->TYPE == fs_context.current_obj.TYPE &&
			o->nA == fs_context.current_obj.nA &&
			o->nB == fs_context.current_obj.nB
			) {
				return 3;
			}
		}
		else if ( fs_obj_descr__free (o) ) {
			// fs_obj_descr__print (o);
			fs_context.descr_pointer = o;
			* fs_context.search_stack = fs_context.current_obj.KID;
			
			// P_locking_output__string (PSTR("Found at: "), _false);
			//KID t = fs_context.current_obj.KID;
			//locking_output__hex_buffer((byte_t *) & t, sizeof(KID));
		}
	}
	
	return script__next;
}

script_result fs__create__same_found () {
	P__put__string (PSTR("Same Object exists."), _true);
	locking_output__prompt ();
	return script__exit;
}

script_result simply_end () {
	fs_context.current_folder.KID = * (fs_context.search_stack + 1);
	
	locking_output__new_line ();
	locking_output__prompt ();
	return script__exit;
}


void fs_kpointer__reduce_free_value (byte_t count) {
	union intro_flags f;
	f.as_byte = fs_context.intro.as_intro.flags.as_byte;
	if (f.as_flags.fs_free_amount) {
		f.as_flags.fs_free_amount -= count;
		fs_K__change
		(& fs_context.intro, & fs_context.intro__memsync, 0, & f.as_byte, sizeof(byte_t));
	}
}


KID fs__K__allocate (KID fid) {
	
	KID * kp = (KID *) fs_context.intro.as_intro.kp;
	KID nid = ROOT_KID + 1;
	KID pid = KP_END;
	
	while ( kp [nid - 1] != KP_REM && 
	kp [nid - 1] != KP_NAN ) {
		nid++;
		if ( nid >= K_COUNT ) {
			P_locking_output__string (PSTR("! There are no Klusters left !"), _true);
			return KP_NAN;
		}
	}
	
	
	//kp [pid - 1] = nid;
	if ( fid != KP_NAN ) {
		pid = fs_kpointer__get_last_kid (fid);
		fs_K__change 
		(& fs_context.intro, & fs_context.intro__memsync, pid, & nid, sizeof(byte_t));
	}
	
	//kp [nid - 1] = KP_END;
	P_locking_output__string(PSTR("Allocated: "), _false);
	locking_output__hex_byte (nid);
	locking_output__new_line ();
	
	pid = KP_END;
	fs_K__change
	(& fs_context.intro, & fs_context.intro__memsync, nid, & pid, sizeof(byte_t));
	
	fs_kpointer__reduce_free_value (1);
	
	return nid;
}



script_result fs__create__build_new_descr () {
	
	P_locking_output__string(PSTR("Building new descriptor."), _true);
	
	KID nid = KP_NAN;
	if ( * fs_context.search_stack ) fs_context.current_folder.KID = * fs_context.search_stack;
	
	// means contains no free descr
	if ( fs_context.descr_pointer == _null ) {
		P_locking_output__string (PSTR("No free descriptors found."), _true);
		
		// 0: allocate new kluster for Dir
		//fs_context.current_folder.KID = 
		//fs__K__allocate (fs_context.current_folder.KID);
		nid = fs__K__allocate (fs_context.current_folder.KID);
		if ( nid == KP_NAN ) return 11;
		
		fs_context.current_folder.KID = nid;
		// 1: take first descriptor
		fs_context.descr_pointer = (fs_obj_descr *) fs_context.current_obj_buf.as_directory.items;
	}
	
	// fs_obj_descr__print (fs_context.descr_pointer);
	
	tmp_descr.iROOT = fs_context.current_obj.iROOT;
	tmp_descr.iUSER = fs_context.current_obj.iUSER;
	nid = fs__K__allocate (KP_NAN);
	if ( nid == KP_NAN ) return 11;
	
	tmp_descr.KID = nid;
	
	tmp_descr.nA = fs_context.current_obj.nA;
	tmp_descr.nB = fs_context.current_obj.nB;
	tmp_descr.SIZE = fs_context.current_obj.SIZE;
	tmp_descr.TYPE = fs_context.current_obj.TYPE;
	// fs_obj_descr__print (& tmp_descr);
	
	fs_context.current_obj.KID = fs_context.current_folder.KID;
	
	return 2;
}


script_result fs__create__write_new_descr () {
	
	memaddress offset =
	(byte_t *) fs_context.descr_pointer - (byte_t *) fs_context.current_obj_buf.as_directory.items;
	
	//locking_output__hex_buffer ((byte_t *) & fs_context.current_obj_buf.row, sizeof(K_t));
	//locking_output__hex_buffer ((byte_t *) & fs_context.current_obj__memsync, sizeof(memsync_control));
	
	fs_K__change 
	(& fs_context.current_obj_buf, & fs_context.current_obj__memsync, offset,
	& tmp_descr, sizeof(fs_obj_descr));
	
	//locking_output__new_line ();
	//locking_output__hex_buffer ((byte_t *) & fs_context.current_obj_buf.row, sizeof(K_t));
	//locking_output__hex_buffer ((byte_t *) & fs_context.current_obj__memsync, sizeof(memsync_control));
	
	return script__next;
}





script_result fs__touch__write_data__setup () {
	fs_context.current_obj.KID = tmp_descr.KID;
	
	return script__next;
}

script_result fs__touch__write_data (struct script * s) {
	fs_obj_descr * o = & tmp_descr;
	byte_t * data = (byte_t *) s->data;
	
	byte_t a = 0;
	
	for ( a = 0; a < K_SIZE && a < o->SIZE; a++, data++ ) {
		fs_context.current_obj_buf.row [a] = * data;
	}
	
	o->SIZE -= a;
	s->data = data;
	
	// zero any data left
	while (a < K_SIZE) {
		fs_context.current_obj_buf.row [a++] = 0x00;	
	}
	
	// locking_output__hex_buffer ((byte_t *) & fs_context.current_obj_buf.row, sizeof (K_t));
	
	fs_K__mark_changed (& fs_context.current_obj__memsync);
	return script__next;
}


script_result fs__touch__write__check () {
	// P_locking_output__string(PSTR("Checking if data left."), _true);
	fs_obj_descr * o = & tmp_descr;
	KID nid = 0;
	if ( o->SIZE ) {
		nid = fs__K__allocate(tmp_descr.KID);
		tmp_descr.KID = nid;
		return -4;
	}
	
	P_locking_output__string(PSTR("Data written to file."), _true);
	
	return script__next;
}

#define fs__touch__script_size 19
script_result (* fs__touch__script_f [fs__touch__script_size]) (struct script *)  = {
	fs__touch__entry,				// 0: cound size and set KID to load
	
	fs__ls_entry,					// 1: check access
	fs__ls_load_kluster,			// 2: load K
	fs__create__check_same,			// 3: if obj is same: +3; else: +1;
	fs__ls_check_chain,				// 4: if chain ended: +1; else: -2;
	
	fs__create__build_new_descr,	// 5: get free obj_descr and allocate K for Obj (+2 to skip error)
	fs__create__same_found,			// 6: msg 'found' and exit
	
	fs__ls_load_kluster,			// 7: load K witch contains free obj descriptor
	fs__create__write_new_descr,	// 8: write new descr to current dir
	fs__sync__intro,				// 9: sync intro
	fs__sync__current_K,			// 10: sync Dir
	
	/*
		write data:
		
	0.	Set c_obj KID = new_obj_descr 
	1.	Load c_obj
	2.	Write data to c_obj
	3.	Sync current K
	4.	If data left:
			allocate KID to c_obj
			goto 1
		Else:
			sync intro
	*/
	fs__touch__write_data__setup,	// 11: get ready to write data
	fs__ls_load_kluster,			// 12: load current K
	fs__touch__write_data,			// 13: write data to current K
	fs__sync__current_K,			// 14: sync changes with memory
	fs__touch__write__check,		// 15: check if any data left
	
	fs__count_free,
	fs__sync__intro,				// 16: sync intro

	
	simply_end						// 17: simpy exit
};


void fs__touch__script__init (void * d) {
	script__init (& fs__touch__script, fs__touch__script_size, d, fs__touch__script_f );
}

void fs__touch__script_task () {
	script__step (& fs__touch__script);
}


script_result fs__mkdir__entry (struct script * s) {
	
	P_locking_output__string (PSTR("Create Directory."), _true);
	fs_context.current_obj.SIZE = 0;
	
	fs_context.current_folder.KID =
	fs_kpointer__get_first_kid (fs_context.current_folder.KID);
	
	* fs_context.search_stack = 0;
	
	// to restore current folder after all
	* (fs_context.search_stack + 1) = fs_context.current_folder.KID;
	
	fs_context.descr_pointer = _null;
	
	return script__next;
}


script_result fs__mkdir__write_parent_descr__setup () {
	fs_context.current_obj.KID = tmp_descr.KID;
	
	return script__next;
}


script_result fs__mkdir__write_parent_descr () {
	fs_obj_descr * o = & tmp_descr;
	
	// locking_output__hex_buffer ((byte_t *) & fs_context.current_obj_buf.row, sizeof (K_t));
	o->iROOT = 0;
	o->iUSER = 0;
	o->KID = * (fs_context.search_stack + 1);
	o->nA = '.';
	o->nB = '.';
	o->SIZE = 0;
	o->TYPE = obj_type__dir;
	
	fs_K__change (& fs_context.current_obj_buf, & fs_context.current_obj__memsync,
	0, o, sizeof (fs_obj_descr));
	
	return script__next;
}


#define fs__mkdir__script_size 17
script_result (* fs__mkdir__script_f [fs__mkdir__script_size]) (struct script *)  = {
	fs__mkdir__entry,				// 0: cound size and set KID to load
	
	fs__ls_entry,					// 1: check access
	fs__ls_load_kluster,			// 2: load K
	fs__create__check_same,			// 3: if obj is same: +3; else: +1;
	fs__ls_check_chain,				// 4: if chain ended: +1; else: -2;
	
	fs__create__build_new_descr,	// 5: get free obj_descr and allocate K for Obj (+2 to skip error)
	fs__create__same_found,			// 6: msg 'found' and exit
	
	fs__ls_load_kluster,			// 7: load K witch contains free obj descriptor
	fs__create__write_new_descr,	// 8: write new descr to current dir
	fs__sync__intro,				// 9: sync intro
	fs__sync__current_K,			// 10: sync Dir

	fs__mkdir__write_parent_descr__setup,	// 11: get ready to write data
	fs__ls_load_kluster,			// 12: load current K
	fs__mkdir__write_parent_descr,	// 13: 
	fs__sync__current_K,			// 14: 
	fs__sync__intro,				// 15: sync intro
	
	simply_end						// 16: simpy exit
};


void fs__mkdir__script__init () {
	script__init (& fs__mkdir__script, fs__mkdir__script_size, _null, fs__mkdir__script_f );
}

void fs__mkdir__script_task () {
	script__step (& fs__mkdir__script);
}



