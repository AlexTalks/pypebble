package main

import (
	"fmt"
	"unsafe"

	"github.com/cockroachdb/pebble"
)

/*
#include "libpebble-common.h"
*/
import "C"

//export PebbleOpen
func PebbleOpen(name *C.cchar_t, optsPtr C.uintptr_t) C.handle_and_error_t {
	opts := CGoHandle(optsPtr).Value().(*pebble.Options)
	if !opts.ReadOnly {
		pebble.DefaultLogger.Infof("WARNING: opening database in read-write mode. " +
			"This may cause corruption to an existing database.")
	}
	db, err := pebble.Open(C.GoString(name), opts)
	if err != nil {
		return C.handle_and_error_t{err_msg: C.CString(err.Error())}
	}
	return C.handle_and_error_t{handle: C.uintptr_t(NewCGoHandle(db).Handle)}
}

//export PebbleClose
func PebbleClose(dbPtr C.uintptr_t) *C.cchar_t {
	dbHandle := CGoHandle(dbPtr)
	db := dbHandle.Value().(*pebble.DB)
	err := db.Close()
	if err != nil {
		return C.CString(err.Error())
	}
	defer dbHandle.Delete()
	return nil
}

//export PebbleGet
func PebbleGet(dbPtr C.uintptr_t, keyBytes unsafe.Pointer, keyLen C.int) C.bytes_and_error_t {
	db := CGoHandle(dbPtr).Value().(*pebble.DB)
	key := C.GoBytes(keyBytes, keyLen)
	val, closer, err := db.Get(key)
	if err != nil {
		return C.bytes_and_error_t{bytes: C.bytes_t{}, err_msg: C.CString(err.Error())}
	}
	valLen := C.int64_t(len(val))
	valBytes := C.CBytes(val)
	/*
		std::cout << "(dbg) (Get ) "
		            << "addr: " << std::hex << getResult.bytes.val << ", "
		            << "len: " << std::dec << getResult.bytes.len << ", "
		            << "val: " << std::string((char*)getResult.bytes.val, getResult.bytes.len) << std::endl;
	*/
	fmt.Printf("(dbg) (CGo ) addr: %p, len: %d\n", valBytes, valLen)
	defer closer.Close()
	return C.bytes_and_error_t{bytes: C.bytes_t{val: valBytes, len: valLen}, err_msg: nil}
}

// TODO(sarkesian): Support EngineKey encoding/decoding
// TODO(sarkesian): Support Reader interface:
// 	i.e. iterators, inc. IterOptions (default/Cockroach)
// TODO(sarkesian): Support Writer interface:
// 	i.e. Apply(batch), LogData, RangeKeySet/Unset, RangeKeyDelete

//export PebbleSet
func PebbleSet(
	dbPtr C.uintptr_t,
	keyBytes unsafe.Pointer,
	keyLen C.int,
	valBytes unsafe.Pointer,
	valLen C.int,
	sync bool,
) *C.cchar_t {
	db := CGoHandle(dbPtr).Value().(*pebble.DB)
	key := C.GoBytes(keyBytes, keyLen)
	val := C.GoBytes(valBytes, valLen)
	var opts *pebble.WriteOptions
	if sync {
		opts = pebble.Sync
	} else {
		opts = pebble.NoSync
	}
	err := db.Set(key, val, opts)
	if err != nil {
		return C.CString(err.Error())
	}
	return nil
}

//export PebbleDelete
func PebbleDelete(dbPtr C.uintptr_t, keyBytes unsafe.Pointer, keyLen C.int, sync bool) *C.cchar_t {
	db := CGoHandle(dbPtr).Value().(*pebble.DB)
	key := C.GoBytes(keyBytes, keyLen)
	var opts *pebble.WriteOptions
	if sync {
		opts = pebble.Sync
	} else {
		opts = pebble.NoSync
	}
	err := db.Delete(key, opts)
	if err != nil {
		return C.CString(err.Error())
	}
	return nil
}

//export PebbleSingleDelete
func PebbleSingleDelete(
	dbPtr C.uintptr_t, keyBytes unsafe.Pointer, keyLen C.int, sync bool,
) *C.cchar_t {
	db := CGoHandle(dbPtr).Value().(*pebble.DB)
	key := C.GoBytes(keyBytes, keyLen)
	var opts *pebble.WriteOptions
	if sync {
		opts = pebble.Sync
	} else {
		opts = pebble.NoSync
	}
	err := db.SingleDelete(key, opts)
	if err != nil {
		return C.CString(err.Error())
	}
	return nil
}

//export PebbleDeleteRange
func PebbleDeleteRange(
	dbPtr C.uintptr_t,
	startKeyBytes unsafe.Pointer,
	startKeyLen C.int,
	endKeyBytes unsafe.Pointer,
	endKeyLen C.int,
	sync bool,
) *C.cchar_t {
	db := CGoHandle(dbPtr).Value().(*pebble.DB)
	startKey := C.GoBytes(startKeyBytes, startKeyLen)
	endKey := C.GoBytes(endKeyBytes, endKeyLen)
	var opts *pebble.WriteOptions
	if sync {
		opts = pebble.Sync
	} else {
		opts = pebble.NoSync
	}
	err := db.DeleteRange(startKey, endKey, opts)
	if err != nil {
		return C.CString(err.Error())
	}
	return nil
}

//export PebbleMerge
func PebbleMerge(
	dbPtr C.uintptr_t,
	keyBytes unsafe.Pointer,
	keyLen C.int,
	valBytes unsafe.Pointer,
	valLen C.int,
	sync bool,
) *C.cchar_t {
	db := CGoHandle(dbPtr).Value().(*pebble.DB)
	key := C.GoBytes(keyBytes, keyLen)
	val := C.GoBytes(valBytes, valLen)
	var opts *pebble.WriteOptions
	if sync {
		opts = pebble.Sync
	} else {
		opts = pebble.NoSync
	}
	err := db.Merge(key, val, opts)
	if err != nil {
		return C.CString(err.Error())
	}
	return nil
}

//export PebbleNewIter
func PebbleNewIter(dbPtr C.uintptr_t, iterOptionsPtr C.uintptr_t) C.uintptr_t {
	db := CGoHandle(dbPtr).Value().(*pebble.DB)
	iterOptions := CGoHandle(iterOptionsPtr).Value().(*pebble.IterOptions)
	iter := db.NewIter(iterOptions)
	return C.uintptr_t(NewCGoHandle(iter).Handle)
}

//export PebbleNumFiles
func PebbleNumFiles(dbPtr C.uintptr_t) int64 {
	db := CGoHandle(dbPtr).Value().(*pebble.DB)
	return db.Metrics().Total().NumFiles
}
