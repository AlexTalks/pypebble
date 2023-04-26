#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pebblepp/iterator.h"
#include "pebblepp/options.h"
#include "pebblepp/pebble.h"
namespace py = pybind11;

#define DOC_READ_ONLY "Read-only by default."
#define DOC_SYNC "By default, syncs the write to disk."
#define DOC_ITER_VALID "Returns true if pointing at a valid key, false otherwise."
#define DOC_ITER_LIMIT "If limit is provided, it serves as a best-effort exclusive limit."

namespace cockroachdb::pebble {

template <typename Copyable>
const Copyable CopyObject(const Copyable& v) {
  return v;
}

template <class T>
T* NoInit() {
  throw std::runtime_error("This class cannot be instantiated from Python");
  return (T*)nullptr;
}

typedef std::pair<py::bytes, py::bytes> PyRangeKeyData;

PYBIND11_MODULE(pypebble, m) {
  // -- Module-level Functions and Utilities --
  m.def("cgo_handles", &CGoHandles,
        "Number of live handles to Go-managed memory available to C/Python.");
  m.def("pretty_key", &PrettyPrintKey, "Pretty-print a CockroachDB key.");
  m.def(
      "scan_key",
      [](const std::string& human_key) {
        return py::bytes(PrettyScanKey(human_key));
      },
      "Scan a human-readable CockroachDB key.");

  py::class_<Object>(m, "_Object").def_property_readonly("go_type", &Object::GoType);

  // -- pebble.Options and utils --
  py::class_<Options, Object>(
      m, "Options",
      "Optional parameters for a Pebble DB.")
      .def(py::init(&NoInit<Options>));
  m.def("basic_options", &BasicOptions,
        "Default Pebble Options.\n"
        DOC_READ_ONLY,
        py::arg("read_write") = false);
  m.def("cockroach_options", &CockroachDefaultOptions,
        "Default options for CockroachDB Pebble instances.\n"
        DOC_READ_ONLY,
        py::arg("read_write") = false);
  m.def("pebble_options", &PebbleOptions,
        "Customizable Pebble Options.",
        py::arg("write"),
        py::arg("use_cockroach_interfaces"),
        py::arg("l0_compaction_threshold"),
        py::arg("l0_stop_writes_threshold"),
        py::arg("lbase_max_bytes"),
        py::arg("levels"),
        py::arg("max_concurrent_compactions"),
        py::arg("mem_table_size"),
        py::arg("mem_table_stop_writes_threshold"),
        py::arg("block_size"),
        py::arg("index_block_size"));

  // -- pebble.Iterator and utils --
  py::enum_<IterKeyType>(m, "IterKeyType")
      .value("POINTS_ONLY", kPointsOnly)
      .value("RANGES_ONLY", kRangesOnly)
      .value("POINTS_AND_RANGES", kPointsAndRanges);

  py::class_<IterOptions, Object>(
      m, "IterOptions",
      "Optional per-iterator parameters.")
      .def(py::init())
      .def(py::init<const IterOptions&>())
      .def("__copy__", &CopyObject<IterOptions>)
      .def_property("lower_bound", &IterOptions::LowerBound, &IterOptions::SetLowerBound)
      .def_property("upper_bound", &IterOptions::UpperBound, &IterOptions::SetUpperBound)
      .def_property("key_types", &IterOptions::KeyTypes, &IterOptions::SetKeyTypes);

  py::enum_<IterValidityState>(m, "IterValidityState")
      .value("EXHAUSTED", kExhausted)
      .value("VALID", kValid)
      .value("AT_LIMIT", kAtLimit);

  py::class_<Iterator, Object>(
      m, "Iterator",
      "An iterator for scanning over a DB's key/value pairs in key order.\n"
      "Must be closed after use.")
      .def(py::init(&NoInit<Iterator>))
      .def(
          "clone",
          &Iterator::Clone,
          py::arg("refresh_batch_view") = false,
          py::arg("opts") = nullptr,
          "Create a new unpositioned Iterator over the same underlying data.\n"
          "Accepts None for opts, in which case the pre-set IterOptions is used.")
      .def(
          "seek_ge",
          [](Iterator& self, py::bytes key) {
            return self.SeekGE(key);
          },
          "Move the iterator to the first key greater than or equal to the given key.\n"
          DOC_ITER_VALID,
          py::arg("key"))
      .def(
          "seek_ge_with_limit",
          [](Iterator& self, py::bytes key, py::bytes limit) {
            return self.SeekGEWithLimit(key, limit);
          },
          "Move the iterator to the first key greater than or equal to the given key.\n"
          DOC_ITER_LIMIT "\n"
          DOC_ITER_VALID,
          py::arg("key"),
          py::arg("limit"))
      .def(
          "seek_prefix_ge",
          [](Iterator& self, py::bytes key) {
            return self.SeekPrefixGE(key);
          },
          "Move the iterator to the first key greater than or equal to the given key, "
          "and which has the same prefix as the given key.\n"
          DOC_ITER_VALID,
          py::arg("key"))
      .def(
          "seek_lt",
          [](Iterator& self, py::bytes key) {
            return self.SeekLT(key);
          },
          "Move the iterator to the last key less than the given key.\n"
          DOC_ITER_VALID,
          py::arg("key"))
      .def(
          "seek_lt_with_limit",
          [](Iterator& self, py::bytes key, py::bytes limit) {
            return self.SeekLTWithLimit(key, limit);
          },
          "Move the iterator to the last key less than the given key.\n"
          DOC_ITER_LIMIT "\n"
          DOC_ITER_VALID,
          py::arg("key"),
          py::arg("limit"))
      .def(
          "first",
          &Iterator::First,
          "Move the iterator to the first key.\n"
          DOC_ITER_VALID)
      .def(
          "last",
          &Iterator::Last,
          "Move the iterator to the last key.\n"
          DOC_ITER_VALID)
      .def(
          "next",
          &Iterator::Next,
          "Move the iterator to the next key.\n"
          DOC_ITER_VALID)
      .def(
          "next_with_limit",
          [](Iterator& self, py::bytes limit) {
            return self.NextWithLimit(limit);
          },
          "Move the iterator to the next key.\n"
          DOC_ITER_LIMIT "\n"
          DOC_ITER_VALID,
          py::arg("limit"))
      .def(
          "prev",
          &Iterator::Prev,
          "Move the iterator to the previous key.\n"
          DOC_ITER_VALID)
      .def(
          "prev_with_limit",
          [](Iterator& self, py::bytes limit) {
            return self.PrevWithLimit(limit);
          },
          "Move the iterator to the previous key.\n"
          DOC_ITER_LIMIT "\n"
          DOC_ITER_VALID,
          py::arg("limit"))
      .def(
          "range_key_changed",
          &Iterator::RangeKeyChanged,
          "Indicates whether the most recent positioning operation encountered a range key.\n"
          "If true, previously returned range key bounds and data have been invalidated.")
      .def(
          "has_point_and_range",
          &Iterator::HasPointAndRange,
          "Indicates whether there exists a point key, a range key or"
          "both at the current iterator position.")
      .def(
          "range_bounds",
          [](Iterator& self) {
            return std::pair<py::bytes, py::bytes>(self.RangeBounds());
          },
          "Returns the start (inclusive) and end (exclusive) bounds of the"
          "range key covering the current iterator position.")
      .def(
          "key",
          [](Iterator& self) {
            return py::bytes(self.Key());
          },
          "The key at the current iterator position.")
      .def(
          "value",
          [](Iterator& self) {
            return py::bytes(self.Value());
          },
          "The value for the key at the current iterator position.")
      .def(
          "range_keys",
          [](Iterator& self) {
            py::list py_range_keys;
            std::vector<RangeKeyData> range_keys(self.RangeKeys());

            auto convert = [&py_range_keys](const RangeKeyData& data) {
              py_range_keys.append(std::make_pair(
                  py::bytes(data.suffix), py::bytes(data.value)
                ));
            };

            std::for_each(range_keys.cbegin(), range_keys.cend(), convert);
            return py_range_keys;
          },
          "Returns the list of range keys (suffix, key value) covering the "
          "current iterator position.\n"
          "The range bounds may be retrieved separately through Iterator.range_bounds().")
      // TODO(sarkesian): Potentially remove, leaving conversion to separate functions,
      // Python decorators, or Python-level settings.
      .def(
          "pretty_key",
          &Iterator::PrettyKey,
          "The key at the current iterator position, "
          "interpreted as a CockroachDB key and pretty-printed.")
      .def(
          "valid",
          &Iterator::Valid,
          "Returns true if positioned at a valid key, false otherwise.")
      .def(
          "error",
          &Iterator::Error,
          "Returns any accumulated error encountered by the iterator.")
      .def("close", &Iterator::Close, "Close the iterator.")
      .def(
          "set_bounds",
          [](Iterator& self, py::bytes lower, py::bytes upper) {
            self.SetBounds(lower, upper);
          },
          "Set the lower and upper bounds for the iterator.\n"
          "Invalidates the iterator.",
          py::arg("lower"),
          py::arg("upper"))
      .def(
          "set_options",
          &Iterator::SetOptions,
          "Change options for the iterator.\n"
          "Invalidates the iterator.",
          py::arg("opts"))
      .def("read_amp", &Iterator::ReadAmp, "The read amplification experienced by this iterator.")
      .def("reset_stats", &Iterator::ResetStats, "Reset all stats to 0.")
      .def(
          "stats",
          [](Iterator& self) {
            IteratorStats stats(self.Stats());
            auto call_stats_as_dict = [](IteratorCallStats& call_stats) {
              py::dict call_stats_dict;
              call_stats_dict["forward_seeks"] = std::get<kFwdSeekCount>(call_stats);
              call_stats_dict["reverse_seeks"] = std::get<kRevSeekCount>(call_stats);
              call_stats_dict["forward_steps"] = std::get<kFwdStepCount>(call_stats);
              call_stats_dict["reverse_steps"] = std::get<kRevStepCount>(call_stats);
              return call_stats_dict;
            };

            py::dict py_stats;
            py_stats["interface_stats"] = call_stats_as_dict(std::get<kInterfaceStats>(stats));
            py_stats["internal_stats"] = call_stats_as_dict(std::get<kInternalStats>(stats));
            return py_stats;
          },
          "The iterator's current stats.");

  // -- pebble.DB --
  py::class_<DB, Object>(
      m, "DB",
      "A concurrent, persistent ordered key/value Pebble store.")
      .def(py::init(&NoInit<DB>))
      .def(
          "close",
          &DB::Close,
          "Close a open Pebble database.\n"
          "It is not safe to close a DB until all outstanding iterators are closed.")
      .def(
          "num_files",
          &DB::NumFiles,
          "Number of files used by this Pebble DB.")
      .def(
          "get",
          [](DB& self, py::bytes key) {
            return py::bytes(self.Get(key));
          },
          "Get the value for a given key.",
          py::arg("key"))
      .def(
          "set",
          [](DB& self, py::bytes key, py::bytes val, bool sync) {
            self.Set(key, val, sync);
          },
          "Set the value for a given key.\n"
          DOC_SYNC,
          py::arg("key"),
          py::arg("val"),
          py::arg("sync") = true)
      .def(
          "delete",
          [](DB& self, py::bytes key, bool sync) {
            self.Delete(key, sync);
          },
          "Deletes the value for a given key.\n"
          DOC_SYNC,
          py::arg("key"),
          py::arg("sync") = true)
      .def(
          "single_delete",
          [](DB& self, py::bytes key, bool sync) {
            self.SingleDelete(key, sync);
          },
          "Add an action to the batch that single deletes the entry for a key.\n"
          DOC_SYNC,
          py::arg("key"),
          py::arg("sync") = true)
      .def(
          "delete_range",
          [](DB& self, py::bytes start_key, py::bytes end_key, bool sync) {
            self.DeleteRange(start_key, end_key, sync);
          },
          "Deletes all keys/values in the range [start,end).\n"
          DOC_SYNC,
          py::arg("start_key"),
          py::arg("end_key"),
          py::arg("sync") = true)
      .def(
          "merge",
          [](DB& self, py::bytes key, py::bytes val, bool sync) {
            self.Merge(key, val, sync);
          },
          "Merge the value at a given key with the new value.\n" DOC_SYNC,
          py::arg("key"),
          py::arg("val"),
          py::arg("sync") = true)
      .def(
          "new_iter",
          [](DB& self, IterOptions& opts) {
            return self.NewIter(opts);
          },
          "Create a new, unpositioned pypebble.Iterator.",
          py::arg("opts"))
      .def(
          "new_iter",
          [](DB& self) {
            return self.NewIter();
          },
          "Create a new, unpositioned pypebble.Iterator.");

  // -- pebble.Open(..) --
  m.def("open",
      [](const std::string& path, bool read_write) {
        return DB::Open(path, read_write);
      },
      "Open a Pebble DB.", py::arg("path"), py::arg("read_write") = false);
  m.def("open",
      [](const std::string& path, const Options* opts) {
        return DB::Open(path, opts);
      },
      "Open a Pebble DB.", py::arg("path"), py::arg("options"));
}

}  // namespace cockroachdb::pebble
