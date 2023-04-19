package main

import (
	"time"

	cockroachStorage "github.com/cockroachdb/cockroach/pkg/storage"
	"github.com/cockroachdb/pebble"
	"github.com/cockroachdb/pebble/bloom"
	"github.com/cockroachdb/pebble/vfs"
)

/*
#include <stdint.h> // for uintptr_t
*/
import "C"

//export PebbleBasicOptions
func PebbleBasicOptions() C.uintptr_t {
	opts := &pebble.Options{}
	return C.uintptr_t(NewCGoHandle(opts).Handle)
}

//export PebbleCockroachDefaultOptions
func PebbleCockroachDefaultOptions(readWrite bool) C.uintptr_t {
	opts := cockroachStorage.DefaultPebbleOptions()
	opts.ReadOnly = !readWrite
	opts.Experimental.KeyValidationFunc = nil
	return C.uintptr_t(NewCGoHandle(opts).Handle)
}

//export PebbleOptions
func PebbleOptions(
	readWrite bool,
	useCockroachInterfaces bool,
	l0CompactionThreshold int,
	l0StopWritesThreshold int,
	lBaseMaxBytes int64,
	levels int,
	maxConcurrentCompactions int,
	memTableSize int,
	memTableStopWritesThreshold int,
	blockSize int,
	indexBlockSize int,
) C.uintptr_t {
	comparer := pebble.DefaultComparer
	merger := pebble.DefaultMerger
	if useCockroachInterfaces {
		comparer = cockroachStorage.EngineComparer
		merger = cockroachStorage.MVCCMerger
	}
	opts := &pebble.Options{
		ReadOnly:              !readWrite,
		Comparer:              comparer,
		FS:                    vfs.Default,
		L0CompactionThreshold: l0CompactionThreshold,
		L0StopWritesThreshold: l0StopWritesThreshold,
		LBaseMaxBytes:         lBaseMaxBytes,
		Levels:                make([]pebble.LevelOptions, levels),
		MaxConcurrentCompactions: func() int {
			return maxConcurrentCompactions
		},
		MemTableSize:                memTableSize,
		MemTableStopWritesThreshold: memTableStopWritesThreshold,
		Merger:                      merger,
		// NB: Could be made configurable.
		FlushDelayDeleteRange: 10 * time.Second,
		FlushDelayRangeKey:    10 * time.Second,
	}
	if useCockroachInterfaces {
		opts.BlockPropertyCollectors = cockroachStorage.PebbleBlockPropertyCollectors
		// NB: Experimental options are not currently supported.
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
	return C.uintptr_t(NewCGoHandle(opts).Handle)
}
