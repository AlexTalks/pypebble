#include "pebblepp/common.h"

#include "libpebble.h"

namespace cockroachdb {
namespace pebble {

CGoHandle::CGoHandle(uintptr_t new_handle) : handle_(new_handle) {}

CGoHandle::~CGoHandle() {
  if (handle_) {
    ReleaseHandle(handle_);
  }
}

}  // namespace pebble
}  // namespace cockroachdb
