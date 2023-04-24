#include <boost/python.hpp>

#include "pebblepp/iterator.h"
#include "pebblepp/options.h"
#include "pebblepp/pebble.h"
using namespace boost::python;

namespace cockroachdb {
namespace pebble {

BOOST_PYTHON_FUNCTION_OVERLOADS(basic_options_overloads, BasicOptions, 0, 1)
BOOST_PYTHON_FUNCTION_OVERLOADS(cockroach_options_overloads, CockroachDefaultOptions, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(set_overloads, Set, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(delete_overloads, Delete, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(single_delete_overloads, SingleDelete, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(delete_range_overloads, DeleteRange, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(merge_overloads, Merge, 2, 3)

DB* (*open1)(const std::string&) = &DB::Open;
DB* (*open2)(const std::string&, const Options*) = &DB::Open;

template <typename Copyable>
const Copyable copyObject(const Copyable& v) {
  return v;
}

BOOST_PYTHON_MODULE(pypebble) {
  class_<Options, boost::noncopyable>("Options", no_init);
  def("BasicOptions", &BasicOptions,
      basic_options_overloads(args("read_write"),
                              "basic pebble options")[return_value_policy<manage_new_object>()]);
  def("CockroachDefaultOptions", &CockroachDefaultOptions,
      cockroach_options_overloads(
          args("read_write"),
          "cockroach default options")[return_value_policy<manage_new_object>()]);
  // TODO(sarkesian): Convert the below arguments to kwargs.
  def("PebbleOptions", &PebbleOptions,
      args("read_write", "use_cockroach_interfaces", "l0_compaction_threshold",
           "l0_stop_writes_threshold", "lbase_max_bytes", "levels", "max_concurrent_compactions",
           "mem_table_size", "mem_table_stop_writes_threshold", "block_size", "index_block_size"),
      return_value_policy<manage_new_object>(), "custom Pebble options");

  enum_<IterKeyType>("IterKeyType")
      .value("POINTS_ONLY", kPointsOnly)
      .value("RANGES_ONLY", kRangesOnly)
      .value("POINTS_AND_RANGES", kPointsAndRanges);

  class_<IterOptions>("IterOptions")
      .def(init<const IterOptions&>())
      .def("__copy__", &copyObject<IterOptions>)
      .add_property("lower_bound", &IterOptions::LowerBound, &IterOptions::SetLowerBound)
      .add_property("upper_bound", &IterOptions::UpperBound, &IterOptions::SetUpperBound)
      .add_property("key_types", &IterOptions::KeyTypes, &IterOptions::SetKeyTypes);

  enum_<IterValidityState>("IterValidityState")
      .value("EXHAUSTED", kExhausted)
      .value("VALID", kValid)
      .value("AT_LIMIT", kAtLimit);

  class_<Iterator, boost::noncopyable>("Iterator", no_init)
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

  class_<DB, boost::noncopyable>("DB", no_init)
      .def("Close", &DB::Close)
      .def("NumFiles", &DB::NumFiles)
      .def("Get", &DB::Get, args("key"), "gets value of a key")
      .def("Set", &DB::Set, set_overloads(args("key", "val", "sync"), "sets a key to a value"))
      .def("Delete", &DB::Delete, delete_overloads(args("key", "sync"), "deletes a key"))
      .def("SingleDelete", &DB::SingleDelete,
           single_delete_overloads(args("key", "sync"), "single deletes a key"))
      .def("DeleteRange", &DB::DeleteRange,
           delete_range_overloads(args("start_key", "end_key", "sync"), "delete a range of keys"))
      .def("Merge", &DB::Merge,
           merge_overloads(args("key", "val", "sync"), "merge a value into a key"))
      .def("NewIter", &DB::NewIter, return_value_policy<manage_new_object>());

  def("Open", open1, args("path"), return_value_policy<manage_new_object>(),
      "opens a new pebble.DB");
  def("Open", open2, args("path", "options"), return_value_policy<manage_new_object>(),
      "opens a new pebble.DB");
  def("CGoHandles", &CGoHandles,
      "number of live handles to Go-managed memory available to C/Python");
}

}  // namespace pebble
}  // namespace cockroachdb
