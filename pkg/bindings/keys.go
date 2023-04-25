package main

import (
	"unsafe"

	cockroachKeys "github.com/cockroachdb/cockroach/pkg/keys"
	cockroachKeysUtil "github.com/cockroachdb/cockroach/pkg/util/keysutil"
)

/*
#include "libpebble-common.h"
*/
import "C"

//export PrettyPrintKey
func PrettyPrintKey(keyBytes unsafe.Pointer, keyLen C.int) *C.cchar_t {
	key := C.GoBytes(keyBytes, keyLen)
	return C.CString(cockroachKeys.PrettyPrint(nil, key))
}

//export PrettyScanKey
func PrettyScanKey(keyCStr *C.cchar_t) C.bytes_and_error_t {
	keyStr := C.GoString(keyCStr)
	scanner := cockroachKeysUtil.MakePrettyScanner(nil /* tableParser */)
	key, err := scanner.Scan(keyStr)
	if err != nil {
		return C.bytes_and_error_t{bytes: C.bytes_t{}, err_msg: C.CString(err.Error())}
	}
	valLen := C.int64_t(len(key))
	valBytes := C.CBytes(key)
	return C.bytes_and_error_t{bytes: C.bytes_t{val: valBytes, len: valLen}, err_msg: nil}
}
