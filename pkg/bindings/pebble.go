package main

import (
	"runtime/cgo"
	"time"
	"unsafe"

	"github.com/cockroachdb/pebble"
	"github.com/cockroachdb/pebble/bloom"
	"github.com/cockroachdb/pebble/vfs"
)

/*
#include <stdint.h> // for uintptr_t

typedef const char cchar_t;

typedef struct {
	uintptr_t handle;
	const char* errMsg;
} PebbleOpenResult;
*/
import "C"

//export ReleaseHandle
func ReleaseHandle(ptr C.uintptr_t) {
	cgo.Handle(ptr).Delete()
}

//export PebbleBasicOptions
func PebbleBasicOptions() C.uintptr_t {
	opts := &pebble.Options{}
	return C.uintptr_t(cgo.NewHandle(opts))
}

//export PebbleCockroachAdjacentOptions
func PebbleCockroachAdjacentOptions() C.uintptr_t {
	return PebbleOptions(
		2,
		1000,
		64<<20, // 64 MB
		7,
		1,
		64<<20, // 64 MB
		4,
		uint64(pebble.FormatPrePebblev1Marked),
		32<<10,  // 32 KB
		256<<10, // 256 KB
	)
}

//export PebbleOptions
func PebbleOptions(
	l0CompactionThreshold int,
	l0StopWritesThreshold int,
	lBaseMaxBytes int64,
	levels int,
	maxConcurrentCompactions int,
	memTableSize int,
	memTableStopWritesThreshold int,
	formatVersion uint64,
	blockSize int,
	indexBlockSize int,
) C.uintptr_t {
	opts := &pebble.Options{
		L0CompactionThreshold: l0CompactionThreshold,
		L0StopWritesThreshold: l0StopWritesThreshold,
		LBaseMaxBytes:         lBaseMaxBytes,
		Levels:                make([]pebble.LevelOptions, levels),
		MaxConcurrentCompactions: func() int {
			return maxConcurrentCompactions
		},
		MemTableSize:                memTableSize,
		MemTableStopWritesThreshold: memTableStopWritesThreshold,
		FormatMajorVersion:          pebble.FormatMajorVersion(formatVersion),
		// NB: Could support, but not currently.
		FlushDelayDeleteRange: 10 * time.Second,
		FlushDelayRangeKey:    10 * time.Second,
		// NB: Changing the below options not supported.
		// DB may not open if created under different options.
		Comparer: pebble.DefaultComparer,
		FS:       vfs.Default,
		Merger:   pebble.DefaultMerger,
	}
	for i := 0; i < len(opts.Levels); i++ {
		l := &opts.Levels[i]
		l.BlockSize = blockSize
		l.IndexBlockSize = indexBlockSize
		l.FilterPolicy = bloom.FilterPolicy(10)
		l.FilterType = pebble.TableFilter
		if i > 0 {
			l.TargetFileSize = opts.Levels[i-1].TargetFileSize * 2
		}
		l.EnsureDefaults()
	}
	return C.uintptr_t(cgo.NewHandle(opts))
}

//export PebbleOpen
func PebbleOpen(name *C.cchar_t, optsPtr C.uintptr_t) C.PebbleOpenResult {
	opts := cgo.Handle(optsPtr).Value().(*pebble.Options)
	db, err := pebble.Open(C.GoString(name), opts)
	if err != nil {
		return C.PebbleOpenResult{errMsg: C.CString(err.Error())}
	}
	return C.PebbleOpenResult{handle: C.uintptr_t(cgo.NewHandle(db))}
}

//export PebbleClose
func PebbleClose(dbPtr C.uintptr_t) *C.cchar_t {
	dbHandle := cgo.Handle(dbPtr)
	db := dbHandle.Value().(*pebble.DB)
	err := db.Close()
	if err != nil {
		return C.CString(err.Error())
	}
	defer dbHandle.Delete()
	return nil
}

//export PebbleSet
func PebbleSet(
	dbPtr C.uintptr_t,
	keyBytes unsafe.Pointer, keyLen C.int,
	valBytes unsafe.Pointer, valLen C.int,
	sync bool,
) *C.cchar_t {
	db := cgo.Handle(dbPtr).Value().(*pebble.DB)
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

//export PebbleGet
func PebbleGet(
	dbPtr C.uintptr_t,
	keyBytes unsafe.Pointer, keyLen C.int,
) (unsafe.Pointer, int, *C.cchar_t) {
	db := cgo.Handle(dbPtr).Value().(*pebble.DB)
	key := C.GoBytes(keyBytes, keyLen)
	val, closer, err := db.Get(key)
	if err != nil {
		return nil, 0, C.CString(err.Error())
	}
	valLen := len(val)
	valBytes := C.CBytes(val)
	defer closer.Close()
	return valBytes, valLen, nil
}

//export PebbleNumFiles
func PebbleNumFiles(dbPtr C.uintptr_t) int64 {
	db := cgo.Handle(dbPtr).Value().(*pebble.DB)
	return db.Metrics().Total().NumFiles
}
