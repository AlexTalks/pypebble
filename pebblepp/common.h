#pragma once

#include <cstdint>

namespace cockroachdb::pebble {

class CGoHandle {
 public:
  CGoHandle(uintptr_t new_handle);
  CGoHandle(const CGoHandle&) = delete;
  void operator=(const CGoHandle&) = delete;

  virtual ~CGoHandle();

  friend class DB;

  virtual uintptr_t dbg_handle() const { return handle_; }

 protected:
  uintptr_t handle_;
};
}  // namespace cockroachdb::pebble
