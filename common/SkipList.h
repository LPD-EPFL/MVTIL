
#ifndef _SKIP_LIST_H_
#define _SKIP_LIST_H_

#include "random.h"
#include <cstddef>

#include <string>
#include <iostream>

//#define SKIP_LIST_MAXLEVEL 4

using namespace std;

template <class K,class V,int MAXLEVEL>

class skiplist_node{

	public:
		K key;
		V value;
        int top_level;
		skiplist_node<K,V,MAXLEVEL> *next[MAXLEVEL];

        skiplist_node(K searchKey):key(searchKey),top_level(MAXLEVEL)
        {
            for ( int i=0; i<MAXLEVEL; i++ ) {
                next[i] = NULL;
            }
        }

	    skiplist_node(K searchKey,V val):key(searchKey),value(val),top_level(MAXLEVEL)
        {
	        for ( int i=0; i<MAXLEVEL; i++ ) {
	            next[i] = NULL;
	        }
	    }

	    skiplist_node(K searchKey,V val, int topLevel):key(searchKey),value(val),top_level(topLevel)
        {
	        for ( int i=0; i<MAXLEVEL; i++ ) {
	            next[i] = NULL;
	        }
	    }
	 
	    virtual ~skiplist_node(){
	    }
};


template<class K, class V, int MAXLEVEL = SKIP_LIST_MAXLEVEL>
class skiplist
{
public:
    typedef K KeyType;
    typedef V ValueType;
    typedef skiplist_node<K,V,MAXLEVEL> NodeType;
 
    skiplist(K minKey,K maxKey):head(NULL),max_level(MAXLEVEL),
                                min_key(minKey),max_key(maxKey)
    {
        head = new NodeType(minKey);
        NodeType *tail = new NodeType(maxKey);
        for ( int i=0; i<MAXLEVEL; i++ ) {
            head->next[i] = tail;
        }
        seeds = seed_rand();
    }
 
    virtual ~skiplist()
    {
        NodeType *node = head;
        NodeType *next;
        while(node != NULL){
            next = node -> next[0];
            delete node;
            node = next;
        }
    }

    NodeType* find(K searchKey,NodeType* &prev)
    {
        NodeType* node = head;
        for(int level=node->top_level-1; level >=0; level--) {
            while (node->next[level] != NULL && node->next[level]->key <= searchKey ) {
                node = node->next[level];
            }
        }
        prev = node;
        return node->next[0];
    }

    int insert(K searchKey,V value){
        NodeType* node = head;
        for(int level=node->top_level-1; level >=0; level--) {
            while (node->next[level] != NULL && node->next[level]->key <= searchKey ) {
                node = node->next[level];
            }
        }

        int result = (node->key!=searchKey);
        if(result == 1)
        {
            int level = GetRandomLevel();
            NodeType* insertNode = new NodeType(searchKey,value,level);
            NodeType* nextNode = node->next[0];
            for(int i=0;i<node->top_level;i++){
                node->next[i] = insertNode;
            }
            for(int i=0;i<level;i++){
                insertNode->next[i] = nextNode;
            }
        }
        return result;
    }

    int remove(K searchKey){
        NodeType* node = head;
        for(int level=node->top_level-1; level >=0; level--) {
            while (node->next[level] != NULL && node->next[level]->key < searchKey ) {
                node = node->next[level];
            }
        }

        NodeType* nextNode = node->next[0];
        int result = (nextNode->key==searchKey);
        if(result == 1)
        {
            for(int i=0;i<node->top_level;i++){
                node->next[i] = nextNode->next[0];
            }
            delete nextNode;
        }
        return result;
    }

    void erase(K searchKey){
        NodeType* node = head->next[0];
        while (node->next[0] != NULL && node->key <= searchKey ) {
            NodeType* cur = node;
            node = node->next[0];
            delete cur;
        }
        for(int i=0;i<MAXLEVEL;i++){
            head->next[i] = node;
        }
    }

    void SetProb(int probability){
       prob =  probability;
    }

    void printList()
    {
        NodeType* currNode = head->next[0];
        while ( currNode->next[0] != NULL ) {
            cout << "(" << currNode->key << "," << currNode->value << ")" << endl;
            currNode = currNode->next[0];
        }
        return;
    }

private:
    K min_key;
    K max_key;
    int max_level;
    NodeType *head;
    unsigned long* seeds;
    double prob = 0.5;

    int GetRandomLevel() {
        int level = 1;        
        while ( rand_range(101) < 100*prob && level < MAXLEVEL ) {
            level++;
        }
        return level;
    }
};

#endif




