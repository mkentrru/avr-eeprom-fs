/*
 * user_uart.c
 *
 * Created: 9/15/2021 1:35:25 AM
 *  Author: mkentrru
 */ 

#include "lab.h"


#define CMD_NONE	0x00
#define CMD_WRONG	0x01
#define CMD_HELP	0x02
#define CMD_LOGIN	0x03
void cmd__login ();
#define CMD_LS		0x04
void cmd__ls ();
#define CMD_CAT		0x05
void cmd__cat ();
#define CMD_CD		0x06
void cmd__cd ();
#define CMD_LOGOUT	0x07
void cmd_logout ();
#define CMD_RM		0x08
void cmd__rm ();
#define CMD_INTRO	0x09
void cmd__intro ();
#define CMD_INFO	0x0A
void cmd__info ();
#define CMD_TOUCH	0x0B
void cmd__touch ();
#define CMD_MKDIR	0x0C
void cmd__mkdir ();
#define CMD_PASSWD	0x0D
void cmd__passwd ();
#define CMD_CHCHECK	0x0E
void cmd__chaincheck ();

struct _args args;


char * string_cut (char * s, char ** pos) {
	// skip \' ' at beginning
	while ( * s == ' ' ) {
		s++;
	}
	char * res = s;
	* pos = s;

	while ( valid_cmd_symbol( * s ) )
	s++;

	if (s == * pos) {
		* pos = _null;
		res = _null;
	}
	else {
		* pos = s;
	}

	return res;
}

exit_status substring_compare (char * a, char * b) {
	while ( valid_cmd_symbol(* a) || valid_cmd_symbol(* b) ) {
		if ( * a != * b ) return exit_failure;
		a++; b++;
	}
	return exit_success;
}

byte_t cover_input_string (char * s, struct _args * args ){
	args->argc = 0;
	
	char * a = s;
	char * r = s;
	while ( (r = string_cut (s, & a)) != _null && args->argc < MAX_ARGCOUNT ) {
		args->argv[args->argc++] = r;
		s = a;
	}
	
	if ( args->argc == 0 ) {
		return CMD_NONE;
	}
	
	if ( substring_compare(args->argv[0], "help") == exit_success )		return CMD_HELP;
	if ( substring_compare(args->argv[0], "ls") == exit_success )		return CMD_LS;
	if ( substring_compare(args->argv[0], "cat") == exit_success )		return CMD_CAT;
	if ( substring_compare(args->argv[0], "cd") == exit_success )		return CMD_CD;
	if ( substring_compare(args->argv[0], "login") == exit_success )	return CMD_LOGIN;
	if ( substring_compare(args->argv[0], "logout") == exit_success )	return CMD_LOGOUT;
	if ( substring_compare(args->argv[0], "passwd") == exit_success )	return CMD_PASSWD;
	if ( substring_compare(args->argv[0], "info") == exit_success )		return CMD_INFO;
	if ( substring_compare(args->argv[0], "rm") == exit_success )		return CMD_RM;
	if ( substring_compare(args->argv[0], "touch") == exit_success )	return CMD_TOUCH;
	if ( substring_compare(args->argv[0], "intro") == exit_success )	return CMD_INTRO;
	if ( substring_compare(args->argv[0], "mkdir") == exit_success )	return CMD_MKDIR;
	if ( substring_compare(args->argv[0], "check") == exit_success )	return CMD_CHCHECK;
	
	
	return CMD_WRONG;
}

exit_status check_username (struct _args * args) {
	if ( substring_compare(args->argv[1], "root") == exit_success ) {
		fs_context.access_control.current_try = 1;
		return exit_success;
	}
	else if ( substring_compare(args->argv[1], "user") == exit_success ) {
		fs_context.access_control.current_try = 0;
		return exit_success;
	}
	put_string("Wrong Subject name.", _true);
	return exit_failure;
}

