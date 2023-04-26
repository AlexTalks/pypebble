#include <pybind11/pybind11.h>

#include "pebblepp/iterator.h"
#include "pebblepp/options.h"
#include "pebblepp/pebble.h"
namespace py = pybind11;

#define DOC_READ_ONLY "Read-only by default."
#define DOC_SYNC "By default, syncs the write to disk."

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

PYBIND11_MODULE(pypebble, m) {
  // -- Module-level Functions and Utilities --
  m.def("cgo_handles", &CGoHandles,
        "Number of live handles to Go-managed memory available to C/Python.");
  m.def("pretty_key", &PrettyPrintKey, "Pretty-print a CockroachDB key.");
  m.def("scan_key", &PrettyScanKey, "Scan a human-readable CockroachDB key.");

  py::class_<Object>(m, "_Object").def_property_readonly("go_type", &Object::GoType);

  // -- pebble.Options and utils --
  py::class_<Options, Object>(m, "Options",
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

  py::class_<IterOptions, Object>(m, "IterOptions",
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
      //.def("clone", &Iterator::Clone)
      .def("seek_ge", &Iterator::SeekGE)
      .def("seek_ge_with_limit", &Iterator::SeekGEWithLimit)
      .def("seek_prefix_ge", &Iterator::SeekPrefixGE)
      .def("seek_lt", &Iterator::SeekLT)
      .def("seek_lt_with_limit", &Iterator::SeekLTWithLimit)
      .def("first", &Iterator::First)
      .def("last", &Iterator::Last)
      .def("next", &Iterator::Next)
      .def("next_with_limit", &Iterator::NextWithLimit)
      .def("prev", &Iterator::Prev)
      .def("prev_with_limit", &Iterator::PrevWithLimit)
      .def("range_key_changed", &Iterator::RangeKeyChanged)
      //.def("has_point_and_range", &Iterator::HasPointAndRange)
      .def("range_bounds", &Iterator::RangeBounds)
      .def("key", &Iterator::Key)
      // TODO(sarkesian): potentially remove, use converter for keys
      .def("pretty_key", &Iterator::PrettyKey)
      .def("value", &Iterator::Value)
      //.def("range_keys", &Iterator::RangeKeys)
      .def("valid", &Iterator::Valid)
      //.def("error", &Iterator::Error)
      .def("close", &Iterator::Close)
      .def("set_bounds", &Iterator::SetBounds)
      .def("set_options", &Iterator::SetOptions)
      .def("read_amp", &Iterator::ReadAmp)
      .def("reset_stats", &Iterator::ResetStats);
      //.def("stats", &Iterator::Stats);

  // -- pebble.DB --
  py::class_<DB, Object>(m, "DB",
                         "A concurrent, persistent ordered key/value Pebble store.")
      .def(py::init(&NoInit<DB>))
      .def("close", &DB::Close,
           "Close a open Pebble database.\n"
           "It is not safe to close a DB until all outstanding iterators are closed.")
      .def("num_files", &DB::NumFiles)
      .def("get", &DB::Get,
           "Get the value for a given key.",
           py::arg("key"))
      .def("set", &DB::Set,
           "Set the value for a given key.\n"
           DOC_SYNC,
           py::arg("key"),
           py::arg("val"),
           py::arg("sync") = true)
      .def("delete", &DB::Delete,
           "Deletes the value for a given key.\n"
           DOC_SYNC,
           py::arg("key"),
           py::arg("sync") = true)
      .def("single_delete", &DB::SingleDelete,
           "Add an action to the batch that single deletes the entry for a key.\n"
           DOC_SYNC,
           py::arg("key"),
           py::arg("sync") = true)
      .def("delete_range", &DB::DeleteRange,
           "Deletes all keys/values in the range [start,end).\n"
           DOC_SYNC,
           py::arg("start_key"),
           py::arg("end_key"),
           py::arg("sync") = true)
      .def("merge", &DB::Merge,
           "Merge the value at a given key with the new value.\n" DOC_SYNC,
           py::arg("key"),
           py::arg("val"),
           py::arg("sync") = true)
      .def("new_iter",
          [](DB* self, IterOptions& opts) { return self->NewIter(opts); },
          "Create a new, unpositioned pypebble.Iterator.",
          py::arg("opts"))
      .def("new_iter",
          [](DB* self) { return self->NewIter(); },
          "Create a new, unpositioned pypebble.Iterator.");

  // -- pebble.Open(..) --
  m.def("open",
      [](const std::string& path, bool read_write) { return DB::Open(path, read_write); },
      "Open a Pebble DB.", py::arg("path"), py::arg("read_write") = false);
  m.def("open",
      [](const std::string& path, const Options* opts) { return DB::Open(path, opts); },
      "Open a Pebble DB.", py::arg("path"), py::arg("options"));
}

}  // namespace cockroachdb::pebble
