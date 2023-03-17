#pragma once

#include <cstdint>
#include <string>

namespace cockroachdb {
namespace pebble {

class DB {
 public:
  DB(std::string name);
  static DB* Open(const std::string& name);
  int64_t NumFiles();
  void Close();

  void Set(const std::string& key, const std::string& val, bool sync);
  std::string Get(const std::string& key);

  // No copying
//  DB(const DB&) = delete;
//  void operator=(const DB&) = delete;

 private:
  uintptr_t handle_;
  bool closed_;

  DB(uintptr_t new_handle);
  void checkValid();
};

} // namespace pebble
} // namespace cockroachdb
