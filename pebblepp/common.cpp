#include "pebblepp/common.h"

#include <utility>

#include "libpebble.h"

namespace cockroachdb::pebble {

const std::string kUnknownGoType("Go type unknown or not wrapped.");

std::string Object::GoType() { return kUnknownGoType; }

CGoHandle::CGoHandle(uintptr_t new_handle) : handle_(new_handle) {}

CGoHandle::~CGoHandle() {
  if (handle_) {
    ReleaseHandle(handle_);
  }
}

std::string CGoHandle::GoType() {
  if (!handle_) {
    return Object::GoType();
  }

  const char* name = HandleTypeName(handle_);
  std::string go_type(name);
  free((void*)name);

  return go_type;
}

}  // namespace cockroachdb::pebble
