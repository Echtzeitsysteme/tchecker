#!/bin/sh -l

apt update
apt upgrade -y

export CC="gcc"
export CXX="g++"

./ci-scripts/build.sh
./ci-scripts/test.sh

./ci-scripts/show-config.sh
