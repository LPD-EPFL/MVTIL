#include "ClientReply.h"

using namespace std;

void ClientReply::dump() {
    cout<<"Transaction Id: "<<tid<<" ";
    std::string rt;
    switch(rtype) { 
        case READ_REPLY:
            rt = "READ_REPLY"; 
            break;
        case WRITE_REPLY:
            rt = "WRITE_REPLY"; 
            break;
        case COMMIT_ACK:
            rt = "COMMIT_ACK";
            break;
        case ABORT_ACK:
            rt = "ABORT_ACK";
            break;
        case HINT_REPLY:
            rt = "HINT_REPLY";
            break;
        case TIMEOUT:
            rt = "TIMEOUT";
            break;
        default:
            rt = "UNKOWN";
            break;
    }
    cout<<"Reply type: "<<rt<<" ";
    if (value == NULL) {
        cout<<"Value: NULL ";
    } else {
        cout<<"Value: "<<*value<<" ";
    }
    if (lockInfo == NULL) {
        cout<<"Lock info: NULL ";
    } else {
        std::string st;
        switch(lockInfo->state) { 
            case FAIL_NO_VERSION:
                st = "FAIL_NO_VERSION"; 
                break;
            case FAIL_READ_MARK_LARGE:
                st = "FAIL_READ_MARK_LARGE"; 
                break;
            case FAIL_INTERSECTION_EMPTY:
                st = "FAIL_INTERSECTION_EMPTY";
                break;
            case WRITES_NOT_FOUND:
                st = "WRITES_NOT_FOUND";
                break;
            case W_LOCK_SUCCESS:
                st = "W_LOCK_SUCCESS";
                break;
            case TIMEOUT:
                st = "R_LOCK_SUCCESS";
                break;
            default:
                st = "UNKOWN";
                break;
        }

        cout<<"Lock info: state "<<st<<" locked ["<<lockInfo->locked.start<<","<<lockInfo->locked.end<<"] potential ["<<lockInfo->potential.start<<","<<lockInfo->potential.end<<"] ";
        if (lockInfo->version == NULL) {
            cout<<"version NULL ";
        } else {
            std::string vt;
            switch(lockInfo->version->state) { 
                case COMMITTED:
                    vt = "COMMITTED"; 
                    break;
                case PENDING:
                    vt = "PENDING"; 
                    break;
                case ABORTED:
                    vt = "ABORTED";
                    break;
                case NO_SUCH_VERSION:
                    vt = "NO_SUCH_VERSION";
                    break;
                default:
                    vt = "UNKOWN";
                    break;
            }
            cout<<"version state "<<vt<<" timestamp "<<lockInfo->version->timestamp<<" duration "<<lockInfo->version->duration<<" maxReadFrom "<<lockInfo->version->maxReadFrom<<" ";
        }
    }
    cout<<endl;
}
