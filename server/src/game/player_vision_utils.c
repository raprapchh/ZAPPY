/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-raphael.chanliongco
** File description:
** player_vision_utils.c
*/

#include "server.h"

void append_resource_name_part2(char *temp, resource_type_t resource,
    size_t max_len)
{
    size_t current_len = strlen(temp);

    switch (resource) {
        case MENDIANE:
            strncat(temp, "mendiane", max_len - current_len - 1);
            break;
        case PHIRAS:
            strncat(temp, "phiras", max_len - current_len - 1);
            break;
        case THYSTAME:
            strncat(temp, "thystame", max_len - current_len - 1);
            break;
        default:
            strncat(temp, "unknown", max_len - current_len - 1);
            break;
    }
}

void append_resource_name(char *temp, resource_type_t resource,
    size_t max_len)
{
    size_t current_len = strlen(temp);

    switch (resource) {
        case FOOD:
            strncat(temp, "food", max_len - current_len - 1);
            break;
        case LINEMATE:
            strncat(temp, "linemate", max_len - current_len - 1);
            break;
        case DERAUMERE:
            strncat(temp, "deraumere", max_len - current_len - 1);
            break;
        case SIBUR:
            strncat(temp, "sibur", max_len - current_len - 1);
            break;
        default:
            append_resource_name_part2(temp, resource, max_len);
            break;
    }
}

void get_vision_coords_north_south(player_t *player, int dist, int *coords)
{
    coords[0] = player->x + coords[2];
    if (player->orientation == NORTH)
        coords[1] = player->y - dist;
    else
        coords[1] = player->y + dist;
}

void get_vision_coords_east_west(player_t *player, int dist, int *coords)
{
    coords[1] = player->y + coords[2];
    if (player->orientation == EAST)
        coords[0] = player->x + dist;
    else
        coords[0] = player->x - dist;
}

void get_vision_coords(player_t *player, int dist, int i, int *coords)
{
    coords[0] = player->x;
    coords[1] = player->y;
    coords[2] = i;
    if (player->orientation == NORTH || player->orientation == SOUTH)
        get_vision_coords_north_south(player, dist, coords);
    else
        get_vision_coords_east_west(player, dist, coords);
}
