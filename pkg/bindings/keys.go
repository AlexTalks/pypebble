package main

import (
	"unsafe"

	cockroachKeys "github.com/cockroachdb/cockroach/pkg/keys"
)

/*
#include "libpebble-common.h"
*/
import "C"

//export PrettyKey
func PrettyKey(keyBytes unsafe.Pointer, keyLen C.int) *C.cchar_t {
	key := C.GoBytes(keyBytes, keyLen)
	return C.CString(cockroachKeys.PrettyPrint(nil, key))
}
