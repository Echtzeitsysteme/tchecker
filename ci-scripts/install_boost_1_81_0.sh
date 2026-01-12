#!/usr/bin/env bash

wget https://sourceforge.net/projects/boost/files/boost/1.81.0/boost_1_81_0.tar.gz
tar xf boost_1_81_0.tar.gz
cd boost_1_81_0

./bootstrap.sh

#sudo ./b2 --with-all  link=static install --prefix=./boost