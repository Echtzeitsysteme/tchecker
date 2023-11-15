#!/bin/sh -l

apt update
apt upgrade -y

export CC="clang"
export CXX="clang++"

./ci-scripts/build.sh
./ci-scripts/test.sh

./ci-scripts/show-config.sh
