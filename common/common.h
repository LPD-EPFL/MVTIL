
#ifndef _COMMON_H
#define _COMMON_H

// #include <cstdint>
// #include <vector>
#include <string>

#define MIN_TIMESTAMP 0
#define MAX_TIMESTAMP 0xFFFFFFFFFFFFFFFF

typedef int64_t Timestamp;
typedef int64_t TransactionId;

typedef std::string Key;
typedef std::string Value;

// typedef struct TI{
// 	Timestamp start;
// 	Timestamp end;
// 	bool operator==(const struct TI &hs) const{
// 	    return (start == hs.start)&&(end == hs.end);
// 	}
// 	bool operator != (const struct TI &hs) const {
// 	    return (start != hs.start)||(end != hs.end);
// 	}
// 	bool operator < (const struct TI & ) const;
// 	virtual void printTo(std::ostream& out) const;
// }TimestampInterval;


// typedef struct _TimestampInterval__isset {
//   _TimestampInterval__isset() : start(false), finish(false) {}
//   bool start :1;
//   bool finish :1;
// } _TimestampInterval__isset;


// class TimestampInterval : public virtual ::apache::thrift::TBase {
//  public:

//   TimestampInterval(const TimestampInterval&);
//   TimestampInterval& operator=(const TimestampInterval&);
//   TimestampInterval() : start(0), finish(0) {}

//   virtual ~TimestampInterval() throw();
//   Timestamp start;
//   Timestamp finish;

//   _TimestampInterval__isset __isset;

//   void __set_start(const Timestamp val);

//   void __set_finish(const Timestamp val);

//   bool operator == (const TimestampInterval & rhs) const
//   {
//     if (!(start == rhs.start))
//       return false;
//     if (!(finish == rhs.finish))
//       return false;
//     return true;
//   }
//   bool operator != (const TimestampInterval &rhs) const {
//     return !(*this == rhs);
//   }

//   bool operator < (const TimestampInterval & ) const;

//   uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
//   uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

//   virtual void printTo(std::ostream& out) const;
// };


// typedef struct LE 
// {
// 	Value value;
// 	TimestampInterval interval;
// }LockEntry;


// typedef struct DS{
//     std::string host;
//     int port;
// }DataServerInfo;

#endif
