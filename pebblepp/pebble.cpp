#include "pebblepp/pebble.h"

#include "libpebble.h"

namespace cockroachdb::pebble {

int CGoHandles() { return LiveCGoHandles(); }

std::string PrettyPrintKey(const std::string& key) {
  return ::PrettyPrintKey((char*)key.data(), key.length());
}

std::string PrettyScanKey(const std::string& human_key) {
  bytes_and_error_t result = ::PrettyScanKey(human_key.c_str());
  if (result.err_msg) {
    throw std::runtime_error(result.err_msg);
  }
  auto key = std::string((char*)result.bytes.val, result.bytes.len);
  free(result.bytes.val);
  return key;
}

DB::DB(uintptr_t new_handle) : CGoHandle(new_handle), closed_(false) {}
DB::~DB() {}

DB* DB::Open(const std::string& path, bool read_write) {
  Options basicOptions(PebbleBasicOptions(read_write));
  return DB::Open(path, &basicOptions);
}

DB* DB::Open(const std::string& path, const Options* options) {
  handle_and_error_t result = PebbleOpen(path.c_str(), options->handle_);
  if (result.err_msg) {
    throw std::runtime_error(result.err_msg);
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
}

std::string DB::Get(const std::string& key) {
  checkValid();
  bytes_and_error_t get_result = PebbleGet(handle_, (void*)key.data(), key.length());
  if (get_result.err_msg) {
    throw std::runtime_error(get_result.err_msg);
  }

  auto val = std::string((char*)get_result.bytes.val, get_result.bytes.len);
  free(get_result.bytes.val);
  return val;
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

Iterator* DB::NewIter() {
  checkValid();
  handle_and_error_t result = PebbleNewIter(handle_, IterOptions().handle_);
  if (result.err_msg) {
    throw std::runtime_error(result.err_msg);
  }

  return new Iterator(result.handle);
}

Iterator* DB::NewIter(IterOptions& opts) {
  checkValid();
  handle_and_error_t result = PebbleNewIter(handle_, opts.handle_);
  if (result.err_msg) {
    throw std::runtime_error(result.err_msg);
  }

  return new Iterator(result.handle);
}

}  // namespace cockroachdb::pebble
