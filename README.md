Multi-Version Timestamp Interval Locking (MVTIL)
================================================

MVTIL is a distributed concurrency control protocol based on multiversion timestamp locking (MVTL). Here we compare it with the alternative protocols, like 2PL and MVTO+ to demonstrate its advantage. More details can be found here: 

Compilation
-----------
The libraries required in this system are 

  * Boost
  * Apache Thrift
  * Intel TBB (https://github.com/01org/tbb)

You can install the dependencies of MVTIL by running script `make_dependencies.sh`


Build & Test
------------
To build the system

	make -j

To test the system


Configuration
-------------
The system configuration can be changed in `common/config.h` file. Here we list the meaning of some important configurations.


	S_THREAD_CNT		:	Number of threads running simultaneously in this server
	PORT				:	Service port
	DEFAULT_TIMEOUT		:	Timeout for one opertaion
	MIN_NUM_RETRIES		:	Minimum retry number of one read operation.
	MAX_RESTART			:	Maximum retry number of one transaction
	C_THREAD_CNT		:	Number of concurrent transactions in this client
	KEY_SPACE			:	Size of key space size
	TEST_TYPE			:	Type of the tested transaction. Five types of transactions are supported (Read-Only, Read-Intensive, Write-Intensive, Short, Mix)
	CLIENT_ID			:	Client Id
	TEST_DURATION_MS	:	Test Duration

For each protocol, there are some separate configurations.

MVTIL: 

	MVTL_POLICY			:	LOCK_FIRST_INTERVAL or LOCK_LAST_INTERVAL are supported
	INTERVAL_DURATION	:	Initial interval length




EC2 Deployment
--------------

It also contains the alternative protocols 2PL and MVTO 




## Dependecies:

* boost

* thrift

* libcuckoo (https://github.com/efficient/libcuckoo.git)

* cityhash (https://github.com/google/cityhash)

* tbb (https://github.com/01org/tbb)

