/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-raphael.chanliongco
** File description:
** map_spawn.c
*/

#include "server.h"

static resource_t *create_resource(resource_type_t type, int x, int y)
{
    resource_t *resource;

    resource = malloc(sizeof(resource_t));
    if (!resource)
        return NULL;
    resource->type = type;
    resource->x = x;
    resource->y = y;
    resource->next = NULL;
    return resource;
}

bool map_add_resource(map_t *map, int x, int y, resource_type_t type)
{
    tile_t *tile;
    resource_t *resource;

    if (!map || type < 0 || type >= RESOURCE_TYPES)
        return false;
    tile = map_get_tile(map, x, y);
    if (!tile)
        return false;
    resource = create_resource(type, tile->x, tile->y);
    if (!resource)
        return false;
    resource->next = tile->resources;
    tile->resources = resource;
    tile->resource_count[type]++;
    tile->dirty = true;
    return true;
}

void map_remove_player(map_t *map, player_t *player)
{
    tile_t *tile;
    player_t **current;

    if (!map || !player)
        return;
    tile = map_get_tile(map, player->x, player->y);
    if (!tile)
        return;
    current = &tile->players;
    while (*current) {
        if (*current == player) {
            *current = player->next;
            return;
        }
        current = &(*current)->next;
    }
}
