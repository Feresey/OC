// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#define _POSIX_C_SOURCE 199309L

#include "game.h"

void send_field(field_t field, void* requester)
{
    struct timespec ts = { 0, 100 }, tv;
    zmq_pollitem_t p = { requester, 0, ZMQ_POLLOUT, 0 };
    size_t size = sizeof(cell_t) * field.size;
    for (uint i = 0; i < field.size; ++i) {
        while (zmq_poll(&p, 1, 1) == -1)
            nanosleep(&ts, &tv);
        zmq_send(requester, field.regiment[i], size, 0);
    }
    zmq_send(requester, &field.end, 4, 0);
}

void get_field(field_t* field, void* resiever)
{
    struct timespec ts = { 0, 100 }, tv;
    zmq_pollitem_t p = { resiever, 0, ZMQ_POLLIN, 0 };
    size_t size = sizeof(cell_t) * field->size;
    for (uint i = 0; i < field->size; ++i) {
        while (zmq_poll(&p, 1, 1) == -1)
            nanosleep(&ts, &tv);
        zmq_recv(resiever, field->regiment[i], size, 0);
    }
    zmq_recv(resiever, &field->end, 4, 0);
}

field_t gen_field(uint size)
{
    cell_t** res = (cell_t**)malloc(sizeof(cell_t*) * size);
    for (uint i = 0; i < size; ++i) {
        res[i] = (cell_t*)malloc(sizeof(cell_t) * size);
        for (uint j = 0; j < size; ++j) {
            res[i][j].type = nothing;
            for (uint l = 0; l < 4; ++l)
                res[i][j].edges[l] = 0;
        }
    }
    field_t R;
    R.regiment = res;
    R.size = size;
    R.end = 0;
    return R;
}

void destroy_field(field_t F)
{
    for (uint i = 0; i < F.size; ++i)
        free(F.regiment[i]);
    free(F.regiment);
}

int fix_field(point_t player, field_t* f)
{
    cell_t** F = f->regiment;
    int ind = 0;
    int no = 0;
    for (uint i = 0; i < f->size; ++i)
        for (uint j = 0; j < f->size; ++j) {
            if (F[i][j].edges[0]
                && F[i][j].edges[1]
                && F[i][j].edges[2]
                && F[i][j].edges[3]
                && F[i][j].type == nothing) {
                F[i][j].type = player;
                ++ind;
            }
            if (F[i][j].type == nothing)
                ++no;
        }
    if (!no)
        f->end = 1;
    return ind;
}

int try_turn(field_t* f, turn_t choice, point_t player)
{
    if (f->size < choice.x || f->size < choice.y) // incorrect pos
        return WRONG_TURN;

    cell_t** F = f->regiment;
    F[choice.x][choice.y].edges[choice.side] = 1;
    if (choice.side == top && choice.x)
        F[choice.x - 1][choice.y].edges[bottom] = 1;
    else if (choice.side == right && choice.y < f->size - 1)
        F[choice.x][choice.y + 1].edges[left] = 1;
    else if (choice.side == bottom && choice.x < f->size - 1)
        F[choice.x + 1][choice.y].edges[top] = 1;
    else if (choice.side == left && choice.y)
        F[choice.x][choice.y - 1].edges[right] = 1;

    return fix_field(player, f);
}

void show_field(field_t f)
{
    cell_t** F = f.regiment;
    printf(F[0][0].edges[top] && F[0][0].edges[left] ? "┌" : " ");
    for (uint j = 0; j < f.size; ++j) {
        printf(F[0][j].edges[top] ? "─" : " ");
        if (F[0][j].edges[top]) {
            if (j < f.size - 1 && F[0][j + 1].edges[top]) {
                printf(F[0][j].edges[right] ? "┬" : "─");
            } else if (F[0][j].edges[right])
                printf("┐");
        } else if (j < f.size - 1 && F[0][j + 1].edges[top] && F[0][j].edges[right])
            printf("┌");
        else
            printf(" ");
    }
    printf("\n");

    for (uint i = 0; i < f.size; ++i) {
        printf(F[i][0].edges[left] ? "│" : " ");
        for (uint j = 0; j < f.size; ++j) {
            printf(F[i][j].type != nothing ? (F[i][j].type == A ? "A" : "B") : "N");
            printf(F[i][j].edges[right] ? "│" : " ");
        }
        printf("\n");

        if (F[i][0].edges[bottom]) {
            if (F[i][0].edges[left]) {
                printf((i < f.size - 1 && F[i + 1][0].edges[left]) ? "├" : "└");
            } else if (i < f.size - 1 && F[i + 1][0].edges[left])
                printf("┌");
            else
                printf(" ");
        } else
            printf((F[i][0].edges[left] && i < f.size - 1 && F[i + 1][0].edges[left]) ? "│" : " ");

        for (uint j = 0; j < f.size; ++j) {
            printf(F[i][j].edges[bottom] ? "─" : " ");
            if (!F[i][j].edges[bottom]) {
                if (!F[i][j].edges[right]
                    && i < f.size - 1 && j < f.size - 1
                    && F[i][j + 1].edges[bottom]
                    && F[i + 1][j].edges[right])
                    printf("┌");
                else if (F[i][j].edges[right]
                    && i < f.size - 1 && j < f.size - 1
                    && F[i][j + 1].edges[bottom]
                    && F[i + 1][j].edges[right])
                    printf("├");
                else if (F[i][j].edges[right]
                    && j < f.size - 1
                    && F[i][j + 1].edges[bottom])
                    printf("└");
                else
                    printf((F[i][j].edges[right] && i < f.size - 1 && F[i + 1][j].edges[right]) ? "│" : " ");
            } else {
                if (!F[i][j].edges[right]) {
                    if (i < f.size - 1 && j < f.size - 1
                        && F[i][j + 1].edges[bottom]
                        && F[i + 1][j].edges[right])
                        printf("┬");
                    else if (i < f.size - 1
                        && F[i + 1][j].edges[right])
                        printf("┐");
                    else
                        printf("─");
                } else {
                    if (i < f.size - 1 && j < f.size - 1
                        && F[i][j + 1].edges[bottom]
                        && F[i + 1][j].edges[right])
                        printf("┼");
                    else if (i < f.size - 1
                        && F[i + 1][j].edges[right])
                        printf("┤");
                    else
                        printf((j < f.size - 1 && F[i][j + 1].edges[bottom]) ? "┴" : "┘");
                }
            }
        }
        printf("\n");
    }
    printf("\n");
}

void get_turn(uint x, uint y, side_t side, field_t f)
{
    turn_t turn;
    turn.x = x;
    turn.y = y;
    turn.side = side;
    int res = try_turn(&f, turn, B);
    if (res == WRONG_TURN)
        printf("Wrong turn!!!\n");
    show_field(f);
}
