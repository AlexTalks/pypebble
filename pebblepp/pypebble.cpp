#include <boost/python.hpp>
#include "pebblepp/pebble.h"
using namespace boost::python;

BOOST_PYTHON_MODULE(pebble_ext)
{
  class_<cockroachdb::pebble::DB>("DB", init<std::string>())
      //.def("Open", cockroachdb::pebble::DB::Open).staticmethod("Open")
      .def("Close", &cockroachdb::pebble::DB::Close)
      .def("NumFiles", &cockroachdb::pebble::DB::NumFiles)
      .def("Set", &cockroachdb::pebble::DB::Set)
      .def("Get", &cockroachdb::pebble::DB::Get);
}

