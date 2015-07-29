
#ifndef _VERSION_SKIPLIST_H_
#define _VERSION_SKIPLIST_H_

typedef struct Node {
    
} Node;

class VersionSkiplist {
    public:
        Version* find(Timestamp ts, Version* prev);
        int add(Timestamp ts, Version* v);
        int remove(Timestamp ts);

    private:
        Node* head;
};


#endif
