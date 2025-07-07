/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** timeout.c
*/

#include "server.h"

int calculate_timeout(event_queue_t *events, uint64_t now)
{
    int timeout = 100;

    if (!events || !events->head)
        return timeout;
    if (events->head->execute_at <= now)
        return 0;
    timeout = (int)(events->head->execute_at - now);
    if (timeout > 1000)
        timeout = 1000;
    if (timeout < 0)
        timeout = 0;
    return timeout;
}
