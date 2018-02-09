#include "TransactionManager.h"

#include <iostream>
#include <string>

using boost::shared_ptr;
using namespace std;
using namespace ::TxProtocol;

int main(int argc, char **argv) {

  TransactionManager* manager = new TransactionManager(1);
  Transaction* tx1 = manager->StartTransaction();
  Transaction* tx2 = manager->StartTransaction();
  string value;
  manager->WriteData(tx2,"t","99");
  manager->ReadData(tx1,"k",value);
  cout<<value<<endl;
  manager->WriteData(tx1,"k","123");
  manager->ReadData(tx1,"k",value);
  cout<<value<<endl;
  manager->ReadData(tx1,"t",value);
  cout<<value<<endl;
  manager->WriteData(tx1,"t","456");
  manager->ReadData(tx2,"k",value);
  cout<<value<<endl;
  manager->WriteData(tx2,"k","23");
  manager->CommitTransaction(tx1);
  manager->CommitTransaction(tx2);
  return 0;
}

