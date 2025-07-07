/*
** EPITECH PROJECT, 2025
** server [WSL: Ubuntu]
** File description:
** gui_broadcast2.c
*/

#include "server.h"

void broadcast_tile_content_to_guis(server_t *server, int x, int y)
{
    client_t *client = server->clients;

    if (!server)
        return;
    while (client) {
        if (client->type == CLIENT_GUI && client->state == STATE_CONNECTED)
            gui_send_tile_content(client, server->map, x, y);
        client = client->next;
    }
}

void broadcast_egg_laid_to_guis(server_t *server, egg_t *egg, team_t *team,
    player_t *layer)
{
    client_t *client = server->clients;
    char message[128];

    if (!server || !egg || !team || !layer)
        return;
    snprintf(message, sizeof(message), "enw #%d #%d %d %d\n",
        egg->id, layer->id, egg->x, egg->y);
    while (client) {
        if (client->type == CLIENT_GUI && client->state == STATE_CONNECTED)
            client_send(client, message);
        client = client->next;
    }
}

void broadcast_incantation_start_to_guis(server_t *server, player_t *player,
    int level)
{
    client_t *client = server->clients;

    if (!server || !player)
        return;
    while (client) {
        if (client->type == CLIENT_GUI && client->state == STATE_CONNECTED)
            gui_send_incantation_start(client, player, level);
        client = client->next;
    }
}

void broadcast_incantation_end_to_guis(server_t *server, player_t *player,
    bool success)
{
    client_t *client = server->clients;

    if (!server || !player)
        return;
    while (client) {
        if (client->type == CLIENT_GUI && client->state == STATE_CONNECTED)
            gui_send_incantation_end(client, player, success);
        client = client->next;
    }
}

void broadcast_player_expelled_to_guis(server_t *server, player_t *player)
{
    client_t *client = server->clients;

    if (!server || !player)
        return;
    while (client) {
        if (client->type == CLIENT_GUI && client->state == STATE_CONNECTED)
            gui_send_player_expelled(client, player);
        client = client->next;
    }
}
