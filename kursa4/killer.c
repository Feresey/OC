// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <zmq.h>
#include <assert.h>
#include "game.h"
#include <stdlib.h>
#include<stdio.h>
int main(int argc, char **argv)
{
	void *ctx = zmq_ctx_new();
	void *serv = zmq_socket(ctx,ZMQ_REQ);
	char a[30];
	sprintf(a,"%s%d", "tcp://localhost:",argc > 1 ?atoi(argv[1]): 1234);
	int rc = zmq_connect(serv,a);
	assert(rc ==0);
	Message_t m;
	m.id = m.point = m.size = 0;
	turn_t t = {0,0,0};
	m.turn = t;
	zmq_send(serv,&m,sizeof(Message_t),0);

	zmq_close(serv);
	zmq_ctx_destroy(ctx);
}
