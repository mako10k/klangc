#!/bin/bash

set -e
sudo apt-get update
sudo apt-get install --yes libgc-dev
autoreconf -iv
./configure
make
sudo make install

