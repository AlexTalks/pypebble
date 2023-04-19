#include "pebblepp/options.h"
#include "libpebble.h"

namespace cockroachdb {
namespace pebble {

CGoHandle::~CGoHandle() {
  if(handle_) {
    ReleaseHandle(handle_);
  }
}

CGoHandle::CGoHandle(uintptr_t new_handle)
: handle_(new_handle) {}

Options* BasicOptions() {
  return new Options(PebbleBasicOptions());
}

Options* CockroachDefaultOptions(bool read_write) {
  return new Options(PebbleCockroachDefaultOptions(read_write));
}

Options* PebbleOptions(
    bool read_write,
    bool use_cockroach_interfaces,
    int l0_compaction_threshold,
    int l0_stop_writes_threshold,
    int64_t l_base_max_bytes,
    int levels,
    int max_concurrent_compactions,
    int mem_table_size,
    int mem_table_stop_writes_threshold,
    int block_size,
    int index_block_size) {
  return new Options(::PebbleOptions(
      read_write,
      use_cockroach_interfaces,
      l0_compaction_threshold,
      l0_stop_writes_threshold,
      l_base_max_bytes,
      levels,
      max_concurrent_compactions,
      mem_table_size,
      mem_table_stop_writes_threshold,
      block_size,
      index_block_size));
}

} // namespace pebble
} // namespace cockroachdb
