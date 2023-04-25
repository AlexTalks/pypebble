package main

import (
	"fmt"
	"runtime/cgo"
	"unsafe"
)

/*
#include "libpebble-common.h"
*/
import "C"

var liveCGoHandles = 0

type HandleWrapper struct {
	cgo.Handle
}

func CGoHandle(ptr C.uintptr_t) HandleWrapper {
	return HandleWrapper{cgo.Handle(ptr)}
}

func NewCGoHandle(v any) HandleWrapper {
	defer func() {
		liveCGoHandles++
	}()
	return HandleWrapper{cgo.NewHandle(v)}
}

func (h HandleWrapper) Value() any {
	return h.Handle.Value()
}

func (h HandleWrapper) Delete() {
	defer func() {
		liveCGoHandles--
	}()
	h.Handle.Delete()
}

//export LiveCGoHandles
func LiveCGoHandles() int {
	return liveCGoHandles
}

//export ReleaseHandle
func ReleaseHandle(ptr C.uintptr_t) {
	CGoHandle(ptr).Delete()
}

var dbg_global_str_ptr *string

//export MakeString
func MakeString() string {
	var sptr *string
	if dbg_global_str_ptr != nil {
		sptr = dbg_global_str_ptr
	} else {
		s := "hello from go!"
		sptr = &s
		dbg_global_str_ptr = sptr
	}

	ptr := unsafe.Pointer(C._GoStringPtr(*sptr))
	l := C._GoStringLen(*sptr)
	fmt.Printf("(dbg) (CGo ) addr: %p, len: %d\n", ptr, int(l))
	return *sptr
}
