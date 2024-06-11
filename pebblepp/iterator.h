#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "pebblepp/common.h"

namespace cockroachdb::pebble {

enum IterKeyType { kPointsOnly = 0, kRangesOnly = 1, kPointsAndRanges = 2 };
enum IterValidityState { kExhausted = 0, kValid = 1, kAtLimit = 2 };

class IterOptions : public CGoHandle {
 public:
  IterOptions();
  IterOptions(const IterOptions&);
  IterOptions& operator=(const IterOptions&);

  virtual ~IterOptions();

  std::string LowerBound() const;
  void SetLowerBound(const std::string& bound);

  std::string UpperBound() const;
  void SetUpperBound(const std::string& bound);

  IterKeyType KeyTypes() const;
  void SetKeyTypes(IterKeyType k);

  friend class Iterator;
};

// RangeKeyData is defined as (suffix, key value).
struct RangeKeyData {
  std::string suffix;
  std::string value;
};

// IteratorCallStats is defined as counts for:
// (forward seek, reverse seek, forward step, reverse step).
typedef std::tuple<int64_t, int64_t, int64_t, int64_t> IteratorCallStats;
enum { kFwdSeekCount = 0, kRevSeekCount = 1, kFwdStepCount = 2, kRevStepCount = 3 };

// IteratorStats is defined as (interface call stats, internal call stats).
typedef base::duo<IteratorCallStats> IteratorStats;
enum { kInterfaceStats = 0, kInternalStats = 1 };

class Iterator : public CGoHandle {
 public:
  // No copying
  Iterator(const Iterator&) = delete;
  void operator=(const Iterator&) = delete;

  virtual ~Iterator();

  Iterator* Clone(bool refresh_batch_view = false, const IterOptions* opts = nullptr);

  bool SeekGE(const std::string& key);
  IterValidityState SeekGEWithLimit(const std::string& key, const std::string& limit);
  bool SeekPrefixGE(const std::string& key);

  bool SeekLT(const std::string& key);
  IterValidityState SeekLTWithLimit(const std::string& key, const std::string& limit);

  bool First();
  bool Last();
  bool Next();
  IterValidityState NextWithLimit(const std::string& limit);
  bool Prev();
  IterValidityState PrevWithLimit(const std::string& limit);

  bool RangeKeyChanged();
  base::duo<bool> HasPointAndRange();
  base::duo<std::string> RangeBounds();

  std::string Key();
  std::string Value();

  std::vector<RangeKeyData> RangeKeys();

  bool Valid();
  std::optional<std::runtime_error> Error();
  void Close();

  void SetBounds(const std::string& lower, const std::string& upper);
  void SetOptions(const IterOptions& options);

  int ReadAmp();
  void ResetStats();
  IteratorStats Stats();

  friend class DB;

 private:
  Iterator(uintptr_t new_handle);
};

}  // namespace cockroachdb::pebble
