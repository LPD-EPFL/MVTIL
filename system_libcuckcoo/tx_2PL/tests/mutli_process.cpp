/*   
 *   File: client_exec.h
 *   Author: Junxiong Wang <junxiong.wang@epfl.ch>
 *   Description: 
 *   client_exec.h is part of MVTLSYS
 *
 * Copyright (c) 2017 Junxiong Wang <junxiong.wang@epfl.ch>,
 *                Tudor David <tudor.david@epfl.ch>
 *                Distributed Programming Lab (LPD), EPFL
 *
 * MVTLSYS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "TransactionManager.h"
#include <string>
#include <chrono>
#include <iostream>

#define NUM_THREADS 10
#define RO_SIZE 100
#define RW_SIZE 50
#define KEY_SIZE 10
#define VALUE_SIZE 100

#define TEST_DURATION_MS 10000

volatile bool start;
volatile bool stop;
volatile uint64_t processes[NUM_THREADS];

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

typedef enum {READ_ONLY, MANY_READS_ONE_WRITE, WRITE_INTENSIVE, RW_ONE_KEY, R_ONE_KEY, RW_SHORT, NUM_TTYPES} TransactionType;

inline TransactionType get_random_transaction_type() {
  return static_cast<TransactionType>(rand() % NUM_TTYPES);   //TODO change rand function
}

void execute_transaction(TransactionType type) {

    int i;
    Value val;
    Value generated;
    Key key;
    switch(type) {
        case READ_ONLY:
            TX_START_RO;
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
            key = generate_random_key();
            TX_READ(key, val);
            generated = generate_random_value();
            TX_WRITE(key, generated);
            TX_COMMIT;
            break;
        case R_ONE_KEY:
            TX_START_RO;
            key = generate_random_key();
            TX_READ(key, val);
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

}

void execute_test(int processId) {
    uint64_t myThroughput = 0;

    while (stop == false) {
       TransactionType t = get_random_transaction_type();
       execute_transaction(t);
       myThroughput++; 
    }

    process[processId] = myThroughput; 
}

int main(int argc, char **argv) {

    return 0;
}