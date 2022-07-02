/*
 * fs_configuration.h
 *
 * Created: 11/1/2021 9:24:38 PM
 *  Author: mkentrru
 */ 


#ifndef FS_CONFIGURATION_H_
#define FS_CONFIGURATION_H_


#define K_SIZE 0x20
#define K_COUNT 0x20
#define MAX_OBJ_SIZE (K_SIZE * (K_COUNT - 1))

typedef uint8_t KID;

#define INTRO_KID	0
#define ROOT_KID	1
#define ktoa(a) (a * K_SIZE)

#define KP_END 0x20
#define KP_REM 0x40
#define KP_NAN 0x00

#define FS_CONST 0x05

#define obj_type__file	1
#define obj_type__dir	0


typedef uint32_t memsync_control;

typedef byte_t passwd_t;
typedef byte_t kaddr_t;

#endif /* FS_CONFIGURATION_H_ */