#!/usr/bin/env bash

set -e

aclocal
autoconf
automake --add-missing
./configure
make

