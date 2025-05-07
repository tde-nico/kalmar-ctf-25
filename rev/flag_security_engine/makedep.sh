#!/bin/sh
set -e

# if ! [ -e quickjs ]; then
#   git clone https://github.com/bellard/quickjs
# fi

# cd quickjs
# head=$(git rev-parse HEAD)
# cd ..

optflags=''
# optflags='-flto -O3'

zig cc -target x86_64-linux-gnu $optflags quickjs/quickjs.c quickjs/libregexp.c quickjs/libunicode.c quickjs/libbf.c quickjs/cutils.c main.cc -Iquickjs -DCONFIG_BIGNUM -DCONFIG_VERSION='"'"$head+kalmar"'"' -lm -o chall.amd64.pathced -g -fno-exceptions -O1 -flto -fPIE -fPIC
# zig cc -target aarch64-linux-gnu $optflags quickjs/{quickjs,libregexp,libunicode,libbf,cutils}.c main.cc -Iquickjs -DCONFIG_BIGNUM -DCONFIG_VERSION='"'"$head+kalmar"'"' -lm -o chall.arm64 -g -fno-exceptions -O1 -flto -fPIE -fPIC
