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
#include "client.h"

void print_server_usage() {
	printf("[server usage]:\n");
	printf("\t-cINT       ; THREAD_CNT\n");
	printf("\t-pINT       ; PORT\n");
	printf("\t-tINT       ; TIMEOUT (in ms)\n");
	printf("\t-Sb   	  ; BLOCKING_SERVER\n");
	printf("\t-Sn	      ; NON_BLOCKING_SERVER\n");
	printf("\t-rINT       ; MIN_NUM_RETRIES\n");
}

void print_client_usage() {
	printf("[client usage]:\n");
	printf("\t-cINT       ; THREAD_CNT\n");
	printf("\t-kINT       ; KEY_SPACE\n");
	printf("\t-tINT       ; TEST_TRANSACTIONS_TYPE\n");
	printf("\t-lINT       ; TEST_DURATION (in ms)\n");
	printf("\t-iINT       ; CLIENT_ID\n");
	printf("\t-rINT       ; READ_TIMES\n");
	printf("\t-wINT       ; WRITE_TIMES\n");
}

void parser_server(int argc, char * argv[]) {

	for (int i = 1; i < argc; i++) {
		assert(argv[i][0] == '-');
		if (argv[i][1] == 'c')
			s_thread_cnt = atoi( &argv[i][2] );
		else if (argv[i][1] == 'p')
			s_port = atoi( &argv[i][2] );
		else if (argv[i][1] == 't')
			s_timeout = atoi( &argv[i][2] );
		else if (argv[i][1] == 'r')
			s_retry = atoi( &argv[i][2] );
		else if (argv[i][1] == 'S') {
			if (argv[i][2] == 'b')
				s_type = BLOCKING_SERVER;
			if (argv[i][2] == 'n')
				s_type = NON_BLOCKING_SERVER;
		}
		else{
			print_server_usage();
			exit(0);
		}
	}
	
}

void parser_client(int argc, char * argv[]) {

	for (int i = 1; i < argc; i++) {
		assert(argv[i][0] == '-');
		if (argv[i][1] == 'c')
			c_thread_cnt = atoi( &argv[i][2] );
		else if (argv[i][1] == 'k')
			c_key_space = atoi( &argv[i][2] );
		else if (argv[i][1] == 't')
			c_test_type = atoi( &argv[i][2] );
		else if (argv[i][1] == 'l')
			c_test_duration = atoi( &argv[i][2] );
		else if (argv[i][1] == 'i')
			c_id = atoi( &argv[i][2] );
		else if (argv[i][1] == 'r')
			c_test_read = atoi( &argv[i][2] );
		else if (argv[i][1] == 'w')
			c_test_write = atoi( &argv[i][2] );
		else{
			print_client_usage();
			exit(0);
		}
	}
	
}
