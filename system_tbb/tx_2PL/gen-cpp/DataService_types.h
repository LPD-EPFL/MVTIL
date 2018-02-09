/**
 * Autogenerated by Thrift Compiler (0.10.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef DataService_TYPES_H
#define DataService_TYPES_H

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
    R_LOCK_SUCCESS = 2,
    W_LOCK_SUCCESS = 3,
    FAIL_PENDING_LOCK = 4,
    FAIL_TIMEOUT = 5,
    COMMIT_OK = 6,
    ABORT_OK = 7,
    ERROR = 8
  };
};

extern const std::map<int, const char*> _OperationState_VALUES_TO_NAMES;

typedef int64_t TransactionId;

typedef int64_t Timestamp;

typedef std::string Key;

typedef std::string Value;

class ReadReply;

class WriteReply;

class CommitReply;

class AbortReply;

typedef struct _ReadReply__isset {
  _ReadReply__isset() : tid(false), state(false), key(false), value(false) {}
  bool tid :1;
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
  OperationState::type state;
  Key key;
  Value value;

  _ReadReply__isset __isset;

  void __set_tid(const TransactionId val);

  void __set_state(const OperationState::type val);

  void __set_key(const Key& val);

  void __set_value(const Value& val);

  bool operator == (const ReadReply & rhs) const
  {
    if (!(tid == rhs.tid))
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
  _WriteReply__isset() : tid(false), state(false), key(false) {}
  bool tid :1;
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
  OperationState::type state;
  Key key;

  _WriteReply__isset __isset;

  void __set_tid(const TransactionId val);

  void __set_state(const OperationState::type val);

  void __set_key(const Key& val);

  bool operator == (const WriteReply & rhs) const
  {
    if (!(tid == rhs.tid))
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

} // namespace

#endif
