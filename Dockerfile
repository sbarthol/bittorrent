FROM ubuntu:18.04

RUN sed -i 's:^path-exclude=/usr/share/man:#path-exclude=/usr/share/man:' \
        /etc/dpkg/dpkg.cfg.d/excludes

RUN apt-get -y update && \
    apt-get -y install \
      cmake \
      g++ \
      man \
      manpages-posix \
      libssl-dev \
      build-essential

WORKDIR /app/build

# docker build -t bit_torrent .
# docker run -it -v /Users/sacha/Desktop/Projects/BitTorrent:/app bit_torrent
