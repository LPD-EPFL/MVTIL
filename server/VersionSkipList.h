
#ifndef _VERSION_SKIPLIST_H_
#define _VERSION_SKIPLIST_H_

#include "common.h"
#include "random.h"
#include "Version.h"

#define MAXLEVEL 4
class OrderedSetNode {
    friend class VersionSkiplist;
    public:
        inline Timestamp getTimestamp();
        inline Version* getVersion();
        inline OrderedSetNode* next();
        OrderedSetNode(Timestamp ts, Version* v, int level);
        ~OrderedSetNode();
    private:
        Timestamp timestamp;
        Version* version;
        uint32_t toplevel;
        struct OrderedSetNode** next;
};

//typedef struct Node_t {
    //Timestamp timestamp;
    //Version* version;
    //uint32_t toplevel;
    //struct Node_t** next;

    //Node(Timestamp ts, Version* v, int level);
    //~Node();

//} Node;

class VersionSkiplist {
    public:
        VersionSkiplist();
        inline Timestamp getFirstTimestamp();
        OrderedSetNode* find(Timestamp ts, OrderedSetNode* prev);
        int insert(Timestamp ts, Version* v);
        int remove(Timestamp ts);
        int reposistion(Timestamp old_ts);
        size_t size();

    private:
        Node* head;
        inline long get_rand_level();
        int levelmax;
        size_t size;
};


#endif
