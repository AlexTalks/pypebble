#include "pebblepp/options.h"

#include "libpebble.h"

namespace cockroachdb::pebble {

Options* BasicOptions(bool read_write) { return new Options(PebbleBasicOptions(read_write)); }

Options* CockroachDefaultOptions(bool read_write) {
  return new Options(PebbleCockroachDefaultOptions(read_write));
}

Options* PebbleOptions(bool read_write, bool use_cockroach_interfaces, int l0_compaction_threshold,
                       int l0_stop_writes_threshold, int64_t lbase_max_bytes, int levels,
                       int max_concurrent_compactions, uint64_t mem_table_size,
                       int mem_table_stop_writes_threshold, int block_size, int index_block_size) {
  return new Options(::PebbleOptions(
      read_write, use_cockroach_interfaces, l0_compaction_threshold, l0_stop_writes_threshold,
      lbase_max_bytes, levels, max_concurrent_compactions, mem_table_size,
      mem_table_stop_writes_threshold, block_size, index_block_size));
}

}  // namespace cockroachdb::pebble
