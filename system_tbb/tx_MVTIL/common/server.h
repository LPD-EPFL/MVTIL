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

#ifndef _SERVER_H_
#define _SERVER_H_

#include "SkipList.h"
#include "DataService_types.h"
#include "config.h"

#include <unordered_map>

#define MAX_LEVEL 4

using namespace TxProtocol;

typedef OperationState::type LockState;
typedef skiplist<Timestamp,Value,MAX_LEVEL> VersionList;
typedef skiplist_node<Timestamp,Value,MAX_LEVEL> Version;

typedef struct LockInfo {
    LockState state; //the status the operation finished with
    TimestampInterval locked; //the interval that was successfully locked
    Version* version; //the version that was locked or created
} LockInfo;

//Gobal Parameter for Servers
extern uint32_t s_thread_cnt;
extern uint32_t s_port;
extern uint32_t s_timeout;
extern uint32_t s_retry;
extern uint32_t s_policy;
extern uint32_t s_type;

#endif