/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** gui_death.c
*/

#include "server.h"

void broadcast_player_death_to_guis(server_t *server, player_t *player)
{
    client_t *client;
    char message[64];

    if (!server || !player)
        return;
    snprintf(message, sizeof(message), "pdi #%d\n", player->id);
    client = server->clients;
    while (client) {
        if (client->type == CLIENT_GUI && client->state == STATE_CONNECTED)
            client_send(client, message);
        client = client->next;
    }
}
