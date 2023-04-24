#include "pebblepp/iterator.h"

#include "libpebble.h"

namespace cockroachdb {
namespace pebble {

IterOptions::IterOptions() : CGoHandle(PebbleNewIterOptions()) {}
IterOptions::~IterOptions() {}

IterOptions::IterOptions(const IterOptions& other)
    : CGoHandle(PebbleIterOptionsClone(other.handle_)) {}

IterOptions& IterOptions::operator=(const IterOptions& other) {
  IterOptions* existing = this;
  if (existing != &other) {
    ReleaseHandle(existing->handle_);
    this->handle_ = PebbleIterOptionsClone(other.handle_);
  }
  return *this;
}

std::string IterOptions::LowerBound() const {
  bytes_t bound = PebbleIterOptionsGetBound(handle_, false);
  return std::string((char*)bound.val, bound.len);
}

void IterOptions::SetLowerBound(const std::string& bound) {
  PebbleIterOptionsSetBound(handle_, false, (void*)bound.data(), bound.length());
}

std::string IterOptions::UpperBound() const {
  bytes_t bound = PebbleIterOptionsGetBound(handle_, true);
  return std::string((char*)bound.val, bound.len);
}

void IterOptions::SetUpperBound(const std::string& bound) {
  PebbleIterOptionsSetBound(handle_, true, (void*)bound.data(), bound.length());
}

IterKeyType IterOptions::KeyTypes() const {
  return IterKeyType(PebbleIterOptionsGetKeyTypes(handle_));
}

void IterOptions::SetKeyTypes(IterKeyType k) {
  PebbleIterOptionsSetKeyTypes(handle_, iter_key_type_t(k));
}

Iterator::Iterator(uintptr_t new_handle) : CGoHandle(new_handle) {}
Iterator::~Iterator() {}

bool Iterator::SeekGE(const std::string& key) {
  return PebbleIterSeekGE(handle_, (void*)key.data(), key.length());
}

IterValidityState Iterator::SeekGEWithLimit(const std::string& key, const std::string& limit) {
  return IterValidityState(PebbleIterSeekGEWithLimit(handle_, (void*)key.data(), key.length(),
                                                     (void*)limit.data(), limit.length()));
}

bool Iterator::SeekPrefixGE(const std::string& key) {
  return PebbleIterSeekPrefixGE(handle_, (void*)key.data(), key.length());
}

bool Iterator::SeekLT(const std::string& key) {
  return PebbleIterSeekLT(handle_, (void*)key.data(), key.length());
}

IterValidityState Iterator::SeekLTWithLimit(const std::string& key, const std::string& limit) {
  return IterValidityState(PebbleIterSeekLTWithLimit(handle_, (void*)key.data(), key.length(),
                                                     (void*)limit.data(), limit.length()));
}

bool Iterator::First() { return PebbleIterFirst(handle_); }

bool Iterator::Last() { return PebbleIterLast(handle_); }

bool Iterator::Next() { return PebbleIterNext(handle_); }

IterValidityState Iterator::NextWithLimit(const std::string& limit) {
  return IterValidityState(PebbleIterNextWithLimit(handle_, (void*)limit.data(), limit.length()));
}

bool Iterator::Prev() { return PebbleIterPrev(handle_); }

IterValidityState Iterator::PrevWithLimit(const std::string& limit) {
  return IterValidityState(PebbleIterPrevWithLimit(handle_, (void*)limit.data(), limit.length()));
}

bool Iterator::RangeKeyChanged() { return PebbleIterRangeKeyChanged(handle_); }

std::pair<bool, bool> Iterator::HasPointAndRange() {
  iter_has_point_and_range_t ret = PebbleIterHasPointAndRange(handle_);
  return std::pair(ret.hasPoint, ret.hasRange);
}

std::pair<std::string, std::string> Iterator::RangeBounds() {
  bounds_t bounds = PebbleIterRangeBounds(handle_);
  std::string start((char*)bounds.startVal, bounds.startLen);
  std::string end((char*)bounds.endVal, bounds.endLen);
  return std::pair(start, end);
}

std::string Iterator::Key() {
  bytes_t ret = PebbleIterKey(handle_);
  return std::string((char*)ret.val, ret.len);
}

std::string Iterator::PrettyKey() {
  bytes_t ret = PebbleIterKey(handle_);
  return std::string(::PrettyKey(ret.val, ret.len));
}

std::string Iterator::Value() {
  bytes_t ret = PebbleIterValue(handle_);
  return std::string((char*)ret.val, ret.len);
}

bool Iterator::Valid() { return PebbleIterValid(handle_); }

std::optional<std::runtime_error> Iterator::Error() {
  const char* err = PebbleIterError(handle_);
  if (err) {
    return std::runtime_error(err);
  }
  return {};
}

void Iterator::Close() {
  const char* err = PebbleIterClose(handle_);
  if (err) {
    throw std::runtime_error(err);
  }
}

void Iterator::SetBounds(const std::string& lower, const std::string& upper) {
  PebbleIterSetBounds(handle_, (void*)lower.data(), lower.length(), (void*)upper.data(),
                      upper.length());
}

void Iterator::SetOptions(const IterOptions& opts) { PebbleIterSetOptions(handle_, opts.handle_); }

int Iterator::ReadAmp() { return PebbleIterReadAmp(handle_); }

void Iterator::ResetStats() { PebbleIterResetStats(handle_); }

}  // namespace pebble
}  // namespace cockroachdb
