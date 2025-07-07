/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** ai_movement_commands.c
*/

#include "server.h"

void cmd_forward(server_t *server, client_t *client)
{
    uint64_t execute_time = get_timestamp() + (7000 / server->config.freq);

    if (!client->player) {
        client_send(client, "ko\n");
        return;
    }
    client->player->is_busy = true;
    client->player->busy_until = execute_time;
    event_queue_add(server->events, execute_time, client->fd, "forward_exec");
}

void cmd_right(server_t *server, client_t *client)
{
    uint64_t execute_time = get_timestamp() + (7000 / server->config.freq);

    if (!client->player) {
        client_send(client, "ko\n");
        return;
    }
    client->player->is_busy = true;
    client->player->busy_until = execute_time;
    event_queue_add(server->events, execute_time, client->fd, "right_exec");
}

void cmd_left(server_t *server, client_t *client)
{
    uint64_t execute_time = get_timestamp() + (7000 / server->config.freq);

    if (!client->player) {
        client_send(client, "ko\n");
        return;
    }
    client->player->is_busy = true;
    client->player->busy_until = execute_time;
    event_queue_add(server->events, execute_time, client->fd, "left_exec");
}
