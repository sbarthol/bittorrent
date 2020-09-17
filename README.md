# BitTorrent

BitTorrent is a small torrenting client written from scratch in C++. The only external library used is OpenSSL for computing SHA1 hashes. Tested only on Ubuntu 18.04 plateform (Dockerfile provided).

## How does it work

* First, parse the torrent file which is "bencoded". The torrent file contains relevant
information like the tracker url (udp or http), the file name, file size, the piece length etc...
* Request the peer list from the tracker. If all goes well, the tracker will respond with
a list of (ip,port) pairs.
* Start downloading the file by sending messages over TCP. First do the BitTorrent "handshake", then
listen to the "have" and "bitfield" messages to know what pieces each peer has. Once you receive
an "unchoke" message, start requesting piece blocks.

## Build

Run the following commands:

```
mkdir build
cd build
cmake ..
make BitTorrent
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

