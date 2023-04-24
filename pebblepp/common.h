#pragma once

#include <cstdint>

namespace cockroachdb {
namespace pebble {

class CGoHandle {
 public:
  CGoHandle(uintptr_t new_handle);
  CGoHandle(const CGoHandle&) = delete;
  void operator=(const CGoHandle&) = delete;

  virtual ~CGoHandle();

  friend class DB;

 protected:
  uintptr_t handle_;
};
}  // namespace pebble
}  // namespace cockroachdb
