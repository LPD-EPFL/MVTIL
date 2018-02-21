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

#include "server.h"

uint32_t s_thread_cnt = S_THREAD_CNT;
uint32_t s_port = PORT;
uint32_t s_timeout = DEFAULT_TIMEOUT;
uint32_t s_retry = MIN_NUM_RETRIES;
uint32_t s_policy = MVTL_POLICY;
uint32_t s_type = S_TYPE;