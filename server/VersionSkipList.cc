#include "VersionSkipList.h"

OrderedSetNode::OrderedSetNode(Timestamp ts, Version*v, int level) : timestamp(ts), version(v), toplevel(level) {
    next = new OrderedSetNode*[MAXLEVEL];
}

OrderedSetNode::~OrderedSetNode() {
    delete next;
    delete version;
}

inline Timestamp OrderedSetNode::getTimestamp() {
    return timestamp;
}

inline Version* OrderedSetNode::getVersion() {
    return version;
}

inline OrderedSetNode* OrderedSetNode::next() {
    return next[0];
}

VersionSkiplist::VersionSkiplist() {
    levelmax = MAXLEVEL;
    OrderedSetNode* min = new OrderedSetNode(MIN_TIMESTAMP, NULL, levelmax);
    OrderedSetNode* max = new OrderedSetNode(MAX_TIMESTAMP + 1, NULL, levelmax);
    max->next[0] = NULL;

    int i;
    for (i=0; i<levelmax; i++) {
        min->next[i] = max;
    }
    head = min;
    sz = 0;
}

VersionSkiplist::~VersionSkiplist() {
    OrderedSetNode* node = head;
    OrderedSetNode* next;
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

inline Timestamp VersionSkiplist::getFirstTimestamp() {
    return head->next[0]->timestamp;
}


//returns the first node with timestamp >= ts; also returns its predecessor
OrderedSetNode* VersionSkiplist::find(Timestamp ts, OrderedSetNode* pred) {
    int i;
    OrderedSetNode* node;
    OrderedSetNode* next;
    OrderedSetNode* result = NULL;

    node = head;
    for (i = node->toplevel-1; i >=0; i--) {
        next = node->next[i];
        while (next->timestamp < ts) {
            node = next;
            next = node->next[i];
        }
    }
    pred = node;
    node = node->next[0];
    result = node;
    return result;
}

int VersionSkiplist::insert(Timestamp ts, Version* v) {
    int i, l, result;
    OrderedSetNode* node;
    OrderedSetNode* next;
    OrderedSetNode* preds[MAXLEVEL];
    OrderedSetNode* succs[MAXLEVEL];

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
        node = new OrderedSetNode(ts, v, l); 
        for (i = 0; i < l; i++) {
            node->next[i] = succs[i];
            preds[i]->next[i] = node;
        }
        sz++;
    }
    return result;
}

int VersionSkiplist::remove(Timestamp ts) {
    int result = 0;
    int i;
    OrderedSetNode* node = NULL;
    OrderedSetNode* next = NULL;
    OrderedSetNode* preds[MAXLEVEL];
    OrderedSetNode* succs[MAXLEVEL];

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
        sz--;
    }
    return result;
}

//reposition a node once the timestamp in the version has changed
int VersionSkiplist::reposition(Timestamp old_ts) {
    int result = 1;
    int i;
    OrderedSetNode* node = NULL;
    OrderedSetNode* next = NULL;
    OrderedSetNode* preds[MAXLEVEL];
    OrderedSetNode* succs[MAXLEVEL];

    node = head;
    for (i = node->toplevel-1; i >= 0; i--) {
        next = node->next[i];
        while (next->timestamp < old_ts) {
            node = next;
            next = node->next[i];
        }
        preds[i] = node;
        succs[i] = node->next[i];
    }

    if (next->timestamp != old_ts) {
        return 0;
    }

    for (i = 0; i < head->toplevel; i++) {
        if (succs[i]->timestamp == old_ts) {
            preds[i]->next[i] = succs[i]->next[i];
        }
    }

    OrderedSetNode* reinsert = next;

    reinsert->timestamp = reinsert->version->timestamp;
    Timestamp ts = reinsert->timestamp;

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
    if (result != 1) {
        return 0;
    }
    for (i = 0; i < reinsert->toplevel; i++) {
        reinsert->next[i] = succs[i];
        preds[i]->next[i] = reinsert;
    }
    return result;

}

size_t VersionSkiplist::size() {
    return sz;
}
