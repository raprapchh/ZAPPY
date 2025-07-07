/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** server_client.c
*/

#include "server.h"

static void remove_client_from_list(server_t *server, client_t *client)
{
    client_t **current = &server->clients;

    while (*current && *current != client)
        current = &(*current)->next;
    if (*current)
        *current = client->next;
}

static void remove_client_from_poll(server_t *server, client_t *client)
{
    for (size_t i = 1; i < server->poll_count; i++) {
        if (server->poll_fds[i].fd != client->fd)
            continue;
        for (size_t j = i; j < server->poll_count - 1; j++)
            server->poll_fds[j] = server->poll_fds[j + 1];
        server->poll_count--;
        break;
    }
}

static void cleanup_client_player(server_t *server, client_t *client)
{
    player_t **player;

    if (!client->player || !client->team)
        return;
    map_remove_player(server->map, client->player);
    player = &client->team->players;
    while (*player && *player != client->player)
        player = &(*player)->next;
    if (*player)
        *player = client->player->next;
    player_destroy(client->player);
    client->player = NULL;
    team_remove_client(client->team);
    client->team = NULL;
}

void server_add_client(server_t *server, client_t *client)
{
    if (server->poll_count >= MAX_CLIENTS + 1) {
        client_send(client, "ko\n");
        client_write(client);
        close(client->fd);
        client_destroy(client);
        return;
    }
    client_send(client, "WELCOME\n");
    client->next = server->clients;
    server->clients = client;
    server->poll_fds[server->poll_count].fd = client->fd;
    server->poll_fds[server->poll_count].events = POLLIN | POLLOUT;
    server->poll_count++;
}

void server_remove_client(server_t *server, client_t *client)
{
    if (!server || !client)
        return;
    remove_client_from_list(server, client);
    remove_client_from_poll(server, client);
    cleanup_client_player(server, client);
    client_destroy(client);
}
