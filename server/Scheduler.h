
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "DataService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace ::TxProtocol;

class DataServiceHandler : virtual public DataServiceIf {
 public:
  DataServiceHandler() {
    // Your initialization goes here
  }

  void HandleAbort(AbortReply& _return, const TransactionId tid) {
    // Your implementation goes here
    printf("HandleAbort\n");
  }

  void HandleCommit(CommitReply& _return, const TransactionId tid, const Timestamp ts) {
    // Your implementation goes here
    printf("HandleCommit\n");
  }

  void HandleReadRequest(ReadReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k) {
    // Your implementation goes here
    printf("HandleReadRequest\n");
  }

  void HandleWriteRequest(WriteReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k, const Value& v) {
    // Your implementation goes here
    printf("HandleWriteRequest\n");
  }

  void HandleFreezeReadRequest(ReadReply& _return, const TransactionId tid, const TimestampInterval& interval, const Key& k) {
    // Your implementation goes here
    printf("HandleFreezeReadRequest\n");
  }

};

#endif
