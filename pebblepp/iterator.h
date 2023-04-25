#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

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

// TODO(sarkesian): Consider conforming this to C++ std iterator paradigm.
class Iterator : public CGoHandle {
 public:
  // No copying
  Iterator(const Iterator&) = delete;
  void operator=(const Iterator&) = delete;

  // TODO(sarkesian): implement Clone(..)
  //*Iterator Clone(const IterOptions& opts, bool refreshBatchView);

  virtual ~Iterator();

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
  std::pair<bool, bool> HasPointAndRange();
  std::pair<std::string, std::string> RangeBounds();

  std::string Key();
  std::string Value();
  // TODO(sarkesian): potentially remove, use converter for keys
  std::string PrettyKey();
  // TODO(sarkesian): implement RangeKeyData[] RangeKeys()

  bool Valid();
  std::optional<std::runtime_error> Error();
  void Close();

  void SetBounds(const std::string& lower, const std::string& upper);
  void SetOptions(const IterOptions& options);

  int ReadAmp();
  void ResetStats();
  // TODO(sarkesian): implement IteratorStats Stats()

  friend class DB;

 private:
  Iterator(uintptr_t new_handle);
};

}  // namespace cockroachdb::pebble
