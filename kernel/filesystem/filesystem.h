/*
 * filesystem.h
 *
 * Created: 11/1/2021 10:27:29 PM
 *  Author: mkentrru
 */ 


#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "../kernel.h"
#include "../uart.h"

#include "fs_configuration.h"
#include "fs_types.h"
#include "memory.h"
#include "fs_scripts.h"

int init_filesystem ();

exit_status fs__check__cluster_chain ();

void fs__ssi__update ();
void fs__ssi__switch ();


extern const char command_prompt [];

void put_prompt ();
void locking_output__prompt ();

#endif /* FILESYSTEM_H_ */