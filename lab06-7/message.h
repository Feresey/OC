#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#define STR_SIZE 256
#define ID double

typedef struct _msg {
    ID client;
    int sum;
    int action;
    ID receiverClient;
    void* requester;
    char message[STR_SIZE];
} Message_t;

#endif