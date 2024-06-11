#ifndef CGO_COMMON_H
#define CGO_COMMON_H

#include <stdint.h> // for uintptr_t
#include <stdbool.h>

typedef const char cchar_t;

typedef struct {
	uintptr_t handle;
	cchar_t* err_msg;
} handle_and_error_t;

typedef struct {
	void* val;
	int64_t len;
} bytes_t;

typedef struct {
	bytes_t bytes;
	cchar_t* err_msg;
} bytes_and_error_t;

#endif
