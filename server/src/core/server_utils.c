/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-raphael.chanliongco
** File description:
** server_utils.c
*/

#include "server.h"

player_t *create_and_setup_player(server_t *server, team_t *team)
{
    int spawn_x = rand() % server->map->width;
    int spawn_y = rand() % server->map->height;
    player_t *player = player_create(server->next_player_id,
        spawn_x, spawn_y, team->id);
    tile_t *tile;

    server->next_player_id++;
    if (!player)
        return NULL;
    tile = map_get_tile(server->map, player->x, player->y);
    if (!tile) {
        player->x = 0;
        player->y = 0;
    }
    return player;
}
