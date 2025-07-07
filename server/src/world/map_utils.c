/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** map.c
*/

#include "server.h"

static const float resource_densities[RESOURCE_TYPES] = {
    0.5f,   /* FOOD */
    0.3f,   /* LINEMATE */
    0.15f,  /* DERAUMERE */
    0.1f,   /* SIBUR */
    0.1f,   /* MENDIANE */
    0.08f,  /* PHIRAS */
    0.05f   /* THYSTAME */
};

int map_count_resource_type(map_t *map, resource_type_t type)
{
    int total = 0;
    int x = 0;
    int y = 0;

    if (!map || type < 0 || type >= RESOURCE_TYPES)
        return 0;
    for (y = 0; y < map->height; y++) {
        for (x = 0; x < map->width; x++) {
            total += map->tiles[y][x].resource_count[type];
        }
    }
    return total;
}

static void spawn_resource_type(map_t *map, resource_type_t type,
    server_t *server)
{
    int total_quantity;
    int spawned;
    int x;
    int y;

    total_quantity = (int)(map->width * map->height *
        resource_densities[type]);
    spawned = 0;
    if (total_quantity < 1)
        total_quantity = 1;
    while (spawned < total_quantity) {
        x = rand() % map->width;
        y = rand() % map->height;
        if (map_add_resource(map, x, y, type)) {
            spawned++;
            broadcast_tile_content_to_guis(server, x, y);
        }
    }
}

static void maintain_resource_type(map_t *map, resource_type_t type,
    server_t *server)
{
    int target_quantity = (int)(map->width * map->height *
        resource_densities[type]);
    int current_quantity;
    int needed = 0;
    int spawned = 0;
    int x = 0;
    int y = 0;

    if (target_quantity < 1)
        target_quantity = 1;
    current_quantity = map_count_resource_type(map, type);
    needed = target_quantity - current_quantity;
    while (spawned < needed) {
        x = rand() % map->width;
        y = rand() % map->height;
        if (map_add_resource(map, x, y, type)) {
            spawned++;
            broadcast_tile_content_to_guis(server, x, y);
        }
    }
}

void map_spawn_resources(map_t *map, int freq, server_t *server)
{
    int type;

    (void)freq;
    if (!map)
        return;
    for (type = 0; type < RESOURCE_TYPES; type++) {
        spawn_resource_type(map, (resource_type_t)type, server);
    }
}

void map_maintain_resources(map_t *map, int freq, server_t *server)
{
    int type;

    (void)freq;
    if (!map)
        return;
    for (type = 0; type < RESOURCE_TYPES; type++) {
        maintain_resource_type(map, (resource_type_t)type, server);
    }
}

void map_add_player(map_t *map, player_t *player)
{
    tile_t *tile;

    if (!map || !player)
        return;
    tile = map_get_tile(map, player->x, player->y);
    if (!tile)
        return;
    player->next = tile->players;
    tile->players = player;
}

static bool remove_resource_from_tile(tile_t *tile, resource_type_t type)
{
    resource_t **current;
    resource_t *to_remove;

    current = &tile->resources;
    while (*current) {
        if ((*current)->type == type) {
            to_remove = *current;
            *current = (*current)->next;
            free(to_remove);
            tile->resource_count[type]--;
            tile->dirty = true;
            return true;
        }
        current = &(*current)->next;
    }
    return false;
}

bool map_remove_resource(map_t *map, int x, int y, resource_type_t type)
{
    tile_t *tile;

    if (!map || type < 0 || type >= RESOURCE_TYPES) {
        return false;
    }
    tile = map_get_tile(map, x, y);
    if (!tile) {
        return false;
    }
    if (tile->resource_count[type] == 0) {
        return false;
    }
    return remove_resource_from_tile(tile, type);
}
