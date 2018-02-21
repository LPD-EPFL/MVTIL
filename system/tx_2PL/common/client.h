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

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"
#include "config.h"

//Gobal Parameter for Clients
extern uint32_t c_thread_cnt;
extern uint32_t c_key_space;
extern uint32_t c_test_type;
extern uint32_t c_test_duration;
extern uint32_t c_test_read;
extern uint32_t c_test_write;
extern uint32_t c_stype;
extern uint32_t c_restart;
extern uint32_t c_id;

#endif