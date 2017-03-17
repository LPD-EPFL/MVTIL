#include "TransactionManager.h"

#include <iostream>
#include <string>
// #include <thrift/protocol/TBinaryProtocol.h>
// #include <thrift/transport/TSocket.h>
// #include <thrift/transport/TBufferTransports.h>

// using namespace apache::thrift;
// using namespace apache::thrift::protocol;
// using namespace apache::thrift::transport;
//using namespace ::apache::thrift::server;

using boost::shared_ptr;
using namespace std;
using namespace ::TxProtocol;

int main(int argc, char **argv) {
  // shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
  // shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  // shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

  // DataServerClient client(protocol);
  //transport->open();
  //client.doOperation();
  //transport->close();

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
  manager->CommitTransaction(tx1);
  manager->CommitTransaction(tx2);
  return 0;
}