exit_status check_objtype (struct _args * args) {
	if ( substring_compare(args->argv[1], "d") == exit_success ) {
		fs_context.current_obj.TYPE = obj_type__dir;
		return exit_success;
	}
	else if ( substring_compare(args->argv[1], "f") == exit_success ) {
		fs_context.current_obj.TYPE = obj_type__file;
		return exit_success;
	}
	
	return exit_failure;
}

const char cmd_error__wrong_command [] PROGMEM = "Try \'help\' for some help.";
const char cmd_error__wrong_args [] PROGMEM = "Wrong arguments. Try \'help\'.";
const char access_denied [] PROGMEM = "Access denied";
const char wrong_name [] PROGMEM = "Wrong name.";

#define none 0
_bool correct_argc (byte_t m, byte_t l) {
	if ( l == none ) {
		if ( args.argc == m )
			return _true;
	}
	else {
		if ( args.argc >= m && args.argc <= l )
			return _true;
	}
	P_locking_output__string (cmd_error__wrong_args, _true);
	locking_output__prompt ();
	return _false;
}

void input_handler_exec (){ // does not check if buffer is empty
	
	char command_buffer [uart_input_size + 1];
	
	
	//while ( !(_stdin_empty) ){
		
		_c_buffer_pos	end_pos = 0;
		
		if ( _c_buffer__parse_string (_stdin, '\0', & end_pos, command_buffer) ) {
			// TODO: error message
			_c_buffer__fix (_stdin);
		}
		
		switch ( cover_input_string(command_buffer, &args) ) {
			case CMD_NONE:
				put_prompt ();
				break;
			
			case CMD_WRONG:
				P__put__string (cmd_error__wrong_command, _true);
				put_prompt ();
				break;
				
			case CMD_HELP:
				// put_string("Ask me about the commands.", _true);
				put_prompt ();
				break;
				
			case CMD_LS:
				if ( correct_argc (1, none) )
					cmd__ls ();
				break;
				
			case CMD_CAT:
				if ( correct_argc (2, none) )
					cmd__cat ();
				break;
				
			case CMD_LOGIN:
				if ( correct_argc (2, none) )
					cmd__login ();
				break;
			
			case CMD_LOGOUT:
				if ( correct_argc (1, none) )
					cmd_logout ();
				break;
			
			case CMD_CD:
				if ( correct_argc (2, none) )
					cmd__cd ();
				break;
			
			case CMD_RM:
				if ( correct_argc (3, none) )
					cmd__rm ();
			
				break;
				
			case CMD_INFO:
				if ( correct_argc (3, none) )
					cmd__info ();
					
				break;
			
			case CMD_INTRO:
				cmd__intro ();
				break;
			
			case CMD_TOUCH:
				if ( correct_argc (3, 4) )
					cmd__touch ();
				break;
			case CMD_MKDIR:
				if ( correct_argc (3, none) )
					cmd__mkdir ();
				break;
			case CMD_PASSWD:
				if ( correct_argc (2, none) )
					cmd__passwd ();
				break;
			case CMD_CHCHECK:	
				if ( correct_argc (1, none) )
					cmd__chaincheck ();
				break;
				
			default:
				// put_string ("Command is not covered yet. wtf", _true);
				put_prompt ();
				break;
		}
		
		
		//_c_buffer__jump (_stdin, end_pos);
		//_c_buffer__next (_stdin); // to skip IEB
		_c_buffer__fix (_stdin);
	//}
	
	// enable_input
}


void cmd__ls () {
	P__put__string (PSTR("Dir: "), _false);
	put_byte (fs_context.current_folder.nA);
	put_byte (fs_context.current_folder.nB);
	new_line ();
	
	fs__ls__script__init ();
	allow_script (fs_script_task, fs__ls__script_task);
}

void cmd__cat () {
	
	fs_context.current_obj.nA = args.argv[1][0];
	fs_context.current_obj.nB = args.argv[1][1];
	fs_context.current_obj.TYPE = obj_type__file;
	
	P__put__string(PSTR("Show data of: "), _false);
	put_byte (fs_context.current_obj.nA);
	put_byte (fs_context.current_obj.nB);
	new_line ();
	
	fs__cat__script__init ();
	allow_script (fs_script_task, fs__cat__script_task);
}

