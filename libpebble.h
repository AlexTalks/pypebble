/* Code generated by cmd/cgo; DO NOT EDIT. */

/* package github.com/alextalks/pypebble/pkg/bindings */


#line 1 "cgo-builtin-export-prolog"

#include <stddef.h>

#ifndef GO_CGO_EXPORT_PROLOGUE_H
#define GO_CGO_EXPORT_PROLOGUE_H

#ifndef GO_CGO_GOSTRING_TYPEDEF
typedef struct { const char *p; ptrdiff_t n; } _GoString_;
#endif

#endif

/* Start of preamble from import "C" comments.  */


#line 5 "cgo_util.go"

#include "libpebble-common.h"

#line 1 "cgo-generated-wrapper"

#line 9 "iterator.go"

#include "libpebble-common.h"

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
	bool hasPoint;
	bool hasRange;
} iter_has_point_and_range_t;

typedef struct {
	void* startVal;
	int64_t startLen;
	void* endVal;
	int64_t endLen;
} bounds_t;


#line 1 "cgo-generated-wrapper"

#line 9 "keys.go"

#include "libpebble-common.h"

#line 1 "cgo-generated-wrapper"

#line 9 "pebble.go"

#include "libpebble-common.h"

#line 1 "cgo-generated-wrapper"

#line 12 "pebble_options.go"

#include <stdint.h> // for uintptr_t

#line 1 "cgo-generated-wrapper"


/* End of preamble from import "C" comments.  */


/* Start of boilerplate cgo prologue.  */
#line 1 "cgo-gcc-export-header-prolog"

#ifndef GO_CGO_PROLOGUE_H
#define GO_CGO_PROLOGUE_H

typedef signed char GoInt8;
typedef unsigned char GoUint8;
typedef short GoInt16;
typedef unsigned short GoUint16;
typedef int GoInt32;
typedef unsigned int GoUint32;
typedef long long GoInt64;
typedef unsigned long long GoUint64;
typedef GoInt64 GoInt;
typedef GoUint64 GoUint;
typedef size_t GoUintptr;
typedef float GoFloat32;
typedef double GoFloat64;
#ifdef _MSC_VER
#include <complex.h>
typedef _Fcomplex GoComplex64;
typedef _Dcomplex GoComplex128;
#else
typedef float _Complex GoComplex64;
typedef double _Complex GoComplex128;
#endif

/*
  static assertion to make sure the file is being used on architecture
  at least with matching size of GoInt.
*/
typedef char _check_for_64_bit_pointer_matching_GoInt[sizeof(void*)==64/8 ? 1:-1];

#ifndef GO_CGO_GOSTRING_TYPEDEF
typedef _GoString_ GoString;
#endif
typedef void *GoMap;
typedef void *GoChan;
typedef struct { void *t; void *v; } GoInterface;
typedef struct { void *data; GoInt len; GoInt cap; } GoSlice;

#endif

/* End of boilerplate cgo prologue.  */

