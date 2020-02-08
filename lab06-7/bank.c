// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "bank.h"

ClientBase* Bank_init()
{
    ClientBase* Base = (ClientBase*)malloc(sizeof(ClientBase));
    if (!Base) {
        fprintf(stderr, "ERROR: not enougth memory\n");
        exit(NOT_MEMORY);
    }

    Base->client.id = 0;
    Base->client.sum = 0;
    Base->next = Base;
    return Base;
}

void Bank_add_client(ClientBase* Base, ID clientID)
{
    if (Base->next == Base) {
        Base->client.id = clientID;
        Base->next = NULL;
    } else {
        while (Base->next)
            Base = Base->next;
        Base->next = Bank_init();
        Base = Base->next;
        Base->next = NULL;
        Base->client.sum = 0;
        Base->client.id = clientID;
    }
}

void Bank_print_client_base(ClientBase* Base)
{
    if (Base) {
        if (Base->next == Base)
            return;
        else
            while (Base) {
                printf("ID: %f\t", Base->client.id);
                printf("SUM: %d\n", Base->client.sum);
                Base = Base->next;
            }
    }
}

Client* _Bank_find_client(ClientBase* Base, double clientID)
{
    if (Base) {
        if (Base->next == Base)
            return NULL;
        else
            while (Base) {
                if (Base->client.id == clientID)
                    return &Base->client;
                Base = Base->next;
            }
    }

    return NULL;
}

void Bank_put_money(double clientID, int sum, ClientBase* Base)
{
    Client* cl = _Bank_find_client(Base, clientID);

    if (cl) {
        cl->sum += sum;
    } else {
        Bank_add_client(Base, clientID);
        cl = _Bank_find_client(Base, clientID);
        cl->sum += sum;
    }
}

int Bank_get_money(double clientID, int sum, ClientBase* Base)
{
    Client* cl = _Bank_find_client(Base, clientID);
    if (!cl) {
        return NOT_CLIENT;
    }
    if (cl->sum >= sum) {
        cl->sum -= sum;
        return SUCCESS;
    } else {
        return NOT_ENOUGH_MONEY;
    }
}

int Bank_send_money(ID clientSender, ID clientReceiver, int sum, ClientBase* Base)
{
    Client* clSender = _Bank_find_client(Base, clientSender);
    if (!clSender) {
        return NOT_CLIENT;
    }
    Client* clReceiver = _Bank_find_client(Base, clientReceiver);
    if (!clReceiver) {
        return RECEIVER_NOT_CLIENT;
    }

    if (clSender->sum >= sum) {
        clSender->sum -= sum;
        clReceiver->sum += sum;
        return SUCCESS;
    } else {
        return NOT_ENOUGH_MONEY;
    }
}

int Bank_get_info(ClientBase* Base, ID client)
{
    Client* cl = _Bank_find_client(Base, client);
    if (!cl) {
        return NOT_CLIENT;
    }
    return cl->sum;
}

void Bank_destroy_database(ClientBase* Base)
{
    if (Base->next == Base) {
        free(Base);
        Base = NULL;
    } else {
        ClientBase* to_delete = Base->next;
        while (to_delete) {
            free(Base);
            Base = to_delete;
            to_delete = to_delete->next;
        }
        free(to_delete);
    }
}