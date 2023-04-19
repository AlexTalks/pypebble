#include <boost/python.hpp>
#include "pebblepp/options.h"
#include "pebblepp/pebble.h"
using namespace boost::python;

namespace cockroachdb {
namespace pebble {

BOOST_PYTHON_FUNCTION_OVERLOADS(options_overloads, CockroachDefaultOptions, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(set_overloads, Set, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(delete_overloads, Delete, 1, 2)

DB* (*open1)(const std::string&) = &DB::Open;
DB* (*open2)(const std::string&, const Options*) = &DB::Open;

BOOST_PYTHON_MODULE(pypebble)
{
  class_<Options, boost::noncopyable>("Options", no_init);
  def("BasicOptions", &BasicOptions,
      return_value_policy<manage_new_object>());
  def("CockroachDefaultOptions", &CockroachDefaultOptions,
      options_overloads(
          args("read_write"), "cockroach default options"
          )[return_value_policy<manage_new_object>()]
      );
  // TODO(sarkesian): Convert the below arguments to kwargs.
  def("PebbleOptions", &PebbleOptions,
      args(
          "read_write",
          "use_cockroach_interfaces",
          "l_0_compaction_threshold",
          "l0_stop_writes_threshold",
          "l_base_max_bytes",
          "levels",
          "max_concurrent_compactions",
          "mem_table_size",
          "mem_table_stop_writes_threshold",
          "block_size",
          "index_block_size"),
      return_value_policy<manage_new_object>(), "custom Pebble options");

  class_<DB, boost::noncopyable>("DB", no_init)
      .def("Close", &DB::Close)
      .def("NumFiles", &DB::NumFiles)
      .def("Get", &DB::Get,
           args("key"), "gets value of a key")
      .def("Set", &DB::Set,
           set_overloads(
               args("key", "val", "sync"), "sets a key to a value"
               ))
      .def("Delete", &DB::Delete,
           delete_overloads(
               args("key", "sync"), "deletes a key"
               ))
      .def("SingleDelete", &DB::SingleDelete,
           delete_overloads(
               args("key", "sync"), "deletes a key"
               ))
      .def("DeleteRange", &DB::DeleteRange,
           set_overloads(
               args("start_key", "end_key", "sync"), "delete a range of keys"
               ))
      .def("Merge", &DB::Merge,
           set_overloads(
               args("key", "val", "sync"), "merge a value into a key"
               ));

  def("Open", open1, args("path"),
      return_value_policy<manage_new_object>(), "opens a new pebble.DB");
  def("Open", open2, args("path", "options"),
      return_value_policy<manage_new_object>(), "opens a new pebble.DB");
  def("CGoHandles", &CGoHandles,
      "number of live handles to Go-managed memory available to C/Python");
}

} // namespace pebble
} // namespace cockroachdb

