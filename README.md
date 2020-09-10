# BitTorrent

BitTorrent is a small torrenting client written in C++.

## Build

Simply run

```
git clone https://github.com/google/googletest.git
mkdir build
cd build
cmake ..
make
```
Debug mode:

```
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```
Debug build enables [AddressSanitizer](https://github.com/google/sanitizers), which can generate false positives for overflow on STL containers. If you encounter this, define the environment variable `ASAN_OPTIONS=detect_container_overflow=0`.

## Testing
```
cd build
make check-tests
```

