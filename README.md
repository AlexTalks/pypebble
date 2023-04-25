# pypebble

A work-in-progress Python wrapper for CockroachDB's [Pebble](https://github.com/cockroachdb/pebble) 
storage engine, using a cgo and C++/pybind11-based interface.

## Building
Requires: cmake (>=3.5), Python (>=3.7), Pybind11, Go >=1.19, a C++ compiler.
```shell
$ go build -buildmode=c-shared -o libpebble.so ./pkg/bindings
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
(Make sure `[libpebble|pypebble].so` files are on the PATH)
```python
>>> import pypebble
>>> db = pypebble.Open("tmp", pypebble.BasicOptions())
>>> db.Set("name", "pebble!", True)
>>> db.Get("name")
'pebble!'
>>> db.Get("other")
Traceback (most recent call last):
File "<stdin>", line 1, in <module>
RuntimeError: pebble: not found
>>> db.NumFiles()
1
>>> db.Close()
```