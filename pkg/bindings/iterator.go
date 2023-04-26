package main

import (
	"unsafe"

	"github.com/cockroachdb/pebble"
)

/*
#include "libpebble-common.h"
#include <stdlib.h>	// for malloc

typedef enum {
	ITER_KEY_TYPE_POINTS_ONLY = 0,
	ITER_KEY_TYPE_RANGES_ONLY = 1,
	ITER_KEY_TYPE_POINTS_AND_RANGES = 2
} iter_key_type_t;

typedef enum {
	ITER_EXHAUSTED = 0,
	ITER_VALID = 1,
	ITER_AT_LIMIT = 2
} iter_validity_state_t;

typedef struct {
	bool has_point;
	bool has_range;
} iter_has_point_and_range_t;

typedef struct {
	bytes_t start;
	bytes_t end;
} bounds_t;

typedef struct {
	bytes_t suffix;
	bytes_t value;
} range_key_data_t;

typedef struct {
	range_key_data_t* elems;
	int64_t len;
} range_key_data_vector_t;

typedef struct {
	int64_t forward_seek_count;
	int64_t reverse_seek_count;
	int64_t forward_step_count;
	int64_t reverse_step_count;
} iterator_stats_counts_t;

typedef struct {
	// Represents calls through the interface.
	iterator_stats_counts_t interface_stats;
	// Represents calls to underlying iterator.
	iterator_stats_counts_t internal_stats;
} iterator_stats_t;

*/
import "C"

//export PebbleNewIterOptions
func PebbleNewIterOptions() C.uintptr_t {
	opts := &pebble.IterOptions{}
	return C.uintptr_t(NewCGoHandle(opts).Handle)
}

//export PebbleIterOptionsClone
func PebbleIterOptionsClone(optsPtr C.uintptr_t) C.uintptr_t {
	newOpts := &pebble.IterOptions{}
	oldOpts := CGoHandle(optsPtr).Value().(*pebble.IterOptions)
	if oldOpts != nil {
		*newOpts = *oldOpts
	}
	return C.uintptr_t(NewCGoHandle(newOpts).Handle)
}

//export PebbleIterOptionsGetBound
func PebbleIterOptionsGetBound(optsPtr C.uintptr_t, upper bool) C.bytes_t {
	opts := CGoHandle(optsPtr).Value().(*pebble.IterOptions)
	var bound []byte
	if upper {
		bound = opts.UpperBound
	} else {
		bound = opts.LowerBound
	}

	return toCBytes(bound)
}

//export PebbleIterOptionsSetBound
func PebbleIterOptionsSetBound(
	optsPtr C.uintptr_t, upper bool, boundPtr unsafe.Pointer, boundLength C.int,
) {
	opts := CGoHandle(optsPtr).Value().(*pebble.IterOptions)
	bound := C.GoBytes(boundPtr, boundLength)
	if upper {
		opts.UpperBound = bound
	} else {
		opts.LowerBound = bound
	}
}

//export PebbleIterOptionsGetKeyTypes
func PebbleIterOptionsGetKeyTypes(optsPtr C.uintptr_t) C.iter_key_type_t {
	opts := CGoHandle(optsPtr).Value().(*pebble.IterOptions)
	return C.iter_key_type_t(opts.KeyTypes)
}

//export PebbleIterOptionsSetKeyTypes
func PebbleIterOptionsSetKeyTypes(optsPtr C.uintptr_t, keyType C.iter_key_type_t) {
	opts := CGoHandle(optsPtr).Value().(*pebble.IterOptions)
	opts.KeyTypes = pebble.IterKeyType(keyType)
}

