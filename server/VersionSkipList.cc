#include "VersionSkipList.h"

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
    }
    return result;
}
