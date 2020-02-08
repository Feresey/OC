#define _POSIX_C_SOURCE 199309L

#include "game.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <zmq.h>

#define err(X)     \
    {              \
        perror(X); \
        exit(1);   \
    }

#define init_game(GAME)                                            \
    {                                                              \
        GAME.A = GAME.B = GAME.pub_id = GAME.num = GAME.point = 0; \
        GAME.pub = GAME.A_sub = GAME.B_sub = GAME.field = NULL;    \
    }

volatile sig_atomic_t flag = 0;

typedef struct game {
    int A, B, pub_id;
    void *pub, *A_sub, *B_sub;
    field_t* field;
    uint num;
    point_t point;
} game_t;

Message_t* memory;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int end = 1;

void sig_check(int sig)
{
    switch (sig) {
    case SIGQUIT: {
        if (flag == 0) {
            flag = sig;
            puts(" Server blocked!");
        } else if (flag == sig) {
            flag = 0;
            puts(" Server unblocked!");
        }
        break;
    }
    case SIGINT:
        flag = sig;
    }
}

struct timespec ts = { 0, 100 }, tv;
turn_t turn;

void* play_session(void* arg)
{
    zmq_sleep(1);
    game_t* game = (game_t*)arg;
    zmq_send(game->pub, &game->A, sizeof(int), 0);
    send_field(*game->field, game->pub);

    zmq_pollitem_t poll[2] = { { game->A_sub, 0, ZMQ_POLLIN, 0 },
                               { game->B_sub, 0, ZMQ_POLLIN, 0 } };
    char cnct = '\0';
    while (end) {
        while (end && memory[game->num].point == nothing 
        && !game->field->end) {
            zmq_send(game->A_sub, &cnct, 1, 0);
            zmq_send(game->B_sub, &cnct, 1, 0);

            if (zmq_poll(&poll[0], 1, 1000) == -1 
            || poll[0].revents != ZMQ_POLLIN)
                goto end; //~  Interrupted~
            zmq_recv(game->A_sub, &cnct, 1, 0);

            if (zmq_poll(&poll[1], 1, 1000) == -1 
            || poll[1].revents != ZMQ_POLLIN)
                goto end; //~  Interrupted~
            zmq_recv(game->B_sub, &cnct, 1, 0);

            nanosleep(&ts, &tv);
        }
        if (!end || game->field->end)
            break;
        pthread_mutex_lock(&mutex);
        turn = memory[game->num].turn;
        memory[game->num].point = nothing;
        pthread_mutex_unlock(&mutex);
        if (memory[game->num].point == game->point)
            continue;

        try_turn(game->field, turn, game->point);
        send_field(*game->field, game->pub);
        game->point = (game->point == A) ? B : A;
    }
end:
    game->field->end = 1;
    send_field(*game->field, game->pub);
    game->pub_id = 0;

    return NULL;
}