//export PebbleIterClone
func PebbleIterClone(
	iterPtr C.uintptr_t, optsPtr C.uintptr_t, refreshBatchView bool,
) C.handle_and_error_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	var opts *pebble.IterOptions
	if optsPtr != 0 {
		opts = CGoHandle(optsPtr).Value().(*pebble.IterOptions)
	}
	cloned, err := iter.Clone(pebble.CloneOptions{
		IterOptions:      opts,
		RefreshBatchView: refreshBatchView,
	})

	if err != nil {
		return C.handle_and_error_t{err_msg: C.CString(err.Error())}
	}
	return C.handle_and_error_t{handle: C.uintptr_t(NewCGoHandle(cloned).Handle)}
}

//export PebbleIterSeekGE
func PebbleIterSeekGE(iterPtr C.uintptr_t, keyPtr unsafe.Pointer, keyLength C.int) bool {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	key := C.GoBytes(keyPtr, keyLength)
	return iter.SeekGE(key)
}

//export PebbleIterSeekGEWithLimit
func PebbleIterSeekGEWithLimit(
	iterPtr C.uintptr_t,
	keyPtr unsafe.Pointer,
	keyLength C.int,
	limitPtr unsafe.Pointer,
	limitLength C.int,
) C.iter_validity_state_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	key := C.GoBytes(keyPtr, keyLength)
	limit := C.GoBytes(limitPtr, limitLength)
	return C.iter_validity_state_t(iter.SeekGEWithLimit(key, limit))
}

//export PebbleIterSeekPrefixGE
func PebbleIterSeekPrefixGE(iterPtr C.uintptr_t, keyPtr unsafe.Pointer, keyLength C.int) bool {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	key := C.GoBytes(keyPtr, keyLength)
	return iter.SeekPrefixGE(key)
}

//export PebbleIterSeekLT
func PebbleIterSeekLT(iterPtr C.uintptr_t, keyPtr unsafe.Pointer, keyLength C.int) bool {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	key := C.GoBytes(keyPtr, keyLength)
	return iter.SeekLT(key)
}

//export PebbleIterSeekLTWithLimit
func PebbleIterSeekLTWithLimit(
	iterPtr C.uintptr_t,
	keyPtr unsafe.Pointer,
	keyLength C.int,
	limitPtr unsafe.Pointer,
	limitLength C.int,
) C.iter_validity_state_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	key := C.GoBytes(keyPtr, keyLength)
	limit := C.GoBytes(limitPtr, limitLength)
	return C.iter_validity_state_t(iter.SeekLTWithLimit(key, limit))
}

//export PebbleIterFirst
func PebbleIterFirst(iterPtr C.uintptr_t) bool {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	return iter.First()
}

//export PebbleIterLast
func PebbleIterLast(iterPtr C.uintptr_t) bool {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	return iter.Last()
}

//export PebbleIterNext
func PebbleIterNext(iterPtr C.uintptr_t) bool {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	return iter.Next()
}

//export PebbleIterNextWithLimit
func PebbleIterNextWithLimit(
	iterPtr C.uintptr_t, limitPtr unsafe.Pointer, limitLength C.int,
) C.iter_validity_state_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	limit := C.GoBytes(limitPtr, limitLength)
	return C.iter_validity_state_t(iter.NextWithLimit(limit))
}

//export PebbleIterPrev
func PebbleIterPrev(iterPtr C.uintptr_t) bool {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	return iter.Prev()
}

//export PebbleIterPrevWithLimit
func PebbleIterPrevWithLimit(
	iterPtr C.uintptr_t, limitPtr unsafe.Pointer, limitLength C.int,
) C.iter_validity_state_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	limit := C.GoBytes(limitPtr, limitLength)
	return C.iter_validity_state_t(iter.PrevWithLimit(limit))
}

//export PebbleIterRangeKeyChanged
func PebbleIterRangeKeyChanged(iterPtr C.uintptr_t) bool {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	return iter.RangeKeyChanged()
}

//export PebbleIterHasPointAndRange
func PebbleIterHasPointAndRange(iterPtr C.uintptr_t) C.iter_has_point_and_range_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	p, r := iter.HasPointAndRange()
	return C.iter_has_point_and_range_t{has_point: C.bool(p), has_range: C.bool(r)}
}

