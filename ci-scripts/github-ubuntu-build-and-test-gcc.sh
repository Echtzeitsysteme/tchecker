#!/bin/sh -l

apt update
apt upgrade -y

export CC="gcc"
export CXX="g++"

chmod a+x ./ci-scripts/build.sh
chmod a+x ./ci-scripts/test.sh
chmod a+x ./ci-scripts/show-config.sh

ci-scripts/build.sh
retn_code=$?

if [ $retn_code -ne 0 ]; then
  exit $retn_code
fi

ci-scripts/test.sh
retn_code=$?
if [ $retn_code -ne 0 ]; then
  exit $retn_code
fi

ci-scripts/show-config.sh
