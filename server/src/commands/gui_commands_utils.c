/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** gui_commands_utils.c
*/

#include "server.h"

void handle_msz(server_t *server, client_t *client)
{
    gui_send_map_size(client, server->map);
}

void handle_bct(server_t *server, client_t *client, const char *args)
{
    int x = 0;
    int y = 0;

    if (sscanf(args, "%d %d", &x, &y) == 2) {
        if (x < 0 || y < 0 || x >= server->map->width ||
            y >= server->map->height) {
            client_send(client, "sbp\n");
            return;
        }
        gui_send_tile_content(client, server->map, x, y);
    } else {
        client_send(client, "sbp\n");
    }
}

void handle_ppo(server_t *server, client_t *client, const char *args)
{
    int player_id = 0;
    player_t *player = NULL;

    if (sscanf(args, "#%d", &player_id) != 1) {
        client_send(client, "sbp\n");
        return;
    }
    player = find_player_by_id(server, player_id);
    if (player)
        gui_send_player_position(client, player);
    else
        client_send(client, "sbp\n");
}

void handle_plv(server_t *server, client_t *client, const char *args)
{
    int player_id = 0;
    player_t *player = NULL;

    if (sscanf(args, "#%d", &player_id) != 1) {
        client_send(client, "sbp\n");
        return;
    }
    player = find_player_by_id(server, player_id);
    if (player)
        gui_send_player_level(client, player);
    else
        client_send(client, "sbp\n");
}

void handle_pin(server_t *server, client_t *client, const char *args)
{
    int player_id = 0;
    player_t *player = NULL;

    if (sscanf(args, "#%d", &player_id) != 1) {
        client_send(client, "sbp\n");
        return;
    }
    player = find_player_by_id(server, player_id);
    if (player)
        gui_send_player_inventory(client, player);
    else
        client_send(client, "sbp\n");
}
