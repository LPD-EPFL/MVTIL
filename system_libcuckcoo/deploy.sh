#!/bin/sh
sudo yum -y install gcc-c++
sudo yum -y install unzip
sudo yum -y install zlib-devel openssl-devel
sudo yum -y install lsof
sudo yum -y install python34

#install boost
wget http://sourceforge.net/projects/boost/files/boost/1.53.0/boost_1_53_0.tar.gz
tar -xzf boost_1_53_0.tar.gz
cd boost_1_53_0
./bootstrap.sh
sudo ./b2 install
cd ..

#install libevent for non-blocking server
wget https://github.com/downloads/libevent/libevent/libevent-2.0.21-stable.tar.gz
tar -xzf libevent-2.0.21-stable.tar.gz
cd libevent-2.0.21-stable
sudo ./configure
sudo make
sudo make install
cd ..

#intall thrift
wget http://apache.claz.org/thrift/0.10.0/thrift-0.10.0.tar.gz
tar -xzf thrift-0.10.0.tar.gz
cd thrift-0.10.0
sudo ./configure --without-java --without-python
sudo make
sudo make install
cd ..

#install cmake 3.0
wget https://cmake.org/files/v3.8/cmake-3.8.0-Linux-x86_64.tar.gz
tar -xzf cmake-3.8.0-Linux-x86_64.tar.gz

#install cityhash
wget https://github.com/google/cityhash/archive/master.zip
cd cityhash-master
sudo ./configure
sudo make 
sudo make install
cd ..

#install libcoco
wget https://github.com/efficient/libcuckoo/archive/master.zip
cd libcuckoo-master
mkdir build
cd build
../../cmake-3.8.0-Linux-x86_64/bin/cmake ..
sudo make
sudo make install
cd ..

#Complier MVTIL, MVTO+, 2PL
cd ../tx_MVTIL
sudo make

cd ../tx_MVTO+
sudo make

cd ../tx_2PL
sudo make

#export LD_LIBRARY_PATH="/usr/local/lib"

