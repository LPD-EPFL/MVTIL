/**
 * Autogenerated by Thrift Compiler (0.10.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef DataService_TYPES_H
#define DataService_TYPES_H

#include "common.h"
#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>


namespace TxProtocol {

struct Operation {
  enum type {
    READ = 1,
    WRITE = 2
  };
};

extern const std::map<int, const char*> _Operation_VALUES_TO_NAMES;

struct OperationState {
  enum type {
    FAIL_NO_VERSION = 1,
    FAIL_READ_MARK_LARGE = 2,
    FAIL_INTERSECTION_EMPTY = 3,
    FAIL_TIMEOUT = 4,
    WRITES_NOT_FOUND = 5,
    R_LOCK_SUCCESS = 6,
    W_LOCK_SUCCESS = 7,
    COMMIT_OK = 8,
    ABORT_OK = 9,
    ERROR = 10,
    NOT_IMPLEMENTED = 11,
    FAIL_PENDING_VERSION = 12
  };
};

extern const std::map<int, const char*> _OperationState_VALUES_TO_NAMES;

//typedef int64_t TransactionId;

//typedef int64_t Timestamp;

//typedef std::string Key;

//typedef std::string Value;

class TimestampInterval;

class ClientGenericRequest;

class ServerGenericReply;

class ReadReply;

class WriteReply;

class CommitReply;

class AbortReply;

class GCReply;

typedef struct _TimestampInterval__isset {
  _TimestampInterval__isset() : lock_start(false), start(false), finish(false) {}
  bool lock_start :1;
  bool start :1;
  bool finish :1;
} _TimestampInterval__isset;

class TimestampInterval : public virtual ::apache::thrift::TBase {
 public:

  TimestampInterval(const TimestampInterval&);
  TimestampInterval& operator=(const TimestampInterval&);
  TimestampInterval() : lock_start(0), start(0), finish(0) {
  }

  virtual ~TimestampInterval() throw();
  Timestamp lock_start;
  Timestamp start;
  Timestamp finish;

  _TimestampInterval__isset __isset;

  void __set_lock_start(const Timestamp val);

  void __set_start(const Timestamp val);

  void __set_finish(const Timestamp val);

  bool operator == (const TimestampInterval & rhs) const
  {
    if (!(lock_start == rhs.lock_start))
      return false;
    if (!(start == rhs.start))
      return false;
    if (!(finish == rhs.finish))
      return false;
    return true;
  }
  bool operator != (const TimestampInterval &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TimestampInterval & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(TimestampInterval &a, TimestampInterval &b);

inline std::ostream& operator<<(std::ostream& out, const TimestampInterval& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _ClientGenericRequest__isset {
  _ClientGenericRequest__isset() : tid(false), op(false), interval(false), key(false), value(false) {}
  bool tid :1;
  bool op :1;
  bool interval :1;
  bool key :1;
  bool value :1;
} _ClientGenericRequest__isset;

class ClientGenericRequest : public virtual ::apache::thrift::TBase {
 public:

  ClientGenericRequest(const ClientGenericRequest&);
  ClientGenericRequest& operator=(const ClientGenericRequest&);
  ClientGenericRequest() : tid(0), op((Operation::type)0), interval(0), key(), value() {
  }

  virtual ~ClientGenericRequest() throw();
  TransactionId tid;
  Operation::type op;
  Timestamp interval;
  Key key;
  Value value;

  _ClientGenericRequest__isset __isset;

  void __set_tid(const TransactionId val);

  void __set_op(const Operation::type val);

  void __set_interval(const Timestamp val);

  void __set_key(const Key& val);

  void __set_value(const Value& val);

  bool operator == (const ClientGenericRequest & rhs) const
  {
    if (!(tid == rhs.tid))
      return false;
    if (!(op == rhs.op))
      return false;
    if (!(interval == rhs.interval))
      return false;
    if (!(key == rhs.key))
      return false;
    if (!(value == rhs.value))
      return false;
    return true;
  }
  bool operator != (const ClientGenericRequest &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ClientGenericRequest & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ClientGenericRequest &a, ClientGenericRequest &b);

inline std::ostream& operator<<(std::ostream& out, const ClientGenericRequest& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _ServerGenericReply__isset {
  _ServerGenericReply__isset() : tid(false), op(false), state(false), interval(false), key(false), value(false) {}
  bool tid :1;
  bool op :1;
  bool state :1;
  bool interval :1;
  bool key :1;
  bool value :1;
} _ServerGenericReply__isset;

class ServerGenericReply : public virtual ::apache::thrift::TBase {
 public:

  ServerGenericReply(const ServerGenericReply&);
  ServerGenericReply& operator=(const ServerGenericReply&);
  ServerGenericReply() : tid(0), op((Operation::type)0), state((OperationState::type)0), key(), value() {
  }

  virtual ~ServerGenericReply() throw();
  TransactionId tid;
  Operation::type op;
  OperationState::type state;
  TimestampInterval interval;
  std::string key;
  std::string value;

  _ServerGenericReply__isset __isset;

  void __set_tid(const TransactionId val);

  void __set_op(const Operation::type val);

  void __set_state(const OperationState::type val);

  void __set_interval(const TimestampInterval& val);

  void __set_key(const std::string& val);

  void __set_value(const std::string& val);

  bool operator == (const ServerGenericReply & rhs) const
  {
    if (!(tid == rhs.tid))
      return false;
    if (!(op == rhs.op))
      return false;
    if (!(state == rhs.state))
      return false;
    if (!(interval == rhs.interval))
      return false;
    if (!(key == rhs.key))
      return false;
    if (!(value == rhs.value))
      return false;
    return true;
  }
  bool operator != (const ServerGenericReply &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ServerGenericReply & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ServerGenericReply &a, ServerGenericReply &b);

inline std::ostream& operator<<(std::ostream& out, const ServerGenericReply& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _ReadReply__isset {
  _ReadReply__isset() : tid(false), interval(false), state(false), key(false), value(false) {}
  bool tid :1;
  bool interval :1;
  bool state :1;
  bool key :1;
  bool value :1;
} _ReadReply__isset;

class ReadReply : public virtual ::apache::thrift::TBase {
 public:

  ReadReply(const ReadReply&);
  ReadReply& operator=(const ReadReply&);
  ReadReply() : tid(0), state((OperationState::type)0), key(), value() {
  }

  virtual ~ReadReply() throw();
  TransactionId tid;
  TimestampInterval interval;
  OperationState::type state;
  Key key;
  Value value;

  _ReadReply__isset __isset;

  void __set_tid(const TransactionId val);

  void __set_interval(const TimestampInterval& val);

  void __set_state(const OperationState::type val);

  void __set_key(const Key& val);

  void __set_value(const Value& val);

  bool operator == (const ReadReply & rhs) const
  {
    if (!(tid == rhs.tid))
      return false;
    if (!(interval == rhs.interval))
      return false;
    if (!(state == rhs.state))
      return false;
    if (!(key == rhs.key))
      return false;
    if (!(value == rhs.value))
      return false;
    return true;
  }
  bool operator != (const ReadReply &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ReadReply & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ReadReply &a, ReadReply &b);

inline std::ostream& operator<<(std::ostream& out, const ReadReply& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _WriteReply__isset {
  _WriteReply__isset() : tid(false), interval(false), state(false), key(false) {}
  bool tid :1;
  bool interval :1;
  bool state :1;
  bool key :1;
} _WriteReply__isset;

class WriteReply : public virtual ::apache::thrift::TBase {
 public:

  WriteReply(const WriteReply&);
  WriteReply& operator=(const WriteReply&);
  WriteReply() : tid(0), state((OperationState::type)0), key() {
  }

  virtual ~WriteReply() throw();
  TransactionId tid;
  TimestampInterval interval;
  OperationState::type state;
  Key key;

  _WriteReply__isset __isset;

  void __set_tid(const TransactionId val);

  void __set_interval(const TimestampInterval& val);

  void __set_state(const OperationState::type val);

  void __set_key(const Key& val);

  bool operator == (const WriteReply & rhs) const
  {
    if (!(tid == rhs.tid))
      return false;
    if (!(interval == rhs.interval))
      return false;
    if (!(state == rhs.state))
      return false;
    if (!(key == rhs.key))
      return false;
    return true;
  }
  bool operator != (const WriteReply &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const WriteReply & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(WriteReply &a, WriteReply &b);

inline std::ostream& operator<<(std::ostream& out, const WriteReply& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _CommitReply__isset {
  _CommitReply__isset() : tid(false), state(false) {}
  bool tid :1;
  bool state :1;
} _CommitReply__isset;

class CommitReply : public virtual ::apache::thrift::TBase {
 public:

  CommitReply(const CommitReply&);
  CommitReply& operator=(const CommitReply&);
  CommitReply() : tid(0), state((OperationState::type)0) {
  }

  virtual ~CommitReply() throw();
  TransactionId tid;
  OperationState::type state;

  _CommitReply__isset __isset;

  void __set_tid(const TransactionId val);

  void __set_state(const OperationState::type val);

  bool operator == (const CommitReply & rhs) const
  {
    if (!(tid == rhs.tid))
      return false;
    if (!(state == rhs.state))
      return false;
    return true;
  }
  bool operator != (const CommitReply &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const CommitReply & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(CommitReply &a, CommitReply &b);

inline std::ostream& operator<<(std::ostream& out, const CommitReply& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _AbortReply__isset {
  _AbortReply__isset() : tid(false), state(false) {}
  bool tid :1;
  bool state :1;
} _AbortReply__isset;

class AbortReply : public virtual ::apache::thrift::TBase {
 public:

  AbortReply(const AbortReply&);
  AbortReply& operator=(const AbortReply&);
  AbortReply() : tid(0), state((OperationState::type)0) {
  }

  virtual ~AbortReply() throw();
  TransactionId tid;
  OperationState::type state;

  _AbortReply__isset __isset;

  void __set_tid(const TransactionId val);

  void __set_state(const OperationState::type val);

  bool operator == (const AbortReply & rhs) const
  {
    if (!(tid == rhs.tid))
      return false;
    if (!(state == rhs.state))
      return false;
    return true;
  }
  bool operator != (const AbortReply &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const AbortReply & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(AbortReply &a, AbortReply &b);

inline std::ostream& operator<<(std::ostream& out, const AbortReply& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _GCReply__isset {
  _GCReply__isset() : state(false) {}
  bool state :1;
} _GCReply__isset;

class GCReply : public virtual ::apache::thrift::TBase {
 public:

  GCReply(const GCReply&);
  GCReply& operator=(const GCReply&);
  GCReply() : state((OperationState::type)0) {
  }

  virtual ~GCReply() throw();
  OperationState::type state;

  _GCReply__isset __isset;

  void __set_state(const OperationState::type val);

  bool operator == (const GCReply & rhs) const
  {
    if (!(state == rhs.state))
      return false;
    return true;
  }
  bool operator != (const GCReply &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const GCReply & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(GCReply &a, GCReply &b);

inline std::ostream& operator<<(std::ostream& out, const GCReply& obj)
{
  obj.printTo(out);
  return out;
}

} // namespace

#endif
