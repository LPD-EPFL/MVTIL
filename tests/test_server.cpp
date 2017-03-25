#include "Scheduler.h"
#include "DataService.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
using namespace  ::TxProtocol;

std::shared_ptr<Scheduler> scheduler;

void execute(string line){
	std::vector<std::string> commands;
	boost::split(commands, line, boost::is_any_of(" "));
	int xact_id = stoi(commands[0]);
	string operation = commands[1];
	if(operation == "Read")
	{
		cout<<"Xact Id:"<<xact_id<<",Read Key:"<<commands[2]<<endl;
		string value;
		ReadReply reply;
		TimestampInterval interval;
		interval.start = stoi(commands[3]);
		interval.finish = stoi(commands[4]);
		scheduler->HandleReadRequest(reply,xact_id,interval,commands[2]);
		cout<<"-------------------"<<endl;
	}
	else if(operation == "Write")
	{
		cout<<"Xact Id:"<<xact_id<<",Write Key:"<<commands[2]<<",Value:"<<commands[5]<<endl;
		WriteReply reply;
		TimestampInterval interval;
		interval.start = stoi(commands[3]);
		interval.finish = stoi(commands[4]);
		scheduler->HandleWriteRequest(reply,xact_id,interval,commands[2],commands[5]);
		cout<<"-------------------"<<endl;
	}
	else if(operation == "Commit")
	{
		cout<<"Xact Id:"<<xact_id<<", Commit"<<endl;
		CommitReply reply;
		scheduler->HandleCommit(reply,xact_id,stoi(commands[2]));
		cout<<"-------------------"<<endl;
	}
	else if(operation == "Abort"){
		cout<<"Xact Id:"<<xact_id<<", Abort"<<endl;
		AbortReply reply;
		scheduler->HandleAbort(reply,xact_id);
		cout<<"-------------------"<<endl;
	}

}


int main(){
	std::fstream test_file("TestServer.txt"); 
	std::string line;
	scheduler = std::make_shared<Scheduler>();
	while (std::getline(test_file, line))
	{
		execute(line);
	}
}