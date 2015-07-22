
#include <set>

typedef enum {COMMITTED, PENDING, ABORTED} op_state;
typedef enum {OP_READ, OP_WRITE} op_type;

class VersionManager {
    public:
        VersionManager();
        VersionManager(Log* log); //recover version manager from existing log
        ~VersionManager();

        Version getVersion(Key k, TimestampInterval interval, OpType flag);
        LockInfo getReadLock(Version v, TimestampInterval interval);
        LockInfo getWriteLock(Version v, TimestampInterval interval);
        //TODO pointer to version instead?

    private:
        Log* log;
        std::map<Key, VersionManagerEntry> versionStore;

        add_to_log(LogKey* k, Version* v);

        class VersionManagerEntry {
            public:
                lockEntry();

            private:
                Key key;
                Lock lock;
                std::set<Version> versions;
                std::set<Version> pending;
        }

        //TODO overload the <>= operators; essentially add a comparator
        

}
