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

#define NUM_THREADS 10
#define RO_SIZE 100

typedef enum {READ_ONLY, MANY_READS_ONE_WRITE, WRITE_INTENSIVE, RW_ONE_KEY, R_ONE_KEY, RW_SHORT} TransactionType;

void execute_transaction(TransactionType type) {
    switch(type) {
        case READ_ONLY:
            int i;
            Value* val;
            Key key;
            TX_START_RO;
            for (i=0; i<RO_SIZE; i++) {
                key = generate_random_key();
                TX_READ(key, val);
            }
            TX_END;
            break;
        case MANY_READS_ONE_WRITE:

            break;
        case WRITE_INTENSIVE;

            break;
        case RW_ONE_KEY;

            break;
        case R_ONE_KEY;

            break;
        case RW_SHORT;

            break;
        default:
            std::cout<<"Unknown transaction type"<<std::endl;
            break;
    }

}



int main(int argc, char **argv) {
    TX_INIT;

}
