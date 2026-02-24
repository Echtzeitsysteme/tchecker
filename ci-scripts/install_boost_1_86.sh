#!/usr/bin/env bash

wget https://sourceforge.net/projects/boost/files/boost/1.86.0/boost_1_86_0.tar.gz
tar xf boost_1_86_0.tar.gz
cd boost_1_86_0

./bootstrap.sh

./b2 link=static cxxflags=-fPIC cflags=-fPIC install
