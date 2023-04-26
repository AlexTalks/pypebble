#pragma once

#include <cstdint>
#include <string>

namespace cockroachdb::pebble {

struct Object {
  virtual ~Object() = default;
  virtual std::string GoType();
};

class CGoHandle : public Object {
 public:
  CGoHandle(uintptr_t new_handle);
  CGoHandle(const CGoHandle&) = delete;
  void operator=(const CGoHandle&) = delete;
  virtual ~CGoHandle();

  virtual std::string GoType();

  friend class DB;

 protected:
  uintptr_t handle_;
};
}  // namespace cockroachdb::pebble
