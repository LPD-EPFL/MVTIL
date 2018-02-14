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

#ifndef _COMMON_H
#define _COMMON_H

// #include <cstdint>
// #include <vector>
#include <string>
#include <cstdint>

#define MIN_TIMESTAMP 0
#define MAX_TIMESTAMP INT64_MAX

typedef int64_t Timestamp;
typedef int64_t TransactionId;

typedef std::string Key;
typedef std::string Value;

#endif
