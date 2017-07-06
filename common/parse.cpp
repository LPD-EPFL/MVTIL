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
	printf("\t-Pf		  ; LOCK_FIRST_INTERVAL\n");
	printf("\t-Pl		  ; LOCK_LAST_INTERVAL\n");
	printf("\t-gcINT      ; GABARGE_COLLECT\n");
	printf("\t-oINT       ; OPTIMIZATION\n");
}

void print_client_usage() {
	printf("[client usage]:\n");
	printf("\t-cINT       ; THREAD_CNT\n");
	printf("\t-kINT       ; KEY_SPACE\n");
	printf("\t-dINT       ; INTERVAL_DURATION (in ms)\n");
	printf("\t-tINT       ; TEST_TRANSACTIONS_TYPE\n");
	printf("\t-lINT       ; TEST_DURATION (in ms)\n");
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
		else if (argv[i][1] == 'P') {
			if (argv[i][2] == 'f')
				s_policy = LOCK_FIRST_INTERVAL;
			if (argv[i][2] == 'l')
				s_policy = LOCK_LAST_INTERVAL;
		}
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
		else if (argv[i][1] == 'd')
			c_interval_duration = atoi( &argv[i][2] );
		else if (argv[i][1] == 't')
			c_test_type = atoi( &argv[i][2] );
		else{
			print_client_usage();
			exit(0);
		}
	}
	
}
