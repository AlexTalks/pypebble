#include "pebblepp/iterator.h"

#include "libpebble.h"

namespace cockroachdb::pebble {

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
  bytes_t ret = PebbleIterOptionsGetBound(handle_, false);
  auto bound = std::string((char*)ret.val, ret.len);
  free(ret.val);
  return bound;
}

void IterOptions::SetLowerBound(const std::string& bound) {
  PebbleIterOptionsSetBound(handle_, false, (void*)bound.data(), bound.length());
}

std::string IterOptions::UpperBound() const {
  bytes_t ret = PebbleIterOptionsGetBound(handle_, true);
  auto bound = std::string((char*)ret.val, ret.len);
  free(ret.val);
  return bound;
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

Iterator* Iterator::Clone(bool refresh_batch_view, const IterOptions* opts) {
  uintptr_t opts_handle(0);
  if (opts) {
    opts_handle = opts->handle_;
  }
  handle_and_error_t result = PebbleIterClone(handle_, opts_handle, refresh_batch_view);
  if (result.err_msg) {
    throw std::runtime_error(result.err_msg);
  }

  return new Iterator(result.handle);
}

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
  return std::pair(ret.has_point, ret.has_range);
}

std::pair<std::string, std::string> Iterator::RangeBounds() {
  bounds_t bounds = PebbleIterRangeBounds(handle_);
  std::string start((char*)bounds.start.val, bounds.start.len);
  std::string end((char*)bounds.end.val, bounds.end.len);
  free(bounds.start.val);
  free(bounds.end.val);
  return std::pair(start, end);
}

std::string Iterator::Key() {
  bytes_t ret = PebbleIterKey(handle_);
  auto key = std::string((char*)ret.val, ret.len);
  free(ret.val);
  return key;
}

std::string Iterator::Value() {
  bytes_t ret = PebbleIterValue(handle_);
  auto val = std::string((char*)ret.val, ret.len);
  free(ret.val);
  return val;
}

std::vector<RangeKeyData> Iterator::RangeKeys() {
    range_key_data_vector_t src_data = PebbleIterRangeKeys(handle_);
    std::vector<RangeKeyData> range_keys;
    range_keys.reserve(src_data.len);
    for (int i = 0; i < src_data.len; i++) {
      range_key_data_t elem = src_data.elems[i];
      RangeKeyData range_key{
          std::string((char*)elem.suffix.val, elem.suffix.len),
          std::string((char*)elem.value.val, elem.value.len)
      };
      free(elem.suffix.val);
      free(elem.value.val);
      range_keys.push_back(range_key);
    }

    return range_keys;
}

std::string Iterator::PrettyKey() {
  bytes_t ret = PebbleIterKey(handle_);
  std::string pretty_key(::PrettyPrintKey(ret.val, ret.len));
  free(ret.val);
  return pretty_key;
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

IteratorStats Iterator::Stats() {
  iterator_stats_t src_stats = PebbleIterStats(handle_);
  IteratorStats stats;

  auto set_stats = [](IteratorCallStats& dst, iterator_stats_counts_t src) {
    dst = {
      src.forward_seek_count,
      src.reverse_seek_count,
      src.forward_step_count,
      src.reverse_step_count
    };
  };

  set_stats(stats.first, src_stats.interface_stats);
  set_stats(stats.second, src_stats.internal_stats);

  return stats;
}

}  // namespace cockroachdb::pebble
