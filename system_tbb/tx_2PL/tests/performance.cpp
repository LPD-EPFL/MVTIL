 * /* 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TransactionManager.h"
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

//Get Local IP
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

//#define KEY_SIZE 5
#define VALUE_SIZE 100

volatile bool start;
volatile bool stop;
volatile uint64_t thr[MAX_NUM_THREADS];
volatile uint64_t commit[MAX_NUM_THREADS];

TransactionManager* transactionManager;

// defined in parser.cpp
void parser_client(int argc, char * argv[]);

std::string random_string(int num)
{
	std::string str = std::to_string(rand() % num);
	return str;
}

inline std::string generate_random_key() {
	return random_string(c_key_space);
}

inline std::string generate_random_value() {
	return random_string(VALUE_SIZE);
}

//typedef enum {READ_ONLY, MANY_READS_ONE_WRITE, WRITE_INTENSIVE, RW_ONE_KEY, R_ONE_KEY, RW_SHORT, NUM_TTYPES} TransactionType;

inline TransactionType get_random_transaction_type(int type) {
	if(type < MIX){
		return static_cast<TransactionType>(type);
	}
	else{
		return static_cast<TransactionType>(rand() % MIX);   //TODO change rand function
	}
}

int execute_transaction(TransactionType type) {
	int suss = 0;
	int i;
	Value val;
	Value generated;
	Key key;
	switch(type) { 
		case READ_ONLY:
			TX_START;
			for (i=0; i<RO_SIZE; i++) {
				key = generate_random_key();
				TX_READ(key, val);
			}
			TX_COMMIT;
			break;
		/*
		case MANY_READS_ONE_WRITE:
			TX_START;
			Key w = generate_random_key();
			for (i=0; i<RO_SIZE; i++) {
				key = generate_random_key();
				TX_READ(key, val);
			}
			generated = generate_random_value();
			TX_WRITE(w, generated);
			TX_COMMIT;
			break;
		*/
		case READ_INTENSIVE:
			TX_START;
			for (i=0; i<RW_SIZE; i++) {
				key = generate_random_key();
				TX_READ(key, val);
			}
			for (i=0; i<RW_SIZE/2; i++) {
				key = generate_random_key();
				TX_READ(key, val);
				key = generate_random_key();
				TX_WRITE(key, val);
			}
			TX_COMMIT;
			break;
		case WRITE_INTENSIVE:
			TX_START;
			for (i=0; i<RW_SIZE; i++) {
				key = generate_random_key();
				TX_READ(key, val);
				key = generate_random_key();
				TX_WRITE(key, val);
			}
			TX_COMMIT;
			break;
		case RW_ONE_KEY:
			TX_START;
			for (i=0; i<RW_SIZE; i++){
				key = generate_random_key();
				TX_READ(key, val);
				generated = generate_random_value();
				TX_WRITE(key, generated);
			}
			TX_COMMIT;
			break;
		case RW_LONG:
			TX_START;
			key = generate_random_key();
			TX_READ(key, val);
			generated = generate_random_value();
			TX_WRITE(key, generated);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			key = generate_random_key();
			TX_READ(key, val);
			generated = generate_random_value();
			TX_WRITE(key, generated);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			key = generate_random_key();
			TX_READ(key, val);
			generated = generate_random_value();
			TX_WRITE(key, generated);
			TX_COMMIT;
			break;
		case RW_SHORT:
			TX_START;
			key = generate_random_key();
			TX_READ(key, val);
			generated = generate_random_value();
			TX_WRITE(key, generated);
			key = generate_random_key();
			TX_READ(key, val);
			generated = generate_random_value();
			TX_WRITE(key, generated);
			key = generate_random_key();
			TX_READ(key, val);
			generated = generate_random_value();
			TX_WRITE(key, generated);
			TX_COMMIT;
			break;
		default:
			std::cout<<"Unknown transaction type"<<std::endl;
			break;
	}
	return suss;
}

void execute_test(int threadId, int type) {
	uint64_t myThroughput = 0;
	uint64_t nu_commit = 0;

	//cout<<"Thread Id:"<<threadId <<"start!"<<endl;
	while (start == false) {
		//wait
	}

	while (stop == false) {
	   TransactionType t = get_random_transaction_type(type);
	   nu_commit += execute_transaction(t);
	   myThroughput++; 
	}

	thr[threadId] = myThroughput;
	commit[threadId] = nu_commit;
	//cout<<"Thread Id:"<<threadId <<" finish!"<<endl; 
}

int generate_client_id(){
	int fd;
	struct ifreq ifr;
	char iface[] = "eth0";
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	unsigned short a, b, c, d;
	sscanf(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr )->sin_addr), "%hu.%hu.%hu.%hu", &a, &b, &c, &d);
	return d;
}

int main(int argc, char **argv) {
	
	parser_client(argc, argv);

	start = false;
	stop = false;
	std::vector<std::thread> threads;
	uint32_t i;

	int client_id = generate_client_id();
	transactionManager = new TransactionManager(client_id);

	for  (i = 0; i < c_thread_cnt; i++) {
		thr[i] = 0;
	}

	for  (i = 0; i < c_thread_cnt; i++) {
	   threads.push_back(std::thread(&execute_test, i, c_test_type));
	}

	//allow threads to start 
	start = true;

	//sleep
	std::this_thread::sleep_for(std::chrono::milliseconds(c_test_duration));

	stop = true; 

	for (auto& th: threads) {
		th.join();
	}

	//gather statistics
	uint64_t total_throughput = 0;
	uint64_t total_commit = 0;
	
	for  (i = 0; i < c_thread_cnt; i++) {
		total_throughput+=thr[i]; 
		total_commit+=commit[i];
	}

	cout<<total_commit<<" "<<total_throughput<<endl;
	return 0;
}
