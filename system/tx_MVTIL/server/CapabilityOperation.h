/*   
 * Copyright (c) 2017 Junxiong Wang <junxiong.wang@epfl.ch>,
 *                Tudor David <tudor.david@epfl.ch>
 *                Distributed Programming Lab (LPD), EPFL
 *
 * MVTLSYS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _CAPABILITY_OPERATION_H_
#define _CAPABILITY_OPERATION_H_

typedef enum {READ, WRITE} LockOperation;

namespace
{
    static bool TestConflict(LockOperation oper1, LockOperation oper2){
        if(oper1 == LockOperation::READ && oper2 == LockOperation::READ)
            return false;
        else if(oper1 == LockOperation::WRITE && oper2 == LockOperation::WRITE)
            return false;
        else
            return true;
    }
}

#endif
