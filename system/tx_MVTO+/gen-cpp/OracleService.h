/**
 * Autogenerated by Thrift Compiler (0.10.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef OracleService_H
#define OracleService_H

#include <thrift/TDispatchProcessor.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include "OracleService_types.h"

namespace Oracle {

#ifdef _WIN32
  #pragma warning( push )
  #pragma warning (disable : 4250 ) //inheriting methods via dominance 
#endif

class OracleServiceIf {
 public:
  virtual ~OracleServiceIf() {}
  virtual Timestamp GetTimestamp() = 0;
  virtual TransactionId GetTransactionId() = 0;
};

class OracleServiceIfFactory {
 public:
  typedef OracleServiceIf Handler;

  virtual ~OracleServiceIfFactory() {}

  virtual OracleServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(OracleServiceIf* /* handler */) = 0;
};

class OracleServiceIfSingletonFactory : virtual public OracleServiceIfFactory {
 public:
  OracleServiceIfSingletonFactory(const boost::shared_ptr<OracleServiceIf>& iface) : iface_(iface) {}
  virtual ~OracleServiceIfSingletonFactory() {}

  virtual OracleServiceIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(OracleServiceIf* /* handler */) {}

 protected:
  boost::shared_ptr<OracleServiceIf> iface_;
};

class OracleServiceNull : virtual public OracleServiceIf {
 public:
  virtual ~OracleServiceNull() {}
  Timestamp GetTimestamp() {
    Timestamp _return = 0;
    return _return;
  }
  TransactionId GetTransactionId() {
    TransactionId _return = 0;
    return _return;
  }
};


class OracleService_GetTimestamp_args {
 public:

  OracleService_GetTimestamp_args(const OracleService_GetTimestamp_args&);
  OracleService_GetTimestamp_args& operator=(const OracleService_GetTimestamp_args&);
  OracleService_GetTimestamp_args() {
  }

  virtual ~OracleService_GetTimestamp_args() throw();

  bool operator == (const OracleService_GetTimestamp_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const OracleService_GetTimestamp_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const OracleService_GetTimestamp_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class OracleService_GetTimestamp_pargs {
 public:


  virtual ~OracleService_GetTimestamp_pargs() throw();

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _OracleService_GetTimestamp_result__isset {
  _OracleService_GetTimestamp_result__isset() : success(false) {}
  bool success :1;
} _OracleService_GetTimestamp_result__isset;

class OracleService_GetTimestamp_result {
 public:

  OracleService_GetTimestamp_result(const OracleService_GetTimestamp_result&);
  OracleService_GetTimestamp_result& operator=(const OracleService_GetTimestamp_result&);
  OracleService_GetTimestamp_result() : success(0) {
  }

  virtual ~OracleService_GetTimestamp_result() throw();
  Timestamp success;

  _OracleService_GetTimestamp_result__isset __isset;

  void __set_success(const Timestamp val);

  bool operator == (const OracleService_GetTimestamp_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const OracleService_GetTimestamp_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const OracleService_GetTimestamp_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _OracleService_GetTimestamp_presult__isset {
  _OracleService_GetTimestamp_presult__isset() : success(false) {}
  bool success :1;
} _OracleService_GetTimestamp_presult__isset;

class OracleService_GetTimestamp_presult {
 public:


  virtual ~OracleService_GetTimestamp_presult() throw();
  Timestamp* success;

  _OracleService_GetTimestamp_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class OracleService_GetTransactionId_args {
 public:

  OracleService_GetTransactionId_args(const OracleService_GetTransactionId_args&);
  OracleService_GetTransactionId_args& operator=(const OracleService_GetTransactionId_args&);
  OracleService_GetTransactionId_args() {
  }

  virtual ~OracleService_GetTransactionId_args() throw();

  bool operator == (const OracleService_GetTransactionId_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const OracleService_GetTransactionId_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const OracleService_GetTransactionId_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class OracleService_GetTransactionId_pargs {
 public:


  virtual ~OracleService_GetTransactionId_pargs() throw();

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _OracleService_GetTransactionId_result__isset {
  _OracleService_GetTransactionId_result__isset() : success(false) {}
  bool success :1;
} _OracleService_GetTransactionId_result__isset;

class OracleService_GetTransactionId_result {
 public:

  OracleService_GetTransactionId_result(const OracleService_GetTransactionId_result&);
  OracleService_GetTransactionId_result& operator=(const OracleService_GetTransactionId_result&);
  OracleService_GetTransactionId_result() : success(0) {
  }

  virtual ~OracleService_GetTransactionId_result() throw();
  TransactionId success;

  _OracleService_GetTransactionId_result__isset __isset;

  void __set_success(const TransactionId val);

  bool operator == (const OracleService_GetTransactionId_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const OracleService_GetTransactionId_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const OracleService_GetTransactionId_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _OracleService_GetTransactionId_presult__isset {
  _OracleService_GetTransactionId_presult__isset() : success(false) {}
  bool success :1;
} _OracleService_GetTransactionId_presult__isset;

class OracleService_GetTransactionId_presult {
 public:


  virtual ~OracleService_GetTransactionId_presult() throw();
  TransactionId* success;

  _OracleService_GetTransactionId_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class OracleServiceClient : virtual public OracleServiceIf {
 public:
  OracleServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  OracleServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  Timestamp GetTimestamp();
  void send_GetTimestamp();
  Timestamp recv_GetTimestamp();
  TransactionId GetTransactionId();
  void send_GetTransactionId();
  TransactionId recv_GetTransactionId();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class OracleServiceProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<OracleServiceIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (OracleServiceProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_GetTimestamp(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_GetTransactionId(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  OracleServiceProcessor(boost::shared_ptr<OracleServiceIf> iface) :
    iface_(iface) {
    processMap_["GetTimestamp"] = &OracleServiceProcessor::process_GetTimestamp;
    processMap_["GetTransactionId"] = &OracleServiceProcessor::process_GetTransactionId;
  }

  virtual ~OracleServiceProcessor() {}
};

class OracleServiceProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  OracleServiceProcessorFactory(const ::boost::shared_ptr< OracleServiceIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< OracleServiceIfFactory > handlerFactory_;
};

class OracleServiceMultiface : virtual public OracleServiceIf {
 public:
  OracleServiceMultiface(std::vector<boost::shared_ptr<OracleServiceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~OracleServiceMultiface() {}
 protected:
  std::vector<boost::shared_ptr<OracleServiceIf> > ifaces_;
  OracleServiceMultiface() {}
  void add(boost::shared_ptr<OracleServiceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  Timestamp GetTimestamp() {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->GetTimestamp();
    }
    return ifaces_[i]->GetTimestamp();
  }

  TransactionId GetTransactionId() {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->GetTransactionId();
    }
    return ifaces_[i]->GetTransactionId();
  }

};

// The 'concurrent' client is a thread safe client that correctly handles
// out of order responses.  It is slower than the regular client, so should
// only be used when you need to share a connection among multiple threads
class OracleServiceConcurrentClient : virtual public OracleServiceIf {
 public:
  OracleServiceConcurrentClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  OracleServiceConcurrentClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  Timestamp GetTimestamp();
  int32_t send_GetTimestamp();
  Timestamp recv_GetTimestamp(const int32_t seqid);
  TransactionId GetTransactionId();
  int32_t send_GetTransactionId();
  TransactionId recv_GetTransactionId(const int32_t seqid);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
  ::apache::thrift::async::TConcurrentClientSyncInfo sync_;
};

#ifdef _WIN32
  #pragma warning( pop )
#endif

} // namespace

#endif