#ifdef __cplusplus
extern "C" {
#endif

extern GoInt LiveCGoHandles();
extern void ReleaseHandle(uintptr_t ptr);
extern uintptr_t PebbleNewIterOptions();
extern uintptr_t PebbleIterOptionsClone(uintptr_t optsPtr);
extern bytes_t PebbleIterOptionsGetBound(uintptr_t optsPtr, GoUint8 upper);
extern void PebbleIterOptionsSetBound(uintptr_t optsPtr, GoUint8 upper, void* boundPtr, int boundLength);
extern iter_key_type_t PebbleIterOptionsGetKeyTypes(uintptr_t optsPtr);
extern void PebbleIterOptionsSetKeyTypes(uintptr_t optsPtr, iter_key_type_t keyType);
extern handle_and_error_t PebbleIterClone(uintptr_t iterPtr, uintptr_t optsPtr, GoUint8 refreshBatchView);
extern GoUint8 PebbleIterSeekGE(uintptr_t iterPtr, void* keyPtr, int keyLength);
extern iter_validity_state_t PebbleIterSeekGEWithLimit(uintptr_t iterPtr, void* keyPtr, int keyLength, void* limitPtr, int limitLength);
extern GoUint8 PebbleIterSeekPrefixGE(uintptr_t iterPtr, void* keyPtr, int keyLength);
extern GoUint8 PebbleIterSeekLT(uintptr_t iterPtr, void* keyPtr, int keyLength);
extern iter_validity_state_t PebbleIterSeekLTWithLimit(uintptr_t iterPtr, void* keyPtr, int keyLength, void* limitPtr, int limitLength);
extern GoUint8 PebbleIterFirst(uintptr_t iterPtr);
extern GoUint8 PebbleIterLast(uintptr_t iterPtr);
extern GoUint8 PebbleIterNext(uintptr_t iterPtr);
extern iter_validity_state_t PebbleIterNextWithLimit(uintptr_t iterPtr, void* limitPtr, int limitLength);
extern GoUint8 PebbleIterPrev(uintptr_t iterPtr);
extern iter_validity_state_t PebbleIterPrevWithLimit(uintptr_t iterPtr, void* limitPtr, int limitLength);
extern GoUint8 PebbleIterRangeKeyChanged(uintptr_t iterPtr);
extern iter_has_point_and_range_t PebbleIterHasPointAndRange(uintptr_t iterPtr);
extern bounds_t PebbleIterRangeBounds(uintptr_t iterPtr);
extern bytes_t PebbleIterKey(uintptr_t iterPtr);
extern bytes_t PebbleIterValue(uintptr_t iterPtr);
extern void PebbleIterRangeKeys(uintptr_t iterPtr);
extern GoUint8 PebbleIterValid(uintptr_t iterPtr);
extern cchar_t* PebbleIterError(uintptr_t iterPtr);
extern cchar_t* PebbleIterClose(uintptr_t iterPtr);
extern void PebbleIterSetBounds(uintptr_t iterPtr, void* lowerBoundPtr, int lowerBoundLength, void* upperBoundPtr, int upperBoundLength);
extern void PebbleIterSetOptions(uintptr_t iterPtr, uintptr_t optsPtr);
extern GoInt PebbleIterReadAmp(uintptr_t iterPtr);
extern void PebbleIterResetStats(uintptr_t iterPtr);
extern void PebbleIterStats(uintptr_t iterPtr);
extern cchar_t* PrettyKey(void* keyBytes, int keyLen);
extern handle_and_error_t PebbleOpen(cchar_t* name, uintptr_t optsPtr);
extern cchar_t* PebbleClose(uintptr_t dbPtr);
extern bytes_and_error_t PebbleGet(uintptr_t dbPtr, void* keyBytes, int keyLen);
extern cchar_t* PebbleSet(uintptr_t dbPtr, void* keyBytes, int keyLen, void* valBytes, int valLen, GoUint8 sync);
extern cchar_t* PebbleDelete(uintptr_t dbPtr, void* keyBytes, int keyLen, GoUint8 sync);
extern cchar_t* PebbleSingleDelete(uintptr_t dbPtr, void* keyBytes, int keyLen, GoUint8 sync);
extern cchar_t* PebbleDeleteRange(uintptr_t dbPtr, void* startKeyBytes, int startKeyLen, void* endKeyBytes, int endKeyLen, GoUint8 sync);
extern cchar_t* PebbleMerge(uintptr_t dbPtr, void* keyBytes, int keyLen, void* valBytes, int valLen, GoUint8 sync);
extern uintptr_t PebbleNewIter(uintptr_t dbPtr, uintptr_t iterOptionsPtr);
extern GoInt64 PebbleNumFiles(uintptr_t dbPtr);
extern uintptr_t PebbleBasicOptions(GoUint8 readWrite);
extern uintptr_t PebbleCockroachDefaultOptions(GoUint8 readWrite);
extern uintptr_t PebbleOptions(GoUint8 readWrite, GoUint8 useCockroachInterfaces, GoInt l0CompactionThreshold, GoInt l0StopWritesThreshold, GoInt64 lBaseMaxBytes, GoInt levels, GoInt maxConcurrentCompactions, GoInt memTableSize, GoInt memTableStopWritesThreshold, GoInt blockSize, GoInt indexBlockSize);

#ifdef __cplusplus
}
#endif
