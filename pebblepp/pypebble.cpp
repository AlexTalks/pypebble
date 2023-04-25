#include "pebblepp/iterator.h"
#include "pebblepp/options.h"
#include "pebblepp/pebble.h"

#include <pybind11/pybind11.h>
namespace py = pybind11;

namespace cockroachdb::pebble {

DB* (*dbOpen)(const std::string&) = &DB::Open;
DB* (*dbOpenWithOpts)(const std::string&, const Options*) = &DB::Open;

template <typename Copyable>
const Copyable CopyObject(const Copyable& v) {
  return v;
}

template<class T>
T* NoInit() {
  throw std::runtime_error("This class cannot be instantiated from Python");
  return (T*)nullptr;
}

PYBIND11_MODULE(pypebble, m) {
  // -- Module-level Functions and Utilities --
  m.def("CGoHandles", &CGoHandles,
        "number of live handles to Go-managed memory available to C/Python");
  m.def("PrettyPrintKey", &PrettyPrintKey, "pretty-print a CockroachDB key");
  m.def("PrettyScanKey", &PrettyScanKey, "scan a human-readable CockroachDB key");

  m.def("Open", dbOpen, "opens a pebble DB",
        py::arg("path"));
  m.def("Open", dbOpenWithOpts, "opens a pebble DB",
        py::arg("path"),
        py::arg("options"));

  // -- pebble.Options and utils --
  py::class_<Options>(m, "Options")
      .def(py::init(&NoInit<Options>));
  m.def("BasicOptions", &BasicOptions,
        "basic pebble options",
        py::arg("read_write") = false);
  m.def("CockroachDefaultOptions", &CockroachDefaultOptions,
        "default options for cockroachdb pebble instances",
        py::arg("read_write") = false);
  m.def("PebbleOptions", &PebbleOptions,
        "customizable pebble options",
        py::arg("read_write"),
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

  py::class_<IterOptions>(m, "IterOptions")
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

  py::class_<Iterator>(m, "Iterator")
      .def(py::init(&NoInit<Iterator>))
      //.def("Clone", &Iterator::Clone)
      .def("SeekGE", &Iterator::SeekGE)
      .def("SeekGEWithLimit", &Iterator::SeekGEWithLimit)
      .def("SeekPrefixGE", &Iterator::SeekPrefixGE)
      .def("SeekLT", &Iterator::SeekLT)
      .def("SeekLTWithLimit", &Iterator::SeekLTWithLimit)
      .def("First", &Iterator::First)
      .def("Last", &Iterator::Last)
      .def("Next", &Iterator::Next)
      .def("NextWithLimit", &Iterator::NextWithLimit)
      .def("Prev", &Iterator::Prev)
      .def("PrevWithLimit", &Iterator::PrevWithLimit)
      .def("RangeKeyChanged", &Iterator::RangeKeyChanged)
      //.def("HasPointAndRange", &Iterator::HasPointAndRange)
      .def("RangeBounds", &Iterator::RangeBounds)
      .def("Key", &Iterator::Key)
      // TODO(sarkesian): potentially remove, use converter for keys
      .def("PrettyKey", &Iterator::PrettyKey)
      .def("Value", &Iterator::Value)
      //.def("RangeKeys", &Iterator::RangeKeys)
      .def("Valid", &Iterator::Valid)
      //.def("Error", &Iterator::Error)
      .def("Close", &Iterator::Close)
      .def("SetBounds", &Iterator::SetBounds)
      .def("SetOptions", &Iterator::SetOptions)
      .def("ReadAmp", &Iterator::ReadAmp)
      .def("ResetStats", &Iterator::ResetStats);
  //.def("Stats", &Iterator::Stats);

  // -- pebble.DB --
  py::class_<DB>(m, "DB")
      .def(py::init(&NoInit<DB>))
      .def("Close", &DB::Close)
      .def("NumFiles", &DB::NumFiles)
      .def("Get", &DB::Get,
           "gets value of a key",
           py::arg("key"))
      .def("Set", &DB::Set, "stores a value at a key",
           py::arg("key"),
           py::arg("val"),
           py::arg("sync") = true)
      .def("Delete", &DB::Delete, "deletes a key",
           py::arg("key"),
           py::arg("sync") = true)
      .def("SingleDelete", &DB::SingleDelete, "single-deletes a key",
           py::arg("key"),
           py::arg("sync") = true)
      .def("DeleteRange", &DB::DeleteRange, "delete a range of keys",
           py::arg("start_key"),
           py::arg("end_key"),
           py::arg("sync") = true)
      .def("Merge", &DB::Merge, "merge a value at a key",
           py::arg("key"),
           py::arg("val"),
           py::arg("sync") = true)
      .def("NewIter", &DB::NewIter, "create a new pebble Iterator",
           py::arg("opts"));
}

}  // namespace cockroachdb::pebble
