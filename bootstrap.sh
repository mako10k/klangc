#!/bin/bash

set -e
sudo apt-get update
sudo apt-get install --yes libgc-dev doxygen graphviz
make uninstall || true
make distclean || true
autoreconf -iv
./configure --enable-debug
make clean
make all
sudo make install

