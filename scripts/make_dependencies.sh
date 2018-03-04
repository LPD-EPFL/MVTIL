#!/bin/sh

#install boost
cd /tmp

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

#install tbb
wget https://github.com/01org/tbb/releases/download/2017_U7/tbb2017_20170604oss_lin.tgz
tar -xzf tbb2017_20170604oss_lin.tgz
sudo mv tbb2017_20170604oss tbb
sudo mkdir /usr/local/include/tbb
sudo mv tbb/include/tbb/* /usr/local/include/tbb
sudo mv tbb/lib/intel64/gcc4.7/* /usr/local/lib