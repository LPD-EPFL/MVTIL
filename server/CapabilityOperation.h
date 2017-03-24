
#ifndef _CAPABILITY_OPERATION_H_
#define _CAPABILITY_OPERATION_H_

typedef enum {READ, WRITE} LockOperation;

namespace
{
	static bool TestConflict(LockOperation oper1, LockOperation oper2){
		if(oper1 == LockOperation::READ && oper2 == LockOperation::READ)
			return false;
		else
			return true;
	}
}

#endif