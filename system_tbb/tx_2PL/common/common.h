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

#ifndef _COMMON_H_
#define _COMMON_H_

#include "DataService.h"
#include <cstdint>
#include <string>
#include <vector>
#include <utility>

typedef int64_t TransactionId;

typedef std::string Key;
typedef std::string Value;

typedef struct DS{
    std::string host;
    int port;
}DataServerInfo;

const std::string NULL_VALUE = "_";

#endif