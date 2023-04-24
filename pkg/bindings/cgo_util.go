package main

import "runtime/cgo"

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
