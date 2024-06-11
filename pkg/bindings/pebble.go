package main

import (
	"os"
	"path/filepath"
	"unsafe"

	"github.com/cockroachdb/pebble"
)

/*
#include "libpebble-common.h"
*/
import "C"

//export PebbleOpen
func PebbleOpen(dirName *C.cchar_t, optsPtr C.uintptr_t) C.handle_and_error_t {
	opts := CGoHandle(optsPtr).Value().(*pebble.Options)
	if !opts.ReadOnly {
		pebble.DefaultLogger.Infof("WARNING: opening database in read-write mode. " +
			"This may cause corruption to an existing database.")
	}
	inputPath := C.GoString(dirName)
	if !filepath.IsAbs(inputPath) {
		pebble.DefaultLogger.Infof("WARNING: path should be absolute, but \"%s\" was given", inputPath)
		cwd, err := os.Getwd()
		if err != nil {
			return C.handle_and_error_t{err_msg: C.CString(err.Error())}
		}
		inputPath = filepath.Join(cwd, inputPath)
	}

	db, err := pebble.Open(inputPath, opts)
	if err != nil {
		return C.handle_and_error_t{err_msg: C.CString(err.Error())}
	}
	mode := "read"
	if !opts.ReadOnly {
		mode = "read-write"
	}
	pebble.DefaultLogger.Infof("opened database in %s mode at \"%s\"", mode, inputPath)
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
	defer closer.Close()
	return C.bytes_and_error_t{bytes: toCBytes(val), err_msg: nil}
}

// TODO(sarkesian): Support EngineKey encoding/decoding
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
func PebbleNewIter(dbPtr C.uintptr_t, iterOptionsPtr C.uintptr_t) C.handle_and_error_t {
	db := CGoHandle(dbPtr).Value().(*pebble.DB)
	iterOptions := CGoHandle(iterOptionsPtr).Value().(*pebble.IterOptions)
	iter, err := db.NewIter(iterOptions)
	if err != nil {
		return C.handle_and_error_t{err_msg: C.CString(err.Error())}
	}
	return C.handle_and_error_t{handle: C.uintptr_t(NewCGoHandle(iter).Handle)}
}

//export PebbleNumFiles
func PebbleNumFiles(dbPtr C.uintptr_t) int64 {
	db := CGoHandle(dbPtr).Value().(*pebble.DB)
	return db.Metrics().Total().NumFiles
}
