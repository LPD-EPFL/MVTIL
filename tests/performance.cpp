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

#include "TransactionManager.h"
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

#define KEY_SIZE 4
#define VALUE_SIZE 100

volatile bool start;
volatile bool stop;
volatile uint64_t thr[NUM_THREADS];
volatile uint64_t commit[NUM_THREADS];

TransactionManager* transactionManager;

//function from https://stackoverflow.com/a/12468109
std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ]; //TODO change rand function
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

inline std::string generate_random_key() {
    return random_string(KEY_SIZE);
}

inline std::string generate_random_value() {
    return random_string(VALUE_SIZE);
}

//typedef enum {READ_ONLY, MANY_READS_ONE_WRITE, WRITE_INTENSIVE, RW_ONE_KEY, R_ONE_KEY, RW_SHORT, NUM_TTYPES} TransactionType;

inline TransactionType get_random_transaction_type() {
  return static_cast<TransactionType>(rand() % MIX);   //TODO change rand function
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
                //generated = generate_random_value();
                TX_WRITE(key, generated);
            }
            TX_COMMIT;
            break;
        case RW_SHORT:
            TX_START;
            key = generate_random_key();
            TX_READ(key, val);
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

void execute_test(int threadId) {
    uint64_t myThroughput = 0;
    uint64_t nu_commit = 0;

    //cout<<"Thread Id:"<<threadId <<"start!"<<endl;
    while (start == false) {
        //wait
    }

    while (stop == false) {
       TransactionType t = get_random_transaction_type();
       nu_commit += execute_transaction(t);
       myThroughput++; 
    }

    thr[threadId] = myThroughput;
    commit[threadId] = nu_commit;
    //cout<<"Thread Id:"<<threadId <<" finish!"<<endl; 
}

int main(int argc, char **argv) {
    start = false;
    stop = false;
    std::vector<std::thread> threads;
    int i;

    transactionManager=new TransactionManager(0);

    for  (i = 0; i < NUM_THREADS; i++) {
        thr[i] = 0;
    }

    for  (i = 0; i < NUM_THREADS; i++) {
       threads.push_back(std::thread(&execute_test, i)); 
    }

    //allow threads to start 
    start = true;

    //sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(TEST_DURATION_MS));

    stop = true; 

    for (auto& th: threads) {
        th.join();
    }

    //gather statistics
    uint64_t total_throughput = 0;
    uint64_t total_commit = 0;
    
    for  (i = 0; i < NUM_THREADS; i++) {
        total_throughput+=thr[i]; 
        total_commit+=commit[i];
    }

    cout<<total_commit<<" "<<total_throughput<<endl;
    return 0;
}
