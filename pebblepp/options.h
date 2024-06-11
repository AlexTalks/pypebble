#pragma once

#include "pebblepp/common.h"

namespace cockroachdb::pebble {

typedef CGoHandle Options;

Options* BasicOptions(bool read_write = false);
Options* PebbleOptions(bool read_write, int l0_compaction_threshold,
                       int l0_stop_writes_threshold, int64_t lbase_max_bytes, int levels,
                       int max_concurrent_compactions, uint64_t mem_table_size,
                       int mem_table_stop_writes_threshold, int block_size, int index_block_size);

#ifdef COCKROACH_SUPPORT
Options* CockroachDefaultOptions(bool read_write = false);
Options* CockroachCustomOptions(bool read_write, int l0_compaction_threshold,
                       int l0_stop_writes_threshold, int64_t lbase_max_bytes, int levels,
                       int max_concurrent_compactions, uint64_t mem_table_size,
                       int mem_table_stop_writes_threshold, int block_size, int index_block_size);
#endif

}  // namespace cockroachdb::pebble
