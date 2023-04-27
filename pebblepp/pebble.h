#pragma once

#include <cstdint>
#include <string>

#include "pebblepp/common.h"
#include "pebblepp/iterator.h"
#include "pebblepp/options.h"

namespace cockroachdb::pebble {

int CGoHandles();

std::string PrettyPrintKey(const std::string& key);
std::string PrettyScanKey(const std::string& human_key);

std::string_view StringFromGo();

class DB : public CGoHandle {
 public:
  static DB* Open(const std::string& name);
  static DB* Open(const std::string& name, const Options* options);

  int64_t NumFiles();
  void Close();

  // No copying
  DB(const DB&) = delete;
  void operator=(const DB&) = delete;

  virtual ~DB();

  std::string Get(const std::string& key);
  void Set(const std::string& key, const std::string& val, bool sync = true);
  void Delete(const std::string& key, bool sync = true);
  void SingleDelete(const std::string& key, bool sync = true);
  void DeleteRange(const std::string& start_key, const std::string& end_key, bool sync = true);
  void Merge(const std::string& key, const std::string& val, bool sync = true);

  Iterator* NewIter(IterOptions& opts);

 private:
  bool closed_;

  DB(uintptr_t new_handle);
  void checkValid();
};

}  // namespace cockroachdb::pebble
