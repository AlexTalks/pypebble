#include "pebblepp/pebble.h"
#include "libpebble.h"

namespace cockroachdb {
namespace pebble {

int CGoHandles() {
  return LiveCGoHandles();
}

DB::DB(uintptr_t new_handle)
: handle_(new_handle), closed_(false) {}

DB::~DB() {
  if(handle_) {
    ReleaseHandle(handle_);
  }
}

DB* DB::Open(const std::string& name) {
  Options basicOptions(PebbleBasicOptions(true));
  return DB::Open(name, &basicOptions);
}

DB* DB::Open(const std::string& name, const Options* options) {
  PebbleOpenResult result = PebbleOpen(name.c_str(), options->handle_);
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

std::string DB::Get(const std::string &key) {
  checkValid();
  PebbleGet_return getResult = PebbleGet(handle_, (void*)key.c_str(), key.length());
  if(getResult.r2) {
    throw std::runtime_error(getResult.r2);
  }

  return std::string((char*)getResult.r0, getResult.r1);
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

void DB::Delete(const std::string& key, bool sync) {
  checkValid();
  const char* err = PebbleDelete(handle_,
                                 (void*)key.c_str(), key.length(),
                                 sync);
  if(err) {
    throw std::runtime_error(err);
  }
}

void DB::SingleDelete(const std::string& key, bool sync) {
  checkValid();
  const char* err = PebbleSingleDelete(handle_,
                                 (void*)key.c_str(), key.length(),
                                 sync);
  if(err) {
    throw std::runtime_error(err);
  }
}

void DB::DeleteRange(const std::string& start_key, const std::string& end_key, bool sync) {
  checkValid();
  const char* err = PebbleDeleteRange(handle_,
                                 (void*)start_key.c_str(), start_key.length(),
                                 (void*)end_key.c_str(), end_key.length(),
                                 sync);
  if(err) {
    throw std::runtime_error(err);
  }
}

void DB::Merge(const std::string& key, const std::string& val, bool sync) {
  checkValid();
  const char* err = PebbleMerge(handle_,
                              (void*)key.c_str(), key.length(),
                              (void*)val.c_str(), val.length(),
                              sync);
  if(err) {
    throw std::runtime_error(err);
  }
}

} // namespace pebble
} // namespace cockroachdb