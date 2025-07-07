/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** player_vision.c
*/

#include "server.h"


static void add_single_resource(char *temp, resource_type_t resource,
    bool *tile_empty)
{
    size_t current_len = strlen(temp);
    size_t max_len = 8192;

    if (!*tile_empty && current_len < max_len - 1)
        strncat(temp, " ", max_len - current_len - 1);
    append_resource_name(temp, resource, max_len);
    *tile_empty = false;
}

static bool add_resource_type(char *temp, resource_type_t type, int count_max,
    bool *tile_empty)
{
    int count = 0;

    while (count < count_max) {
        if (strlen(temp) >= 8192 - 50)
            return false;
        add_single_resource(temp, type, tile_empty);
        count++;
    }
    return true;
}

static void add_tile_resources(char *temp, tile_t *tile, bool *tile_empty)
{
    int r;

    for (r = 0; r < RESOURCE_TYPES; r++) {
        if (!add_resource_type(temp, (resource_type_t)r,
            tile->resource_count[r], tile_empty))
            return;
    }
}

static void add_tile_players(char *temp, tile_t *tile, bool *tile_empty)
{
    player_t *p = tile->players;
    size_t current_len;
    size_t max_len = 8192;

    while (p) {
        current_len = strlen(temp);
        if (current_len >= max_len - 10)
            return;
        if (!*tile_empty && current_len < max_len - 1)
            strncat(temp, " ", max_len - current_len - 1);
        current_len = strlen(temp);
        if (current_len < max_len - 7)
            strncat(temp, "player", max_len - current_len - 1);
        *tile_empty = false;
        p = p->next;
    }
}

static void process_vision_tile(char *temp, player_t *player, map_t *map,
    int *coords)
{
    tile_t *tile = map_get_tile(map, coords[0], coords[1]);
    bool tile_empty = true;
    size_t current_len;
    size_t max_len = 8192;

    if (!tile)
        return;
    if (coords[0] != player->x || coords[1] != player->y) {
        current_len = strlen(temp);
        if (current_len < max_len - 1)
            strncat(temp, ",", max_len - current_len - 1);
    }
    add_tile_resources(temp, tile, &tile_empty);
    if (coords[0] != player->x || coords[1] != player->y) {
        add_tile_players(temp, tile, &tile_empty);
    }
}

static void init_vision_buffer(char *temp, size_t max_len)
{
    temp[0] = '[';
    temp[1] = '\0';
    strncat(temp, "player", max_len - 2);
}

static void finalize_vision_buffer(char *temp, char *vision, size_t max_len)
{
    size_t current_len = strlen(temp);

    if (current_len < max_len - 1)
        strncat(temp, "]", max_len - current_len - 1);
    strncpy(vision, temp, 4095);
    vision[4095] = '\0';
}

static void process_vision_distance(char *temp, player_t *player, map_t *map,
    int dist)
{
    int coords[3];
    int i;
    size_t max_len = 8192;

    for (i = -dist; i <= dist; i++) {
        if (strlen(temp) >= max_len - 100)
            break;
        coords[0] = 0;
        coords[1] = 0;
        get_vision_coords(player, dist, i, coords);
        map_normalize_coords(map, &coords[0], &coords[1]);
        process_vision_tile(temp, player, map, coords);
    }
}

void player_get_vision(player_t *player, map_t *map, char *vision)
{
    int vision_range = player->level;
    char temp[8192];
    size_t max_len = sizeof(temp);
    int dist;

    if (!player || !map || !vision)
        return;
    init_vision_buffer(temp, max_len);
    for (dist = 0; dist <= vision_range; dist++) {
        process_vision_distance(temp, player, map, dist);
        if (strlen(temp) >= max_len - 100)
            break;
    }
    finalize_vision_buffer(temp, vision, max_len);
}
