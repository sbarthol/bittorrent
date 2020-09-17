# BitTorrent

BitTorrent is a small torrenting client written from scratch in C++. The only external library used is OpenSSL for computing SHA1 hashes. Tested only on Ubuntu 18.04 plateform (Dockerfile provided).

## How does it work

torrent format
bencode
tracker (udp, http)
downloading
messages

## Build

Run the following commands:

```
mkdir build
cd build
cmake ..
make
./source/BitTorrent <torrent_file>
```

## Testing

Run the following commands:

```
git clone https://github.com/google/googletest.git
mkdir build
cd build
cmake ..
make
```

After having built the project, the test binaries are located
in build/test/

