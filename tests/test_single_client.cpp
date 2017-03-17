#include "TransactionManager.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
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

unordered_map<string,Transaction*> xact_map;

void execute(TransactionManager* manager, string line){
	std::vector<std::string> commands;
	boost::split(commands, line, boost::is_any_of(" "));
	string xact_id = commands[0];
	string operation = commands[1];
	if(operation == "Create")
	{
		cout<<"Xact Id:"<<xact_id<<", Create"<<endl;
		Transaction* t = manager->StartTransaction();
		xact_map[xact_id] = t;
	}
	else if(operation == "Read")
	{
		Transaction* t = xact_map[xact_id];
		string value;
		manager->ReadData(t,commands[2],value);
		cout<<"Xact Id:"<<xact_id<<",Read Key:"<<commands[2]<<",Value:"<<value<<endl;
	}
	else if(operation == "Write")
	{
		cout<<"Xact Id:"<<xact_id<<",Write Key:"<<commands[2]<<",Value:"<<commands[3]<<endl;
		Transaction* t = xact_map[xact_id];
		manager->WriteData(t,commands[2],commands[3]);
	}
	else if(operation == "Commit")
	{
		cout<<"Xact Id:"<<xact_id<<", Commit"<<endl;
		Transaction* t = xact_map[xact_id];
		manager->CommitTransaction(t);
	}
	else if(operation == "Abort"){
		cout<<"Xact Id:"<<xact_id<<", Abort"<<endl;
		Transaction* t = xact_map[xact_id];
		manager->AbortTransaction(t);
	}
}

int main(int argc, char **argv) {

	TransactionManager* manager = new TransactionManager(1);
	std::fstream test_file("TestSingle.txt"); 
	std::string line;
	while (std::getline(test_file, line))
	{
		execute(manager,line);
	}
}