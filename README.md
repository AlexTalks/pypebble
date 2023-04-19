# pypebble

A work-in-progress Python wrapper for CockroachDB's [Pebble](https://github.com/cockroachdb/pebble) 
storage engine, using a cgo and C++/Boost.Python-based interface.

## Building
(For now, built manually)
```shell
$ go build -buildmode=c-shared -o libpebble.so ./pkg/bindings
$ mkdir build && cd build
$ cmake ..
$ make pypebble
```
Or, for just C++ bindings and sample program:
```shell
$ clang++ -Wall --std=c++17 -o pebtest.out -fPIC pebblepp/main.cpp pebblepp/pebble.cpp pebblepp/options.cpp -I. -L. -lpebble
```

## Python Usage
(Make sure all `.so` files are on the PATH)
```python
>>> import pypebble
>>> db = pypebble.Open("tmp")
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