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

#include <string>
#include <iostream>
#include "TransactionManager.h"

#define NUM_THREADS 10
#define RO_SIZE 100
#define RW_SIZE 50
#define KEY_SIZE 10
#define VAL_SIZE 100

//https://stackoverflow.com/a/12468109
std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
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

typedef enum {READ_ONLY, MANY_READS_ONE_WRITE, WRITE_INTENSIVE, RW_ONE_KEY, R_ONE_KEY, RW_SHORT} TransactionType;

void execute_transaction(TransactionType type) {

    int i;
    Value* val;
    Key key;
    switch(type) {
        case READ_ONLY:
            TX_START_RO;
            for (i=0; i<RO_SIZE; i++) {
                key = generate_random_key();
                TX_READ(key, val);
            }
            TX_END;
            break;
        case MANY_READS_ONE_WRITE:
            TX_START;
            Key w = generate_radom_key();
            TX_DECLARE_WRITE(w);
            for (i=0; i<RO_SIZE; i++) {
                key = generate_random_key();
                TX_READ(key, val);
            }
            val = generate_random_value();
            TX_WRITE(w, val);
            TX_END;
            break;
        case WRITE_INTENSIVE:
            TX_START;
            for (i=0; i<RW_SIZE; i++) {
                key = generate_random_key();
                TX_READ(key, val);
                key = generate_random_key();
                TX_WRITE(key, val);
            }
            TX_END;
            break;
        case RW_ONE_KEY:
            TX_START;
            key = generate_random_key();
            TX_READ(key, val);
            val = generate_random_value();
            TX_WRITE(key, val);
            TX_END;
            break;
        case R_ONE_KEY:
            TX_START_RO;
            key = generate_random_key();
            TX_READ(key, val);
            TX_END;
            break;
        case RW_SHORT:
            TX_START;
            key = generate_random_key();
            TX_READ(key, val);
            key = generate_random_key();
            TX_READ(key, val);
            val = generate_random_value();
            TX_WRITE(key, val);
            key = generate_random_key();
            TX_READ(key, val);
            val = generate_random_value();
            TX_WRITE(key, val);
            TX_END;
            break;
        default:
            std::cout<<"Unknown transaction type"<<std::endl;
            break;
    }

}



int main(int argc, char **argv) {
    TX_INIT;

}
