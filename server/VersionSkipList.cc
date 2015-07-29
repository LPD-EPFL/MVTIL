#include "VersionSkipList.h"

VersionSkiplist::Node::Node(Timestamp ts, Version*v, int level) : timestamp(ts), version(v), toplevel(level) {
    next = new Node*[MAXLEVEL];
}

VersionSkiplist::Node::~Node() {
    delete next;
    delete version;
}

VersionSkiplist::VersionSkiplist() {
    levelmax = MAXLEVEL;
    Node* min = new Node(MIN_TIMESTAMP, NULL, levelmax);
    Node* max = new Node(MAX_TIMESTAMP + 1, NULL, levelmax);
    max->next[0] = NULL;

    int i;
    for (i=0; i<levelmax; i++) {
        min->next[i] = max;
    }
    head = min;
    size = 0;
}

VersionSkiplist::~VersionSkiplist() {
    Node* node = head;
    Node* next;
    while (node != NULL) {
        next= node->next[0];
        delete node;
        node = next;
    }

}

inline int VersionSkiplist::get_rand_level()
{
    int i, level = 1;
    for (i = 0; i < levelmax - 1; i++) {
        if ((rand_range(101)) < 50)
            level++;
        else
            break;
    }
    /* 1 <= level <= levelmax */
    return level;
}


//returns the first node with timestamp >= ts; also returns its predecessor
Version* VersionSkipList::find(Timestamp ts, Version* pred) {
    int i;
    Node* node;
    Node* next;
    Version* result = NULL;

    node = head;
    for (i = node->toplevel-1; i >=0; i--) {
        next = node->next[i];
        while (next->timestamp < ts) {
            node = next;
            next = node->next[i];
        }
    }
    pred = node->version;
    node = node->next[0];
    result = node->version;
    return result;
}

int VersionSkipList::add(Timestamp ts, Version* v) {
    int i, l, result;
    Node* node;
    Node* next;
    Node* preds[MAXLEVEL];
    Node* succs[MAXLEVEL];

    node = head;
    for (i = node->toplevel-1; i>=0; i--) {
        next = node->next[i];
        while (next->timestamp < ts) {
            node = next;
            next = node->next[i];
        }
        preds[i] = node;
        succs[i] = node->next[i];
    }
    node = node->next[0];

    result = (node->timestamp != ts);
    if (result == 1) {
        l = getRandomLevel();
        node = new Node(ts, v, l); 
        for (i = 0; i < l; i++) {
            node->next[i] = succs[i];
            preds[i]->next[i] = node;
        }
        size++;
    }
    return result;
}

int VersionSkipList::remove(Timestamp ts) {
    int result = 0;
    int i;
    Node* node = NULL;
    Node* next = NULL;
    Node* preds[MAXLEVEL];
    Node* succs[MAXLEVEL];

    node = head;
    for (i = node->toplevel-1; i >= 0; i--) {
        next = node->next[i];
        while (next->timestamp < ts) {
            node = next;
            next = node->next[i];
        }
        preds[i] = node;
        succs[i] = node->next[i];
    }

    if (next->timestamp == ts) {
        result = 1;
        for (i = 0; i < head->toplevel; i++) {
            if (succs[i]->timestamp == ts) {
                preds[i]->next[i] = succs[i]->next[i];
            }
        }
        delete(next);
        size--;
    }
    return result;
}

size_t VersionSkiplist::size() {
    return size;
}