int main(int argc, char** argv)
{
    const size_t game_size = (argc > 1) ? (unsigned long)atol(argv[1]) : 5;
    game_t* games = (game_t*)malloc(sizeof(game_t) * game_size);
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * game_size);
    if (!games || !threads)
        err("Memory exception");
    for (int i = 0; i < game_size; ++i)
        threads[i] = 0;
    for (int i = 0; i < game_size; ++i)
        init_game(games[i]);

    int fd;
    if ((fd = shm_open("game_play",
     O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
        err("shm::open_fail");
    if (ftruncate(fd, (long)game_size) == -1)
        err("trucate::fail");
    memory = mmap(NULL, 
    sizeof(Message_t) * game_size, PROT_READ | PROT_WRITE,
     MAP_SHARED, fd, 0); // ~отображаем на вирт. память~
    if (memory == MAP_FAILED)
        err("mmap::mapping_fail");
    close(fd);
    printf("Server started, max count of games is: %zu\n", game_size);

    void* context = zmq_ctx_new();
    void* server = zmq_socket(context, ZMQ_REP);
    char adress[25];

login:
    printf("Enter server's adress: ");
    int server_id;
    scanf("%d", &server_id);
    sprintf(adress, "%s%d", "tcp://*:", server_id);

    if (zmq_bind(server, adress)) {
        puts("adress is invalid,try again:");
        goto login;
    }

    puts("Server started");
    Message_t data;
    while (1) {
        int play = 0;
        signal(SIGINT, sig_check);
        if (flag == SIGINT)
            goto end;

        if (!flag) {
            zmq_recv(server, &data, sizeof(Message_t), 0);
            if (!data.id)
                goto end;
            for (int i = 0; i < game_size; ++i) {
                if (threads[i] && games[i].field->end) {
                    while (games[i].pub_id)
                        nanosleep(&ts, &tv);

                    pthread_detach(threads[i]);
                    threads[i] = 0;
                    zmq_close(games[i].pub);
                    zmq_close(games[i].A_sub);
                    zmq_close(games[i].B_sub);
                    printf("Game %d deleted\n", games[i].num);

                    if (games[i].field) {
                        destroy_field(*games[i].field);
                        free(games[i].field);
                    }
                    init_game(games[i]);
                }
            }

            uint i = 0;

            while (i < game_size 
            && (((data.point == A) ? games[i].A : games[i].B) != data.id))
                ++i;

            if (data.point == nothing || i == game_size) {
                i = 0;
                while (i < game_size) {
                    if (!threads[i] 
                    && (games[i].field && games[i].field->size == data.size))
                        break;
                    else
                        ++i;
                }
                if (i == game_size) {
                    i = 0;
                    while (i < game_size) {
                        if (!threads[i] && !games[i].field)
                            break;
                        else
                            ++i;
                    }
                }
                if (i == game_size)
                    goto end;
                if (!games[i].field) {
                    games[i].field = (field_t*)malloc(sizeof(field_t));
                    *games[i].field = gen_field(data.size);
                    games[i].A = data.id;
                    games[i].num = i;
                    games[i].pub = zmq_socket(context, ZMQ_PUB);
                    games[i].A_sub = zmq_socket(context, ZMQ_REQ);
                    games[i].B_sub = zmq_socket(context, ZMQ_REQ);
                    games[i].pub_id = 2000;

                    do
                        sprintf(adress, "%s%d", "tcp://*:", 
                        ++games[i].pub_id);
                    while (zmq_bind(games[i].pub, adress) == -1);

                    printf("Created new game, size=%d, adress=%s,game_id=%d\n",
                        games[i].field->size, adress, games[i].num);

                    sprintf(adress, "%s%d", "tcp://localhost:", data.req);
                    zmq_connect(games[i].A_sub, adress);
                } else {
                    games[i].B = data.id;
                    sprintf(adress, "%s%d", "tcp://localhost:", data.req);
                    zmq_connect(games[i].B_sub, adress);
                    play = 1;
                }
            }

            pthread_mutex_lock(&mutex);
            memory[i] = data;
            pthread_mutex_unlock(&mutex);

            zmq_send(server, &games[i].pub_id, sizeof(int), 0);
            if (!play)
                continue;

            printf("Players joined to game %d: player_A=%d, player_B=%d\n",
                games[i].num, games[i].A, games[i].B);
            pthread_create(&threads[i], NULL, play_session, &games[i]);
        }
    }

end:
    end = 0;
    for (int i = 0; i < game_size; ++i) {
        if (threads[i]) {
            pthread_detach(threads[i]);
            threads[i] = 0;
            zmq_close(games[i].pub);
            zmq_close(games[i].A_sub);
            zmq_close(games[i].B_sub);
            printf("Game %d deleted\n", games[i].num);
        }
        if (games[i].field) {
            destroy_field(*games[i].field);
            free(games[i].field);
        }
    }
    free(threads);
    free(games);
    if (munmap(memory, game_size))
        err("munmap");

    zmq_close(server);
    zmq_ctx_destroy(context);
    return 0;
}
