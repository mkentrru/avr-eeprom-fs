/*
 * fs_scripts.h
 *
 * Created: 11/1/2021 9:30:58 PM
 *  Author: mkentrru
 */ 


#ifndef FS_SCRIPTS_H_
#define FS_SCRIPTS_H_

#include "filesystem.h"

/*
		SHARED OBJECTS
*/


extern _fs_context fs_context;

/*
		MEMORY FUNCTIONS
*/

void	load_kluster (KID id, byte_t * d, task_id_t tid);
void	store_kluster (KID id, byte_t * d, task_id_t tid);


/*
		OBJ DESCR
*/

_bool fs_obj_descr__empty (fs_obj_descr * o);
_bool fs_obj_descr__removed (fs_obj_descr * o);
_bool fs_obj_descr__shared (fs_obj_descr * o);
_bool fs_obj_descr__good (fs_obj_descr * o);
_bool fs_obj_descr__free (fs_obj_descr * o);

_bool fs_obj_descr__access_deny (fs_obj_descr * o);
_bool fs_obj_descr__available (fs_obj_descr * o);

void fs_obj_descr__print (fs_obj_descr * o);
void fs_obj_descr__print_info (fs_obj_descr * o);

fs_obj_descr * fs_obj_descr__get_by_name (byte_t nA, byte_t nB, byte_t TYPE);
fs_obj_descr * fs_obj_descr__get_by_KID (KID id, byte_t TYPE);


void remove__obj (fs_obj_descr * o);

/*
		INTRO KLUSTER POINTERS
*/

_bool	fs_kpointer__obj_deleted (KID id);
KID		fs_kpointer__get_first_kid (KID id);
KID		fs_kpointer__get_last_kid (KID id);
KID		fs_kpointer__get_next_kid (KID id);
void	fs_kpointer__mark_removed (KID id);
void	fs_kpointer__reduce_free_value (byte_t count);
script_result fs__count_free ();

/*
		KLUSTER CHANGES SYNC
*/

void fs_K__change (K_t * k, memsync_control * ms, kaddr_t addr, void * data, byte_t s);
void fs_K__mark_changed (memsync_control * ms);
exit_status fs_K__get_changed (memsync_control * ms, memaddress * addr, memaddress * size);
KID fs__K__allocate (KID fid);




//	S		C		R		I		P		T		S//




/*
		ls - LIST DIRECTORY
*/

script_result fs__ls_entry ();
script_result fs__ls_load_kluster ();
#define fs__ls_chain_loop -2
script_result fs__ls_check_chain ();
script_result fs__outro ();

extern script fs__ls__script;
void fs__ls__script__init ();
void fs__ls__script_task ();


/*
		cat - DISPLAY FILE DATA
*/

script_result fs__find__by_name ();
script_result fs__find__by_KID ();
script_result fs__find__failure ();

extern script fs__cat__script;
void fs__cat__script__init ();
void fs__cat__script_task ();

/*
		cd - CHANGE DIRECTORY
*/



extern script fs__cd__script;
void fs__cd__script__init ();
void fs__cd__script_task ();

/*
		info - DISPLAY OBJECT INFO
*/

script_result fs__check__if_file ();
script_result fs__sub__setup ();
script_result fs__sub__entry ();
#define fs__sub__root_level 1
#define fs__sub__loop -4
#define fs__sub__dive -2
script_result fs__sub__control ();

extern script fs__info__script;
void fs__info__script__init ();
void fs__info__script_task ();

/*
		rm - REMOVE OBJECT (RECURSIVELY)
*/

script_result fs__sync__intro ();
script_result sync__K (script_result on_exit, KID id);

extern script fs__rm__script;
void fs__rm__script__init ();
void fs__rm__script_task ();

/*
		touch - CREATE FILE
*/

script_result fs__sync__current_K ();
script_result fs__create__check_same ();
script_result fs__create__build_new_descr ();
script_result fs__create__write_new_descr ();
script_result fs__touch__write_data__setup ();
script_result fs__touch__write_data ();
script_result fs__touch__write__check ();


extern script fs__touch__script;
void fs__touch__script__init (void * d);
void fs__touch__script_task ();


/*
		mkdir - CREATE DIRECTORY
*/

extern script fs__mkdir__script;
void fs__mkdir__script__init ();
void fs__mkdir__script_task ();


/*
		login - LOGIN AS ROOT/USER
*/

extern script passwd_input__script;
void passwd_input__script__init ();
void passwd_input__script_task ();

	// if *P file does not exists
extern script passwd_init__script;
void passwd_init__script__init ();
void passwd_init__script_task ();

/*
		passwd - CHANGE PASSWD
*/

extern script passwd_change__script;
void passwd_change__script__init ();
void passwd_change__script_task ();


/*
		SIMPLE SCRIPT STEPs
*/

script_result failure_dumb ();
script_result simply_end ();

extern fs_obj_descr tmp_descr;

#endif /* FS_SCRIPTS_H_ */