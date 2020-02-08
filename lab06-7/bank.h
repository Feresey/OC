#ifndef _BANK_H_
#define _BANK_H_

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "message.h"

#define SUCCESS 1
#define NOT_MEMORY -1
#define NOT_ENOUGH_MONEY -2
#define NOT_CLIENT -3
#define RECEIVER_NOT_CLIENT -4

typedef struct _client {
    ID id;
    int sum;
} Client;

typedef struct _client_base {
    Client client;
    struct _client_base* next;
} ClientBase;

ClientBase* Bank_init();
void Bank_add_client(ClientBase*, ID client);
void Bank_print_client_base(ClientBase*);
Client* _Bank_find_client(ClientBase*, double clientID);
void Bank_destroy_database(ClientBase*);
int Bank_destroy_account(ClientBase*, double clientID);

void Bank_put_money(double ClientID, int sum, ClientBase*);
int Bank_get_money(double ClientID, int sum, ClientBase*);
int Bank_send_money(ID clientSender, ID clientReceiver, int sum, ClientBase*);
int Bank_get_info(ClientBase*, ID client);

#endif