#include "pebblepp/pebble.h"
#include "libpebble.h"

// REMOVE
//#include "./pebble.h"
//#include "../libpebble.h"

namespace cockroachdb {
namespace pebble {

DB::DB(std::string name) {
  PebbleOpenResult result = PebbleOpen(name.c_str(), PebbleBasicOptions());
  if(result.errMsg) {
    throw std::runtime_error(result.errMsg);
  }
  handle_ = result.handle;
}

DB::DB(uintptr_t new_handle)
: handle_(new_handle), closed_(false) {}

DB* DB::Open(const std::string& name) {
  PebbleOpenResult result = PebbleOpen(name.c_str(), PebbleBasicOptions());
  if(result.errMsg) {
    throw std::runtime_error(result.errMsg);
  }

  return new DB(result.handle);
}

void DB::checkValid() {
  if(closed_) {
    throw std::runtime_error("db closed");
  }
  if(!handle_) {
    throw std::runtime_error("invalid ptr to go memory");
  }
}

int64_t DB::NumFiles() {
  checkValid();

  return PebbleNumFiles(handle_);
}

void DB::Close() {
  checkValid();
  const char* err = PebbleClose(handle_);
  if(err) {
    throw std::runtime_error(err);
  }

  closed_ = true;
  handle_ = 0;
}

void DB::Set(const std::string& key, const std::string& val, bool sync) {
  checkValid();
  const char* err = PebbleSet(handle_,
                              (void*)key.c_str(), key.length(),
                              (void*)val.c_str(), val.length(),
                              sync);
  if(err) {
    throw std::runtime_error(err);
  }
}

std::string DB::Get(const std::string &key) {
  checkValid();
  PebbleGet_return getResult = PebbleGet(handle_, (void*)key.c_str(), key.length());
  if(getResult.r2) {
    throw std::runtime_error(getResult.r2);
  }

  return std::string((char*)getResult.r0, getResult.r1);
}

} // namespace pebble
} // namespace cockroachdb
