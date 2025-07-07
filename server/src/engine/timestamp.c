/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** timestamp.c
*/

#include "server.h"

uint64_t get_timestamp(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
        perror("clock_gettime");
        return 0;
    }
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}
