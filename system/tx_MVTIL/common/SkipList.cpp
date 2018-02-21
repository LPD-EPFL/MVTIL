// #include "SkipList.h"
// #define SKIP_LIST_MAXLEVEL 4

// int main(){
// 	skiplist<int,int> list(1,1000);
// 	list.insert(2,3);
// 	list.insert(3,4);
// 	list.insert(4,5);
// 	list.insert(6,6);
// 	list.insert(6,4);
// 	list.printList();
// 	cout<<""<<endl;
// 	list.remove(4);
// 	list.printList();
// 	cout<<""<<endl;
// 	list.erase(5);
// 	list.printList();
// 	cout<<""<<endl;
// 	skiplist_node<int,int,SKIP_LIST_MAXLEVEL> *node1;
// 	skiplist_node<int,int,SKIP_LIST_MAXLEVEL> *node2;
// 	list.insert(3,2);
// 	list.printList();
// 	node2 = list.find(4,node1);
// 	cout<< node1->value <<" "<< node2->value <<endl;
// 	return 0;
// }