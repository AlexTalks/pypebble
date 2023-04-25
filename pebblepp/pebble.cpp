#include "pebblepp/pebble.h"

#include <iostream>

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
  return std::string((char*)result.bytes.val, result.bytes.len);
}

std::string_view StringFromGo() {
  GoString s = MakeString();
  std::cout << "(dbg) (Cpp ) "
            << "addr: " << std::hex << (void*)(s.p) << ", "
            << "len: " << std::dec << s.n << std::endl;
  return std::string_view(s.p, s.n);
}

DB::DB(uintptr_t new_handle) : CGoHandle(new_handle), closed_(false) {}
DB::~DB() {}

DB* DB::Open(const std::string& name) {
  Options basicOptions(PebbleBasicOptions(true));
  return DB::Open(name, &basicOptions);
}

DB* DB::Open(const std::string& name, const Options* options) {
  handle_and_error_t result = PebbleOpen(name.c_str(), options->handle_);
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
  handle_ = 0;
}

std::string DB::Get(const std::string& key) {
  checkValid();
  bytes_and_error_t getResult = PebbleGet(handle_, (void*)key.data(), key.length());
  if (getResult.err_msg) {
    throw std::runtime_error(getResult.err_msg);
  }

  // TODO(sarkesian): use strings, but free any memory returned using C.CBytes in Go.
  // Also, return
  std::cout << "(dbg) (Get ) "
            << "addr: " << std::hex << getResult.bytes.val << ", "
            << "len: " << std::dec << getResult.bytes.len << ", "
            << "val: " << std::string((char*)getResult.bytes.val, getResult.bytes.len) << std::endl;

  std::string ret((char*)getResult.bytes.val, getResult.bytes.len);
  //free(getResult.bytes.val);

  return ret;
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

}  // namespace cockroachdb::pebble
