FROM ubuntu:latest as base
ARG CXX

RUN apt update && \
    apt install -y \
    git cmake bison flex doxygen wget \
    valgrind graphviz cppcheck


FROM base as gcc-image
RUN apt install -y gcc-12 g++-12 \
    build-essential g++ python-dev-is-python3 autotools-dev libicu-dev libbz2-dev && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 1 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 1

RUN apt --purge remove libboost-all-dev libboost-doc libboost-dev

RUN wget http://downloads.sourceforge.net/project/boost/boost/1.81.0/boost_1_81_0.tar.gz && \
    tar -zxvf boost_1_81_0.tar.gz && \
    cd boost_1_81_0/ && \
    ./bootstrap.sh && \
    ./b2 --with=all install && \
    cd ..

RUN git clone --branch v3.2.0 https://github.com/catchorg/Catch2.git /tmp/catch2

RUN mkdir -p /tmp/catch2/build && \
    cd /tmp/catch2/build && \
    cmake -DCMAKE_CXX_COMPILER=g++ .. && \
    make && \
    make install

ENTRYPOINT ["/bin/bash", "-s"]

FROM base as clang-image

RUN apt install -y clang-13 \
    build-essential g++ python-dev-is-python3 autotools-dev libicu-dev libbz2-dev && \
    update-alternatives --install /usr/bin/clang clang /usr/bin/clang-13 1 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-13 1

RUN apt --purge remove libboost-all-dev libboost-doc libboost-dev

RUN wget http://downloads.sourceforge.net/project/boost/boost/1.81.0/boost_1_81_0.tar.gz && \
    tar -zxvf boost_1_81_0.tar.gz && \
    cd boost_1_81_0/ && \
    ./bootstrap.sh && \
    ./b2 --with=all install && \
    cd ..

RUN git clone --branch v3.2.0 https://github.com/catchorg/Catch2.git /tmp/catch2

RUN mkdir -p /tmp/catch2/build && \
    cd /tmp/catch2/build && \
    cmake -DCMAKE_CXX_COMPILER=clang++ .. && \
    make && \
    make install

ENTRYPOINT ["/bin/bash", "-s"]"
