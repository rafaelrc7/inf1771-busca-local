#!/bin/sh

set -e

cd $(dirname "$0")

autoheader
aclocal
automake --add-missing --copy
autoconf

