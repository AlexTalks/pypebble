#pragma once

#include <cstdint>

namespace cockroachdb {
namespace pebble {

class CGoHandle {
 public:
  CGoHandle(uintptr_t new_handle);
  CGoHandle(const CGoHandle&) = delete;
  void operator=(const CGoHandle&) = delete;

  virtual ~CGoHandle();

  friend class DB;

 private:
  uintptr_t handle_;
};

typedef CGoHandle Options;

Options* BasicOptions(bool read_write = false);
Options* CockroachDefaultOptions(bool read_write = false);
Options* PebbleOptions(bool read_write, bool use_cockroach_interfaces, int l0_compaction_threshold,
                       int l0_stop_writes_threshold, int64_t lbase_max_bytes, int levels,
                       int max_concurrent_compactions, int mem_table_size,
                       int mem_table_stop_writes_threshold, int block_size, int index_block_size);

}  // namespace pebble
}  // namespace cockroachdb
