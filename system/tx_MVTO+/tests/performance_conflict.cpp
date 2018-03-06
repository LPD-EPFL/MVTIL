/* 
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

#include "client.h"
#include "TransactionManager.h"
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

using namespace std;

#define VALUE_SIZE 100

volatile bool start;
volatile bool stop;
volatile uint64_t thr[MAX_NUM_THREADS];
volatile uint64_t commit[MAX_NUM_THREADS];

TransactionManager* managers[MAX_NUM_THREADS];

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

inline TransactionType get_random_transaction_type(int type) {
	 return static_cast<TransactionType>(type);
}

int execute_transaction(int threadId, TransactionType type) {
	TransactionManager* transactionManager = managers[threadId];
	int suss = 0;
	int i;
	Value val;
	Value generated;
	Key key;
	switch(type) { 
		case RW_CONFLICT:
			{
				std::vector<int> ops(c_test_read + c_test_write,0);
				for(int i = 0; i < c_test_write; i++){
					ops[i] = 1;
				}
				std::random_shuffle(ops.begin(),ops.end());
				TX_START;
				for(auto op:ops){
					if(op == 0){
						key = generate_random_key();
						TX_READ(key, val);
					}
					else if(op == 1){
						key = generate_random_key();
						generated = generate_random_value();
						TX_WRITE(key, generated);
					}
				}
				TX_COMMIT;
				break;
			}
		default:
			std::cout<<"Unknown transaction type"<<std::endl;
			break;
	}
	return suss;
}

void execute_test(int threadId, int type) {
	uint64_t myThroughput = 0;
	uint64_t nu_commit = 0;
	while (start == false) {
		//wait
	}

	while (stop == false) {
	   TransactionType t = get_random_transaction_type(type);
	   nu_commit += execute_transaction(threadId, t);
	   myThroughput++; 
	}

	thr[threadId] = myThroughput;
	commit[threadId] = nu_commit;
}

int main(int argc, char **argv) {

	parser_client(argc, argv);

	start = false;
	stop = false;
	std::vector<std::thread> threads;
	uint32_t i;

	for  (i = 0; i < c_thread_cnt; i++) {
		thr[i] = 0;
		managers[i] = new TransactionManager(c_id*c_thread_cnt + i);
	}

	for  (i = 0; i < c_thread_cnt; i++) {
	   threads.push_back(std::thread(&execute_test, i, c_test_type)); 
	}

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