void cmd__login () {
	if ( check_username (& args) ) {
		locking_output__prompt ();
	}
	else {
		// login root
		if ( (fs_context.access_control.current_try && ! fs_context.access_control.defined_root) ||
		(! fs_context.access_control.current_try && ! fs_context.access_control.defined_user) ) {
			P_locking_output__string (PSTR("Subject passwd is undefined. Enter new passwd."), _true);
			
			passwd_init__script__init ();
			allow_script (inter_script_task, passwd_init__script_task);
		}
		else {	
			passwd_input__script__init ();
			allow_script (inter_script_task, passwd_input__script_task);
		}
		
	}
}

void cmd_logout () {
	fs_context.access_control.root = _false;
	fs_context.access_control.user = _false;
	P__put__string (PSTR("Logged out."), _true);
	put_prompt ();
}

void cmd__cd () {
	fs_context.current_obj.nA = args.argv[1][0];
	fs_context.current_obj.nB = args.argv[1][1];
	fs_context.current_obj.TYPE = obj_type__dir;
	
	P__put__string (PSTR("Change Dir to: "), _false);
	put_byte (fs_context.current_obj.nA);
	put_byte (fs_context.current_obj.nB);
	
	fs__cd__script__init ();
	allow_script (fs_script_task, fs__cd__script_task);
}

void cmd__rm () {
	if ( check_objtype (& args) ) {
		P__put__string (PSTR("Wrong Obj type."), _true);
		put_prompt ();
	}
	else {
		fs_context.current_obj.nA = args.argv[2][0];
		fs_context.current_obj.nB = args.argv[2][1];
	
		P__put__string(PSTR("Remove: "), _false);
		put_byte (fs_context.current_obj.nA);
		put_byte (fs_context.current_obj.nB);
		new_line ();
	
		fs__rm__script__init ();
		allow_script (fs_script_task, fs__rm__script_task);
	}
}

void cmd__info () {
	if ( check_objtype (& args) ) {
		P__put__string (PSTR("Wrong Obj type."), _true);
		put_prompt ();
	}
	else {
		fs_context.current_obj.nA = args.argv[2][0];
		fs_context.current_obj.nB = args.argv[2][1];
	
		P__put__string (PSTR("Info:"), _true);
	
		fs__info__script__init ();
		allow_script (fs_script_task, fs__info__script_task);
	}
}


void cmd__intro () {
	locking_output__hex_buffer ((byte_t *) fs_context.intro.row, K_SIZE);
	mem_busy__read_area (ktoa(INTRO_KID), K_SIZE, (byte_t *) & fs_context.intro.row);
	locking_output__hex_buffer ((byte_t *) fs_context.intro.row, K_SIZE);

	locking_output__hex_buffer ((byte_t *) & fs_context, sizeof(_fs_context));

	locking_output__prompt ();	
}


_bool correct_obj_rights () {
	char * s = args.argv [2];
	if ( substring_compare(s, "root") == exit_success ) {
		P_locking_output__string (PSTR("As Root Obj."), _true);
		if ( ! fs_context.access_control.root ) return _false;
		fs_context.current_obj.iROOT = 1;
		fs_context.current_obj.iUSER = 0;
		return _true;
	}
	else
	if ( substring_compare(s, "user") == exit_success ) {
		P_locking_output__string (PSTR("As User Obj."), _true);
		if ( ! fs_context.access_control.user ) return _false;
		fs_context.current_obj.iROOT = 0;
		fs_context.current_obj.iUSER = 1;
		return _true;
	}
	else
	if ( substring_compare(s, "all") == exit_success ) {
		P_locking_output__string (PSTR("As shared Obj."), _true);
		fs_context.current_obj.iROOT = 0;
		fs_context.current_obj.iUSER = 0;
		return _true;
	}
	P_locking_output__string (PSTR("Unknown user."), _true);
	return _false;
}


