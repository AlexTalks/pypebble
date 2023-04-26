#pragma once

#include <cstdint>
#include <string>

namespace cockroachdb::pebble {

namespace base {
template <typename T>
using duo = std::pair<T, T>;
}  // namespace base

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
