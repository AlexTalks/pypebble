# pypebble

A work-in-progress Python wrapper for CockroachDB's [Pebble](https://github.com/cockroachdb/pebble) 
storage engine, using a cgo and C++/pybind11-based interface.

## Building
Requires: cmake (>=3.5), Python (>=3.7), Pybind11, Go >=1.19, a C++ compiler.
```shell
$ mkdir build && cd build
$ cmake ..
$ make pypebble
```
Or, for just C++ bindings and sample program:
```shell
$ make gen_libpebble
..
$ make pebtest.out
```

Manual build notes (deprecated):
```shell
$ go build -buildmode=c-shared -o libpebble.so ./pkg/bindings
..
$ clang++ -Wall --std=c++17 -o pebtest.out -fPIC pebblepp/*.cpp -I. -L. -lpebble
```

## Python Usage
Make sure `[libpebble|pypebble].so` files are on the PATH.

You may need to set `DYLD_LIBRARY_PATH=/path/to/libpebble.so` as well.
```python
>>> import pypebble
>>> db = pypebble.open("tmp", read_write=True)
>>> db.set(b"name", b"pebble!", sync=True)
>>> db.get(b"name")
b'pebble!'
>>> db.get(b"other")
Traceback (most recent call last):
File "<stdin>", line 1, in <module>
RuntimeError: pebble: not found
>>> db.num_files()
1
>>> db.close()
```