void cmd__touch () {
	char * d = _null;
	
	if ( ! valid_cmd_symbol(args.argv[1][0]) ||
	! valid_cmd_symbol(args.argv[1][1]) ) {
		P_locking_output__string (wrong_name, _true);
		locking_output__prompt ();
		return;
	}
	
	fs_context.current_obj.nA = args.argv[1][0];
	fs_context.current_obj.nB = args.argv[1][1];
	fs_context.current_obj.TYPE = obj_type__file;
		
	P_locking_output__string(PSTR("Create file: "), _false);
	locking_output__byte (fs_context.current_obj.nA);
	locking_output__byte (fs_context.current_obj.nB);
	locking_output__new_line ();
	
	if ( fs_obj_descr__shared (& fs_context.current_obj) || ! correct_obj_rights () ) {
		P_locking_output__string (access_denied, _true);
		locking_output__prompt ();
		return;
	}
	
	if ( args.argc == 4 ) {
		d = args.argv [3];
	}
	
	fs__touch__script__init (d);
	allow_script (fs_script_task, fs__touch__script_task);
}


void cmd__mkdir () {
	if ( ! valid_cmd_symbol(args.argv[1][0]) ||
	! valid_cmd_symbol(args.argv[1][1]) ) {
		P_locking_output__string (wrong_name, _true);
		locking_output__prompt ();
		return;
	}
	
	fs_context.current_obj.nA = args.argv[1][0];
	fs_context.current_obj.nB = args.argv[1][1];
	fs_context.current_obj.TYPE = obj_type__dir;
	
	P_locking_output__string(PSTR("Create file: "), _false);
	locking_output__byte (fs_context.current_obj.nA);
	locking_output__byte (fs_context.current_obj.nB);
	locking_output__new_line ();
	
	if ( fs_obj_descr__shared (& fs_context.current_obj) || ! correct_obj_rights () ) {
		P_locking_output__string (access_denied, _true);
		locking_output__prompt ();
		return;
	}
	
	fs__mkdir__script__init ();
	allow_script (fs_script_task, fs__mkdir__script_task);
}


/*
// passwd root
if ( (fs_context.access_control.current_try && ! fs_context.access_control.defined_root) ||
// passwd user
(! fs_context.access_control.current_try && ! fs_context.access_control.defined_user) ) {
	P_locking_output__string(PSTR("Subject passwd is undefined. Login first"), _true);
	locking_output__prompt ();
	return script__exit;
}
*/


_bool correct_subject_rights () {
	char * s = args.argv [1];
	if ( substring_compare(s, "root") == exit_success ) {
		P_locking_output__string (PSTR("Root passwd."), _true);
		if ( ! fs_context.access_control.defined_root ) {
			P_locking_output__string(PSTR("Root passwd is undefined. Login first."), _true);
			return _false;
		}
		if ( ! fs_context.access_control.root ) return _false;
		fs_context.access_control.current_try = 1;
		return _true;
	}
	else
	if ( substring_compare(s, "user") == exit_success ) {
		P_locking_output__string (PSTR("USER passwd."), _true);
		if ( ! fs_context.access_control.defined_user ) {
			P_locking_output__string(PSTR("User passwd is undefined. Login first."), _true);
			return _false;
		}
		if ( ! fs_context.access_control.user ) return _false;
		fs_context.access_control.current_try = 0;
		return _true;
	}
	
	P_locking_output__string (PSTR("Unknown user."), _true);
	return _false;
}


void cmd__passwd () {
	
	P_locking_output__string(PSTR("Change "), _false);
	if ( ! correct_subject_rights () ) {
		P_locking_output__string(PSTR("Request denied."), _true);
		locking_output__prompt ();
		return;
	}
	
	passwd_change__script__init ();
	allow_script (inter_script_task, passwd_change__script_task);
}


void cmd__chaincheck () {
	if ( fs__check__cluster_chain () ) {
		P_locking_output__string (PSTR("Chain is broken!"), _true);
	}
	else {
		P_locking_output__string (PSTR("No chain problems detected."), _true);
	}
	
	locking_output__prompt ();
}