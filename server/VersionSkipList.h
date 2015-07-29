
#ifndef _VERSION_SKIPLIST_H_
#define _VERSION_SKIPLIST_H_

#include "common.h"
#include "random.h"
#include "Version.h"

#define MAXLEVEL 8

class VersionSkiplist {
    private:
        typedef struct Node_t {
            Timestamp timestamp;
            Version* version;
            uint32_t toplevel;
            struct Node_t** next;

            Node(Timestamp ts, Version* v, int level);
            ~Node();

        } Node;
    public:
        VersionSkiplist();
        Version* find(Timestamp ts, Version* prev);
        int add(Timestamp ts, Version* v);
        int remove(Timestamp ts);
        size_t size();

    private:
        Node* head;
        inline long get_rand_level();
        int levelmax;
        size_t size;
};


#endif
