# pypebble

---
A work-in-progress Python wrapper for CockroachDB's [Pebble](https://github.com/cockroachdb/pebble) 
storage engine, using a cgo and C++/Boost.Python-based interface.

## Building
(For now, built manually)
```shell
$ go build -buildmode=c-shared -o libpebble.so ./pkg/bindings
$ mkdir build && cd build
$ cmake -DPYTHON_LIBRARY=<path/to/latest/libpython3.XX.dylib> ..
$ make pebble_ext
```
Or, for just C++ bindings and sample program:
```shell
$ clang++ -Wall --std=c++17 -o pebtest.out -fPIC pebblepp/main.cpp pebblepp/pebble.cpp -I. -L. -lpebble
```

## Python Usage
(Make sure all `.so` files are on the correct path)
```python
>>> import pebble_ext
>>> db = pebble_ext.DB("tmp")
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