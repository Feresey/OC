#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <zmq.h>

#include "game.h"

field_t field;
struct timespec ts = { 0, 100 }, tv;

void* check_connect(void* soc)
{
    char cnct;
    zmq_pollitem_t poll[2] = { { soc, 0, ZMQ_POLLIN, 1 },
                               { soc, 0, ZMQ_POLLOUT, 2 } };
    while (poll[0].revents && poll[1].revents) {
        if (zmq_poll(&poll[0], 1, 100) == -1)
            break;
        zmq_recv(soc, &cnct, 1, 0);
        // nanosleep(&ts, &tv);
        if (zmq_poll(&poll[1], 1, 100) == -1)
            break;
        zmq_send(soc, &cnct, 1, 0);
    }
    return NULL;
}

int main()
{
    void* context = zmq_ctx_new();
    void* server = zmq_socket(context, ZMQ_REQ);
    void* sub = zmq_socket(context, ZMQ_SUB);
    void* rep = zmq_socket(context, ZMQ_REP);
    int client_id;
    int rep_id = 1000;
    int server_id;
    char adress[25];

    do
        sprintf(adress, "%s%d", "tcp://*:", ++rep_id);
    while (zmq_bind(rep, adress));

    printf("Enter client's id: ");
    scanf("%d", &client_id);

    printf("Enter server's adress: ");
    scanf("%d", &server_id);

    sprintf(adress, "%s%d", "tcp://localhost:", server_id);
    zmq_connect(server, adress);
    printf("\nConnected to %s\n\n", adress);

    Message_t m;
    Message_t* msg = &m; // (Message_t*)malloc(sizeof(Message_t));
    msg->id = client_id;
    msg->req = rep_id;
    turn_t t = { 0, 0, 0 };
    msg->turn = t;
    msg->point = nothing;
    printf("Put field size: ");
    scanf("%hhu", &msg->size);
    field = gen_field(msg->size);

    zmq_send(server, msg, sizeof(Message_t), 0);
    int pub_id;
    zmq_recv(server, &pub_id, 4, 0);

    sprintf(adress, "%s%d", "tcp://localhost:", pub_id);
    printf("Connect to %s ...\n", adress);
    int rc = zmq_connect(sub, adress);
    assert(rc == 0);
    char* filter = "";
    zmq_setsockopt(sub, ZMQ_SUBSCRIBE, filter, strlen(filter));
    printf("Connection established.\n");

    int player_id;
    zmq_recv(sub, &player_id, sizeof(int), 0);

    pthread_t th;
    pthread_create(&th, NULL, &check_connect, rep);

    puts((player_id != client_id) ? "You are player B" : "You are player A");
    msg->point = (player_id != client_id) ? B : A;

    if (player_id != client_id) {
        get_field(&field, sub);
        show_field(field);
    }

    while (1) {
        if (field.end)
            break;
        get_field(&field, sub);
        if (field.end)
            break;
        show_field(field);
        int turn_res = -1;
        char side;

    turn:
        printf("Your turn >> ");
        scanf("%hhu", &msg->turn.x);
        if (getchar() == EOF)
            goto end;
        scanf("%hhu", &msg->turn.y);
        if (getchar() == EOF)
            goto end;
        scanf("%c", &side);
        if (getchar() == EOF)
            goto end;
        --msg->turn.x;
        --msg->turn.y;

        switch (side) {
        case 't':
            msg->turn.side = top;
            break;
        case 'b':
            msg->turn.side = bottom;
            break;
        case 'l':
            msg->turn.side = left;
            break;
        case 'r':
            msg->turn.side = right;
            break;
        default:
            puts("Incorrect turn, try again");
            goto turn;
        }
        if (try_turn(&field, msg->turn, msg->point) == WRONG_TURN) {
            puts("Incorrect turn, try again");
            goto turn;
        }

        puts("send...");
        // while (zmq_poll(&p[0], 1, 1) == -1)
        nanosleep(&ts, &tv);
        zmq_send(server, msg, sizeof(Message_t), 0);

        // while (zmq_poll(&p[1], 1, 1) == -1)
        nanosleep(&ts, &tv);
        zmq_recv(server, &turn_res, sizeof(int), 0);

        if (field.end)
            break;

        get_field(&field, sub);

        if (field.end)
            break;

        show_field(field);
    }

    show_field(field);
    int count_you = 0, count_nou = 0, is_nothing;
    for (int i = 0; i < field.size; ++i)
        for (int j = 0; j < field.size; ++j)
            if (field.regiment[i][j].type == msg->point)
                ++count_you;
            else if (field.regiment[i][j].type != nothing)
                ++count_nou;
            else
                is_nothing = 1;
    if (count_you > count_nou || !is_nothing)
        printf("YOU WIN!!!\nScore: %d\n", count_you);
    else {
    end:
        puts("You lose...\n");
    }

    pthread_detach(th);
    // free(msg);
    destroy_field(field);
    zmq_close(server);
    zmq_close(sub);
    zmq_close(rep);
    zmq_ctx_destroy(context);
    return 0;
}
