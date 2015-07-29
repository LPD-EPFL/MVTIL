
#ifndef _VERSION_SKIPLIST_H_
#define _VERSION_SKIPLIST_H_

#define MAXLEVEL 8

class VersionSkiplist {
    private:
        typedef struct Node_t {
            Timestamp timestamp;
            Version* version;
            uint32_t toplevel;
            struct Node_t* next[1];

            Node(Timestamp ts, Version* v, int level);
            ~Node();

        } Node;
    public:
        VersionSkipList();
        Version* find(Timestamp ts, Version* prev);
        int add(Timestamp ts, Version* v);
        int remove(Timestamp ts);
        int size();

    private:
        Node* head;
        inline long get_rand_level();
};


#endif
