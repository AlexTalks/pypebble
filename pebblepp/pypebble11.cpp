#include <iostream>

#include "pebblepp/iterator.h"
#include "pebblepp/options.h"
#include "pebblepp/pebble.h"

#include "libpebble.h"

#include <pybind11/pybind11.h>

namespace pybind11 {

PyObject *
PyBytes_FromStringView(std::string_view sv)
{
  const char* str = sv.data();
  Py_ssize_t size = sv.size();

  PyBytesObject *op;
  if (size < 0) {
    pybind11_fail("no size");
  }

  //  op = (PyBytesObject *)_PyBytes_FromSize(size, 0);
  op = (PyBytesObject *)PyObject_Malloc(sizeof (PyBytesObject));
  //_PyObject_InitVar((PyVarObject*)op, &PyBytes_Type, size);
  Py_SET_TYPE(op, &PyBytes_Type);
  Py_INCREF(op);

  if (op == NULL) {
    pybind11_fail("null op");
    return NULL;
  }
  if (str == nullptr) {
    pybind11_fail("null str");
    return (PyObject *) op;
  }

  char** ptr_holder = reinterpret_cast<char**>(&(op->ob_sval));
  (*ptr_holder) = const_cast<char*>(str);

  std::cout << "(dbg) (myby) "
            << "addr: " << std::hex << (void*)(*ptr_holder) << ", "
            << "len: " << std::dec << size << std::endl;

  //  op->ob_sval = str
  //  memcpy(op->ob_sval, str, size);
  return (PyObject *) op;
}

class mybytes : public object {
 public:
  PYBIND11_OBJECT(mybytes, object, PYBIND11_BYTES_CHECK)

  mybytes(std::string_view sv) : sv_(sv), object(PyBytes_FromStringView(sv), borrowed_t{}) {
    if (!m_ptr) {
      pybind11_fail("Could not allocate bytes object!");
    }
  }

  operator std::string() const { return std::string(sv_); }

  // Obtain a string view that views the current `bytes` buffer value.  Note that this is only
  // valid so long as the `bytes` instance remains alive and so generally should not outlive the
  // lifetime of the `bytes` instance.
  operator std::string_view() const { return sv_; }
// private:
  std::string_view sv_;
};

};

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
  return (T*)NULL;
}

PYBIND11_MODULE(pypebble11, m) {
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

  m.def("str_from_go", []() {
     std::string_view s(StringFromGo());
     std::cout << "(dbg) (CPys) "
               << "addr: " << std::hex << (void*)(s.data()) << ", "
               << "len: " << std::dec << s.length() << std::endl;
     return py::bytes(s);
   });

  m.def("mybytes_from_go", []() {
    std::string_view s(StringFromGo());
    std::cout << "(dbg) (CPyb) "
              << "addr: " << std::hex << (void*)(s.data()) << ", "
              << "len: " << std::dec << s.length() << std::endl;
    return py::mybytes(s);
  });

//  m.def("addr", [](py::str b) {
//        char *buffer = nullptr;
//        ssize_t length = 0;
//        if (PyBytes_AsStringAndSize(b.ptr(), &buffer, &length) != 0) {
//          throw std::runtime_error("couldn't interpret str");
//        }
//        std::cout << "(dbg) (pstr) "
//                  << "addr: " << std::hex << (void*)buffer << ", "
//                  << "len: " << std::dec << length << ", "
//                  << "val: " << std::string(buffer, length) << std::endl;
//        std::cout << "(dbg) (pobj) "
//                  << "addr: " << std::hex << (void*)b.ptr() << ", "
//                  << "len: " << std::dec << len(b) << ", "
//                  << "val: " << std::string(b) << std::endl;
//      }, "print the address of the underlying data");

//  m.def("addr", [](py::mybytes b) {
//        std::string_view v(b);
//        std::cout << "(dbg) (myby) "
//                  << "addr: " << std::hex << (void*)v.data() << ", "
//                  << "len: " << std::dec << v.length() << std::endl;
//      }, "print the address of the underlying data");

  m.def("addr", [](py::bytes b) {
        std::string_view v(b);
        std::cout << "(dbg) (bysv) "
                  << "addr: " << std::hex << (void*)v.data() << ", "
                  << "len: " << std::dec << v.length() << std::endl;
//        std::cout << "(dbg) (pobj) "
//                  << "addr: " << std::hex << (void*)b.ptr() << ", "
//                  << "len: " << std::dec << len(b) << ", "
//                  << "val: " << std::string(b) << std::endl;
   }, "print the address of the underlying data");

  m.def("addr", [](py::object b) {
        std::cout << "(dbg) (pobj) "
                  << "addr: " << std::hex << (void*)b.ptr() << std::endl;
      }, "print the address of the underlying data");

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
      // TODO(sarkesian): experimental bytes return, finalize and/or remove.
      .def("KeyBytes", [](Iterator* self) {
        bytes_t result = PebbleIterKey(self->dbg_handle());

        std::cout << "(dbg) (KeyB) "
                  << "addr: " << std::hex << result.val << ", "
                  << "len: " << std::dec << result.len << ", "
                  << "val: " << std::string((char*)result.val, result.len) << std::endl;

        py::bytes retval((char*)result.val, result.len);
        //free(result.val);

        return retval;
      })
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
      // TODO(sarkesian): experimental bytes arg & return, finalize and/or remove.
      .def("GetBytes", [](DB* self, py::bytes k) {
        std::string_view key(k);
        bytes_and_error_t get_result = PebbleGet(self->dbg_handle(), (void*)key.data(), key.length());
        if (get_result.err_msg) {
          throw std::runtime_error(get_result.err_msg);
        }

        std::cout << "(dbg) (GetB) "
                  << "addr: " << std::hex << get_result.bytes.val << ", "
                  << "len: " << std::dec << get_result.bytes.len << ", "
                  << "val: " << std::string((char*)get_result.bytes.val, get_result.bytes.len) << std::endl;

        return py::bytes((char*)get_result.bytes.val, get_result.bytes.len);
      })
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