//export PebbleIterRangeBounds
func PebbleIterRangeBounds(iterPtr C.uintptr_t) C.bounds_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	start, end := iter.RangeBounds()
	return C.bounds_t{
		start: toCBytes(start),
		end:   toCBytes(end),
	}
}

//export PebbleIterKey
func PebbleIterKey(iterPtr C.uintptr_t) C.bytes_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	key := iter.Key()
	return toCBytes(key)
}

//export PebbleIterValue
func PebbleIterValue(iterPtr C.uintptr_t) C.bytes_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	val := iter.Value()
	return toCBytes(val)
}

//export PebbleIterRangeKeys
func PebbleIterRangeKeys(iterPtr C.uintptr_t) C.range_key_data_vector_t {
	pebble.DefaultLogger.Infof("WARNING: RangeKeys() not yet implemented.")
	return C.range_key_data_vector_t{}
}

//export PebbleIterValid
func PebbleIterValid(iterPtr C.uintptr_t) bool {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	return iter.Valid()
}

//export PebbleIterError
func PebbleIterError(iterPtr C.uintptr_t) *C.cchar_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	err := iter.Error()
	if err != nil {
		return C.CString(err.Error())
	}
	return nil
}

//export PebbleIterClose
func PebbleIterClose(iterPtr C.uintptr_t) *C.cchar_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	err := iter.Close()
	if err != nil {
		return C.CString(err.Error())
	}
	return nil
}

//export PebbleIterSetBounds
func PebbleIterSetBounds(
	iterPtr C.uintptr_t,
	lowerBoundPtr unsafe.Pointer,
	lowerBoundLength C.int,
	upperBoundPtr unsafe.Pointer,
	upperBoundLength C.int,
) {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	lowerBound := C.GoBytes(lowerBoundPtr, lowerBoundLength)
	upperBound := C.GoBytes(upperBoundPtr, upperBoundLength)
	iter.SetBounds(lowerBound, upperBound)
}

//export PebbleIterSetOptions
func PebbleIterSetOptions(iterPtr C.uintptr_t, optsPtr C.uintptr_t) {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	opts := CGoHandle(optsPtr).Value().(*pebble.IterOptions)
	iter.SetOptions(opts)
}

//export PebbleIterReadAmp
func PebbleIterReadAmp(iterPtr C.uintptr_t) int {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	return iter.Metrics().ReadAmp
}

//export PebbleIterResetStats
func PebbleIterResetStats(iterPtr C.uintptr_t) {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	iter.ResetStats()
}

//export PebbleIterStats
func PebbleIterStats(iterPtr C.uintptr_t) C.iterator_stats_t {
	iter := CGoHandle(iterPtr).Value().(*pebble.Iterator)
	stats := iter.Stats()
	// TODO(sarkesian): Expose internal stats, range key stats.
	return C.iterator_stats_t{
		interface_stats: C.iterator_stats_counts_t{
			forward_seek_count: C.int64_t(stats.ForwardSeekCount[pebble.InterfaceCall]),
			reverse_seek_count: C.int64_t(stats.ReverseSeekCount[pebble.InterfaceCall]),
			forward_step_count: C.int64_t(stats.ForwardStepCount[pebble.InterfaceCall]),
			reverse_step_count: C.int64_t(stats.ReverseStepCount[pebble.InterfaceCall]),
		},
		internal_stats: C.iterator_stats_counts_t{
			forward_seek_count: C.int64_t(stats.ForwardSeekCount[pebble.InternalIterCall]),
			reverse_seek_count: C.int64_t(stats.ReverseSeekCount[pebble.InternalIterCall]),
			forward_step_count: C.int64_t(stats.ForwardStepCount[pebble.InternalIterCall]),
			reverse_step_count: C.int64_t(stats.ReverseStepCount[pebble.InternalIterCall]),
		},
	}
}
