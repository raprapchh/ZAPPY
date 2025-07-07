/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** ai_movement_commands.c
*/

#include "server.h"

void execute_forward(server_t *server, client_t *client)
{
    if (client->player && !client->player->is_dead &&
        player_move_forward(client->player, server->map)) {
        client_send(client, "ok\n");
        broadcast_player_position_to_guis(server, client->player);
    } else {
        client_send(client, "ko\n");
    }
    client->player->is_busy = false;
}

void execute_right(server_t *server, client_t *client)
{
    if (client->player && !client->player->is_dead) {
        player_turn_right(client->player);
        client_send(client, "ok\n");
        broadcast_player_position_to_guis(server, client->player);
    } else {
        client_send(client, "ko\n");
    }
    client->player->is_busy = false;
}

void execute_left(server_t *server, client_t *client)
{
    if (client->player && !client->player->is_dead) {
        player_turn_left(client->player);
        client_send(client, "ok\n");
        broadcast_player_position_to_guis(server, client->player);
    } else {
        client_send(client, "ko\n");
    }
    client->player->is_busy = false;
}
