/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** gui_broadcast.c
*/

#include "server.h"

void broadcast_player_new_to_guis(server_t *server, player_t *player,
    team_t *team)
{
    client_t *client = server->clients;

    if (!server || !player || !team)
        return;
    while (client) {
        if (client->type == CLIENT_GUI && client->state == STATE_CONNECTED)
            gui_send_player_new(client, player, team);
        client = client->next;
    }
}

void broadcast_player_position_to_guis(server_t *server, player_t *player)
{
    client_t *client = server->clients;

    if (!server || !player)
        return;
    while (client) {
        if (client->type == CLIENT_GUI && client->state == STATE_CONNECTED)
            gui_send_player_position(client, player);
        client = client->next;
    }
}

void broadcast_player_level_to_guis(server_t *server, player_t *player)
{
    client_t *client = server->clients;

    if (!server || !player)
        return;
    while (client) {
        if (client->type == CLIENT_GUI && client->state == STATE_CONNECTED)
            gui_send_player_level(client, player);
        client = client->next;
    }
}

void broadcast_player_inventory_to_guis(server_t *server, player_t *player)
{
    client_t *client = server->clients;

    if (!server || !player)
        return;
    while (client) {
        if (client->type == CLIENT_GUI && client->state == STATE_CONNECTED)
            gui_send_player_inventory(client, player);
        client = client->next;
    }
}

void broadcast_message(server_t *server, player_t *sender, const char *message)
{
    client_t *client = server->clients;
    char response[512];
    int direction = 0;

    if (!server || !sender || !message)
        return;
    while (client) {
        if (client->type == CLIENT_AI &&
            client->state == STATE_CONNECTED &&
            client->player && client->player != sender) {
            snprintf(response, sizeof(response), "message %d, %s\n",
                direction, message);
            client_send(client, response);
        }
        client = client->next;
    }
}
