/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** team.c
*/

#include "server.h"

static bool init_client_buffers(client_t *client)
{
    client->in_buf = buffer_create(BUFFER_SIZE);
    client->out_buf = buffer_create(BUFFER_SIZE);
    client->cmd_queue = command_queue_create(MAX_COMMAND_QUEUE);
    if (!client->in_buf || !client->out_buf || !client->cmd_queue) {
        client_destroy(client);
        return false;
    }
    return true;
}

client_t *client_create(int fd, struct sockaddr_in *addr)
{
    client_t *client = malloc(sizeof(client_t));

    if (!client)
        return NULL;
    memset(client, 0, sizeof(client_t));
    client->fd = fd;
    if (addr)
        client->addr = *addr;
    if (!init_client_buffers(client))
        return NULL;
    client->type = CLIENT_UNKNOWN;
    client->state = STATE_CONNECTING;
    client->connect_time = time(NULL);
    return client;
}
