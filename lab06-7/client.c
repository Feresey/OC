// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

#include "message.h"

#define actions "1) Put money\n\
2) Get money\n\
3) Send money to another account\n\
4) Check balance\n\
5) Leave the bank\n"

void* SendMessage(void* arg)
{
    Message_t* data = (Message_t*)arg;
    zmq_msg_t message;

    zmq_msg_init_size(&message, sizeof(Message_t));
    memcpy(zmq_msg_data(&message), data, sizeof(Message_t));
    zmq_msg_send(&message, data->requester, 0);
    zmq_msg_close(&message);

    zmq_msg_init(&message);
    zmq_msg_recv(&message, data->requester, 0);
    data = (Message_t*)zmq_msg_data(&message);

    printf("%s\n", data->message);
    zmq_msg_close(&message);
    return 0;
}

int main()
{
    void* context = zmq_ctx_new();
    void* server = zmq_socket(context, ZMQ_REQ);

    ID client;
    int bank;
    char adress[25];

    printf("Enter client's login: ");
    scanf("%lf", &client);

    printf("Enter bank's adress: ");
    scanf("%d", &bank);

    sprintf(adress, "%s%d", "tcp://localhost:", bank);
    zmq_connect(server, adress);
    printf("\nConnected to %s\n\n", adress);

    puts(actions);
    int choice = 0, summ = 0;

    while (scanf("%d", &choice)) {

        Message_t data;
        data.action = choice;
        data.client = client;

        switch (choice) {
        case 1: {
            printf("Enter the summ: ");
            scanf("%d", &summ);
            data.sum = summ;
            data.requester = server;

            SendMessage(&data);
            break;
        }
        case 2: {
            printf("Enter the summ: ");
            scanf("%d", &summ);
            data.sum = summ;
            data.requester = server;

            SendMessage(&data);
            break;
        }
        case 3: {
            ID receiverClient;
            printf("Enter receiver id: ");
            scanf("%lf", &receiverClient);
            printf("Enter the summ: ");
            scanf("%d", &summ);

            data.sum = summ;
            data.receiverClient = receiverClient;

            data.requester = server;
            SendMessage(&data);
            break;
        }
        case 4: {
            data.requester = server;

            SendMessage(&data);
            break;
        }
        case 5:
            goto end;
        default: {
            puts("Inccorect command");
        }
        }
    }

end:
    zmq_close(server);
    zmq_ctx_destroy(context);
    return 0;
}