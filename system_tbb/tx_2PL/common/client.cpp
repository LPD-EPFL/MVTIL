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

#include "client.h"

uint32_t c_thread_cnt = C_THREAD_CNT;
uint32_t c_key_space = KEY_SPACE;
uint32_t c_test_type = TEST_TYPE;
uint32_t c_test_duration = TEST_DURATION_MS;
uint32_t c_stype = S_TYPE;
uint32_t c_restart = MAX_RESTART;
uint32_t c_id = CLIENT_ID;
uint32_t c_test_read = RO_SIZE;
uint32_t c_test_write = RW_SIZE;