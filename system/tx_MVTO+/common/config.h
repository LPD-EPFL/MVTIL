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

#ifndef _CONFIG_H_
#define _CONFIG_H_

//Server
#define S_THREAD_CNT			50
#define PORT					6060

#define S_TYPE					NON_BLOCKING_SERVER
#define BLOCKING_SERVER			1
#define NON_BLOCKING_SERVER		2

#define DEFAULT_TIMEOUT			10
#define MIN_NUM_RETRIES			5

//Client
#define MAX_RESTART				4
#define C_THREAD_CNT			100
#define KEY_SPACE				8000
#define TEST_TYPE				MIX
#define CLIENT_ID				0

//Test
#define TEST_DURATION_MS		10000
#define MAX_NUM_THREADS			5000
#define RO_SIZE					20
#define RW_SIZE					10

typedef enum {READ_ONLY, READ_INTENSIVE, WRITE_INTENSIVE, RW, RW_SHORT, MIX, RW_CONFLICT} TransactionType;
//typedef enum {RW_CONFLICT} TransactionType;

#endif
