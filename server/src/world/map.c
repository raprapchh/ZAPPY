/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** map.c
*/

#include "server.h"

static void init_tile(tile_t *tile, int x, int y)
{
    int i;

    tile->x = x;
    tile->y = y;
    tile->resources = NULL;
    tile->players = NULL;
    tile->eggs = NULL;
    tile->dirty = false;
    for (i = 0; i < RESOURCE_TYPES; i++)
        tile->resource_count[i] = 0;
}

static void init_row_tiles(tile_t *row, int y, int width)
{
    int x;

    for (x = 0; x < width; x++)
        init_tile(&row[x], x, y);
}

static void cleanup_allocated_rows(map_t *map, int allocated_count)
{
    int i;

    for (i = 0; i < allocated_count; i++)
        free(map->tiles[i]);
    free(map->tiles);
    free(map);
}

static int allocate_map_rows(map_t *map)
{
    int y;

    for (y = 0; y < map->height; y++) {
        map->tiles[y] = malloc(sizeof(tile_t) * map->width);
        if (!map->tiles[y]) {
            cleanup_allocated_rows(map, y);
            return 0;
        }
        init_row_tiles(map->tiles[y], y, map->width);
    }
    return 1;
}

map_t *map_create(int width, int height)
{
    map_t *map = malloc(sizeof(map_t));

    if (!map)
        return NULL;
    map->width = width;
    map->height = height;
    map->last_spawn = 0;
    map->tiles = malloc(sizeof(tile_t *) * height);
    if (!map->tiles) {
        free(map);
        return NULL;
    }
    if (!allocate_map_rows(map))
        return NULL;
    return map;
}

static void cleanup_tile_resources(tile_t *tile)
{
    resource_t *resource;
    resource_t *next_resource;

    resource = tile->resources;
    while (resource) {
        next_resource = resource->next;
        free(resource);
        resource = next_resource;
    }
}

void map_destroy(map_t *map)
{
    int y;
    int x;

    if (!map)
        return;
    for (y = 0; y < map->height; y++) {
        for (x = 0; x < map->width; x++)
            cleanup_tile_resources(&map->tiles[y][x]);
        free(map->tiles[y]);
    }
    free(map->tiles);
    free(map);
}

tile_t *map_get_tile(map_t *map, int x, int y)
{
    if (!map)
        return NULL;
    map_normalize_coords(map, &x, &y);
    return &map->tiles[y][x];
}

void map_normalize_coords(map_t *map, int *x, int *y)
{
    if (!map || !x || !y)
        return;
    *x = (((*x) % map->width) + map->width) % map->width;
    *y = (((*y) % map->height) + map->height) % map->height;
}
