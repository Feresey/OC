// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <zmq.h>

#include "bank.h"
#include "message.h"

#define Send_msg(DATA, MESSAGE, INFO, SERVER)                \
    memcpy(DATA.message, INFO, strlen(INFO) + 1);           \
    zmq_msg_init_size(&MESSAGE, sizeof(Message_t));          \
    memcpy(zmq_msg_data(&MESSAGE), &DATA, sizeof(Message_t)); \
    zmq_msg_send(&MESSAGE, SERVER, 0);                       \
    zmq_msg_close(&MESSAGE);

volatile sig_atomic_t flag = 0;

void sig_check(int sig)
{
    switch (sig) {
    case SIGINT: {
        if (flag == 0) {
            flag = sig;
            puts(" Server blocked!");
        } else if (flag == sig) {
            flag = 0;
            puts(" Server unblocked!");
        }
        break;
    }
    case SIGQUIT:
        flag = sig;
    }
}

int main()
{
    int code;
    ClientBase* clientBase = Bank_init();
    void* context = zmq_ctx_new();
    void* server = zmq_socket(context, ZMQ_REP);
    char adress[25];

login:

    printf("Enter bank's adress: ");
    int bank;
    scanf("%d", &bank);
    sprintf(adress, "%s%d", "tcp://*:", bank);

    if (zmq_bind(server, adress)) {
        puts("adress is invalid,try again:");
        goto login;
    }

    while (1) {
        signal(SIGINT, sig_check);
        signal(SIGQUIT, sig_check);
        if (flag == SIGQUIT)
            goto end;

        if (!flag) {
            zmq_msg_t message;

            zmq_msg_init(&message);
            zmq_msg_recv(&message, server, 0);
            Message_t data = *(Message_t*)zmq_msg_data(&message);
            zmq_msg_close(&message);

            char info[STR_SIZE];

            switch (data.action) {
            case 1: {
                printf("Put money into the account id: %lf\n", data.client);
                Bank_put_money(data.client, data.sum, clientBase);
                Bank_print_client_base(clientBase);

                Send_msg(data, message, "Operation was completed successfully\0", server);
                break;
            }

            case 2: {
                printf("Get money from the account id: %lf\n", data.client);

                code = Bank_get_money(data.client, data.sum, clientBase);
                if (code == SUCCESS) {
                    printf("Success\n");
                    Bank_print_client_base(clientBase);
                    strcpy(info, "Operation was completed successfully\0");
                } else if (code == NOT_ENOUGH_MONEY) {
                    printf("Not enough money\n");
                    strcpy(info, "You not enough money\0");
                } else {
                    printf("Not bank client\n");
                    strcpy(info, "You aren't a client of bank\0");
                }

                Send_msg(data, message, info, server);
                break;
            }

            case 3: {
                printf("Send money from account id: %lf to account id: %lf\n", data.client, data.receiverClient);
                code = Bank_send_money(data.client, data.receiverClient, data.sum, clientBase);
                if (code == SUCCESS) {
                    printf("Success\n");
                    Bank_print_client_base(clientBase);
                    strcpy(info, "Operation was completed successfully\0");
                } else if (code == NOT_ENOUGH_MONEY) {
                    printf("Not enought money\n");
                    strcpy(info, "You not enough money");
                } else if (code == RECEIVER_NOT_CLIENT) {
                    printf("Receiver not bank client\n");
                    strcpy(info, "Receiver is not a client of bank\0");
                }

                Send_msg(data, message, info, server);
                break;
            }

            case 4: {
                printf("Check account id: %lf\n", data.client);
                code = Bank_get_info(clientBase, data.client);
                if (code == NOT_CLIENT) {
                    printf("Not bank client\n");
                    strcpy(info, "You aren't client of bank\0");
                } else {
                    printf("Client sum is %d\n", code);
                    Bank_print_client_base(clientBase);
                    sprintf(info, "%s%d%c", "Your account is ", code, '\0');
                }

                Send_msg(data, message, info, server);
                break;
            }
            default:
                Send_msg(data, message, "Incorrect command", server);
            }
            zmq_msg_close(&message);
        } else
            sleep(1);
    }
end:
    zmq_close(server);
    zmq_ctx_destroy(context);

    Bank_destroy_database(clientBase);
    return 0;
}