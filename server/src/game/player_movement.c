/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** player_movement.c
*/

#include "server.h"

static void update_position_by_orientation(player_t *player,
    int *new_x, int *new_y)
{
    switch (player->orientation) {
        case NORTH:
            (*new_y)--;
            break;
        case EAST:
            (*new_x)++;
            break;
        case SOUTH:
            (*new_y)++;
            break;
        case WEST:
            (*new_x)--;
            break;
    }
}

bool player_move_forward(player_t *player, map_t *map)
{
    int new_x = player->x;
    int new_y = player->y;

    if (!player || !map)
        return false;
    map_remove_player(map, player);
    update_position_by_orientation(player, &new_x, &new_y);
    map_normalize_coords(map, &new_x, &new_y);
    player->x = new_x;
    player->y = new_y;
    map_add_player(map, player);
    return true;
}

void player_turn_left(player_t *player)
{
    if (!player)
        return;
    switch (player->orientation) {
        case NORTH:
            player->orientation = WEST;
            break;
        case EAST:
            player->orientation = NORTH;
            break;
        case SOUTH:
            player->orientation = EAST;
            break;
        case WEST:
            player->orientation = SOUTH;
            break;
    }
}

void player_turn_right(player_t *player)
{
    if (!player)
        return;
    switch (player->orientation) {
        case NORTH:
            player->orientation = EAST;
            break;
        case EAST:
            player->orientation = SOUTH;
            break;
        case SOUTH:
            player->orientation = WEST;
            break;
        case WEST:
            player->orientation = NORTH;
            break;
    }
}

void player_freeze(player_t *player, uint64_t duration)
{
    if (!player)
        return;
    player->is_frozen = true;
    player->frozen_until = get_timestamp() + duration;
}

bool player_is_frozen(player_t *player)
{
    if (!player)
        return false;
    if (player->is_frozen && get_timestamp() >= player->frozen_until) {
        player->is_frozen = false;
        player->frozen_until = 0;
    }
    return player->is_frozen;
}
