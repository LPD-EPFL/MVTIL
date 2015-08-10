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

#ifndef _VERSION_SKIPLIST_H_
#define _VERSION_SKIPLIST_H_

#include <queue>

#include "common.h"
#include "random.h"
#include "Version.h"
#include "DataServer_types.h"

#define MAXLEVEL 4
class OrderedSetNode {
    friend class VersionSkiplist;
    public:
        OrderedSetNode(Timestamp ts, Version* v, int level);
        ~OrderedSetNode();

        inline Timestamp getTimestamp() {
            return timestamp;
        }

        inline Version* getVersion() {
            return version;
        }

        inline OrderedSetNode* getNext() {
            return next[0];
        }
    private:
        Timestamp timestamp;
        Version* version;
        int toplevel;
        OrderedSetNode** next;
};

class VersionSkiplist {
    public:
        VersionSkiplist();
        ~VersionSkiplist();
        inline Timestamp getFirstTimestamp() {
            return head->next[0]->timestamp;
        }

        OrderedSetNode* find(Timestamp ts, OrderedSetNode** prev);
        int insert(Timestamp ts, Version* v);
        int remove(Timestamp ts);
        int reposition(Timestamp old_ts);
        //returns the max read from ts seen in the removed versions
        Timestamp removeTo(Timestamp ts, std::queue<Timestamp>* q);
        size_t size();


    private:
        OrderedSetNode* head;
        inline int getRandomLevel();
        int levelmax;
        size_t sz;
        unsigned long* seeds;
};


#endif
