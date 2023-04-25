#include "pebblepp/common.h"

#include "libpebble.h"

namespace cockroachdb::pebble {

CGoHandle::CGoHandle(uintptr_t new_handle) : handle_(new_handle) {}

CGoHandle::~CGoHandle() {
  if (handle_) {
    ReleaseHandle(handle_);
  }
}

}  // namespace cockroachdb::pebble
