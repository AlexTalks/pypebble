package main

import (
	"reflect"
	"runtime/cgo"
)

/*
#include "cgo-common.h"
*/
import "C"

var liveCGoHandles = 0

func toCBytes(value []byte) C.bytes_t {
	if value == nil {
		return C.bytes_t{}
	}
	return C.bytes_t{val: C.CBytes(value), len: C.int64_t(len(value))}
}

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

func (h HandleWrapper) TypeName() string {
	val := h.Handle.Value()
	valType := reflect.TypeOf(val)
	return valType.String()
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

//export HandleTypeName
func HandleTypeName(ptr C.uintptr_t) *C.cchar_t {
	return C.CString(CGoHandle(ptr).TypeName())
}

//export ReleaseHandle
func ReleaseHandle(ptr C.uintptr_t) {
	CGoHandle(ptr).Delete()
}
