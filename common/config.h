
#ifndef _CONFIG_H_
#define _CONFIG_H_

//System
//#define CC_ALG 					MVTL
//#define ISOLATION_LEVEL 			SERIALIZABLE

//Server
#define S_THREAD_CNT				50
#define PORT						6060

#define POLICY						LOCK_FIRST_INTERVAL
#define LOCK_FIRST_INTERVAL			1
#define LOCK_LAST_INTERVAL			2

#define S_TYPE						NON_BLOCKING_SERVER
#define BLOCKING_SERVER				1
#define NON_BLOCKING_SERVER			2

#define GABARGE_COLLECT				1
#define DEFAULT_TIMEOUT				10
#define MIN_NUM_RETRIES				5


//Client
#define C_THREAD_CNT				100
#define KEY_SPACE					8000
#define INTERVAL_DURATION			10
#define TEST_TYPE					MIX

#define READ_ONLY					0
#define MANY_READS_ONE_WRITE		1
#define WRITE_INTENSIVE				2
#define RW_ONE_KEY					3
#define RW_SHORT					4
#define MIX							5

//Test
#define TEST_DURATION_MS			10000
#define MAX_NUM_THREADS				5000
#define RO_SIZE						20
#define RW_SIZE						10

//#define WORKLOAD 					YCSB

#endif