#ifndef LIBPEBBLE_COMMON_H
#define LIBPEBBLE_COMMON_H

#include <stdint.h> // for uintptr_t
#include <stdbool.h>

typedef const char cchar_t;

typedef struct {
	uintptr_t handle;
	const char* errMsg;
} handle_and_error_t;

typedef struct {
	void* val;
	int64_t len;
} bytes_t;

typedef struct {
	void* val;
	int64_t len;
	const char* errMsg;
} bytes_and_error_t;

#endif