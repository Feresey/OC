// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>

#include "game.h"

int main()
{
    field_t F = gen_field(3);
    get_turn(0, 0, 0, F);
    get_turn(0, 0, 0, F);
    get_turn(0, 0, 1, F);
    get_turn(0, 0, 2, F);
    get_turn(0, 0, 3, F);

    puts("________________________________");
    get_turn(0, 1, top, F);
    get_turn(0, 2, top, F);
    get_turn(0, 0, top, F);
    get_turn(0, 1, right, F);
    get_turn(0, 1, left, F);
    // get_turn(0, 1, bottom, F);
    get_turn(1, 1, right, F);
    get_turn(1, 1, left, F);
    get_turn(1, 1, bottom, F);

    get_turn(1, 2, right, F);
    get_turn(1, 2, top, F);
    get_turn(1, 2, bottom, F);

    get_turn(0, 2, right, F);

    get_turn(2, 2, right, F);
    get_turn(2, 2, left, F);
    get_turn(2, 2, bottom, F);

    get_turn(2, 0, right, F);
    get_turn(2, 0, left, F);
    get_turn(2, 0, bottom, F);
    get_turn(2, 0, top, F);

    get_turn(1, 0, left, F);
    get_turn(0, 0, left, F);

    destroy_field(F);
    return 0;
}
