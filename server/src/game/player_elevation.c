/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** player_elevation.c
*/

#include "server.h"

const int elevation_requirements[8][RESOURCE_TYPES] = {
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0},
    {0, 2, 0, 1, 0, 2, 0},
    {0, 1, 1, 2, 0, 1, 0},
    {0, 1, 2, 1, 3, 0, 0},
    {0, 1, 2, 3, 0, 1, 0},
    {0, 2, 2, 2, 2, 2, 1}
};

const int elevation_players_required[8] = {1, 2, 2, 4, 4, 6, 6, 8};

static bool check_resources_for_elevation(tile_t *tile, int level_idx)
{
    for (int i = 0; i < RESOURCE_TYPES; i++) {
        if (tile->resource_count[i] < elevation_requirements[level_idx][i])
            return false;
    }
    return true;
}

static int count_players_same_level(tile_t *tile, int level)
{
    player_t *p = tile->players;
    int count = 0;

    while (p) {
        if (p->level == level)
            count++;
        p = p->next;
    }
    return count;
}

bool player_can_level_up(player_t *player, map_t *map)
{
    tile_t *tile;
    int level_idx;
    int players_on_tile;

    if (!player || !map || player->level >= 8)
        return false;
    tile = map_get_tile(map, player->x, player->y);
    if (!tile)
        return false;
    level_idx = player->level - 1;
    if (level_idx < 0 || level_idx >= 7)
        return false;
    if (!check_resources_for_elevation(tile, level_idx))
        return false;
    players_on_tile = count_players_same_level(tile, player->level);
    return players_on_tile >= elevation_players_required[level_idx];
}

static void consume_elevation_resources(map_t *map, player_t *player,
    int level_idx)
{
    for (int i = 0; i < RESOURCE_TYPES; i++) {
        for (int j = 0; j < elevation_requirements[level_idx][i]; j++)
            map_remove_resource(map, player->x, player->y,
                (resource_type_t)i);
    }
}

void player_level_up(player_t *player, map_t *map)
{
    tile_t *tile;
    int level_idx;

    if (!player || !map || player->level >= 8)
        return;
    tile = map_get_tile(map, player->x, player->y);
    if (!tile)
        return;
    level_idx = player->level - 1;
    consume_elevation_resources(map, player, level_idx);
    player->level++;
}
