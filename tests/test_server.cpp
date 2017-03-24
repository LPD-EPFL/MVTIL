#include "Scheduler.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

Scheduler scheduler;

void execute(string line){
	std::vector<std::string> commands;
	boost::split(commands, line, boost::is_any_of(" "));
	int xact_id = atoi(commands[0]);
	string operation = commands[1];
	if(operation == "Read")
	{
		string value;
		ReadReply reply;
		TimestampInterval interval;
		interval.start = atoi(commands[3]);
		interval.finish = atoi(commands[4]);
		scheduler.HandleReadRequest(reply,xact_id,interval,commands[2]);
		cout<<"Xact Id:"<<xact_id<<",Read Key:"<<commands[2]<<",Value:"<<value<<endl;
	}
	else if(operation == "Write")
	{
		cout<<"Xact Id:"<<xact_id<<",Write Key:"<<commands[2]<<",Value:"<<commands[3]<<endl;
		WriteReply reply;
		TimestampInterval interval;
		interval.start = atoi(commands[3]);
		interval.finish = atoi(commands[4]);
		scheduler.HandleWriteRequest(reply,xact_id,interval,commands[2],atoi(commands[5]));
	}
	else if(operation == "Commit")
	{
		cout<<"Xact Id:"<<xact_id<<", Commit"<<endl;
		CommitReply reply;
		scheduler.HandleCommit(reply,xact_id,atoi(commands[2]));
	}
	else if(operation == "Abort"){
		HandleAbort reply;
		cout<<"Xact Id:"<<xact_id<<", Abort"<<endl;
		scheduler.HandleAbort(reply,xact_id);
	}

}


int main(){
	std::fstream test_file("TestServer.txt"); 
	std::string line;
	while (std::getline(test_file, line))
	{
		execute(line);
	}
}