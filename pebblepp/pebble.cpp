#include "pebblepp/pebble.h"

#include "libpebble.h"

namespace cockroachdb {
namespace pebble {

int CGoHandles() { return LiveCGoHandles(); }

std::string PrettyPrintKey(const std::string& key) {
  return std::string(PrettyKey((void*)key.data(), key.length()));
}

DB::DB(uintptr_t new_handle) : handle_(new_handle), closed_(false) {}

DB::~DB() {
  if (handle_) {
    ReleaseHandle(handle_);
  }
}

DB* DB::Open(const std::string& name) {
  Options basicOptions(PebbleBasicOptions(true));
  return DB::Open(name, &basicOptions);
}

DB* DB::Open(const std::string& name, const Options* options) {
  handle_and_error_t result = PebbleOpen(name.c_str(), options->handle_);
  if (result.errMsg) {
    throw std::runtime_error(result.errMsg);
  }

  return new DB(result.handle);
}

void DB::checkValid() {
  if (closed_) {
    throw std::runtime_error("db closed");
  }
  if (!handle_) {
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
  if (err) {
    throw std::runtime_error(err);
  }

  closed_ = true;
  handle_ = 0;
}

std::string DB::Get(const std::string& key) {
  checkValid();
  bytes_and_error_t getResult = PebbleGet(handle_, (void*)key.data(), key.length());
  if (getResult.errMsg) {
    throw std::runtime_error(getResult.errMsg);
  }

  return std::string((char*)getResult.bytes.val, getResult.bytes.len);
}

void DB::Set(const std::string& key, const std::string& val, bool sync) {
  checkValid();
  const char* err =
      PebbleSet(handle_, (void*)key.data(), key.length(), (void*)val.data(), val.length(), sync);
  if (err) {
    throw std::runtime_error(err);
  }
}

void DB::Delete(const std::string& key, bool sync) {
  checkValid();
  const char* err = PebbleDelete(handle_, (void*)key.data(), key.length(), sync);
  if (err) {
    throw std::runtime_error(err);
  }
}

void DB::SingleDelete(const std::string& key, bool sync) {
  checkValid();
  const char* err = PebbleSingleDelete(handle_, (void*)key.data(), key.length(), sync);
  if (err) {
    throw std::runtime_error(err);
  }
}

void DB::DeleteRange(const std::string& start_key, const std::string& end_key, bool sync) {
  checkValid();
  const char* err = PebbleDeleteRange(handle_, (void*)start_key.data(), start_key.length(),
                                      (void*)end_key.data(), end_key.length(), sync);
  if (err) {
    throw std::runtime_error(err);
  }
}

void DB::Merge(const std::string& key, const std::string& val, bool sync) {
  checkValid();
  const char* err =
      PebbleMerge(handle_, (void*)key.data(), key.length(), (void*)val.data(), val.length(), sync);
  if (err) {
    throw std::runtime_error(err);
  }
}

Iterator* DB::NewIter(IterOptions& opts) {
  checkValid();
  return new Iterator(PebbleNewIter(handle_, opts.handle_));
}

}  // namespace pebble
}  // namespace cockroachdb
