/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** player.c
*/

#include "server.h"

player_t *player_create(int id, int x, int y, int team_id)
{
    player_t *player = malloc(sizeof(player_t));

    player->id = id;
    player->x = x;
    player->y = y;
    player->level = 1;
    player->orientation = (orientation_t)(rand() % 4 + 1);
    player->team_id = team_id;
    player->last_meal = get_timestamp();
    player->is_frozen = false;
    player->frozen_until = 0;
    player->is_dead = false;
    player->is_busy = false;
    player->busy_until = 0;
    player->next = NULL;
    for (int i = 0; i < RESOURCE_TYPES; i++)
        player->inventory[i] = 0;
    player->inventory[FOOD] = 10;
    return player;
}

void player_destroy(player_t *player)
{
    if (player)
        free(player);
}

bool player_take_resource(player_t *player, map_t *map, resource_type_t type)
{
    if (!player || !map || type < 0 || type >= RESOURCE_TYPES) {
        return false;
    }
    if (map_remove_resource(map, player->x, player->y, type)) {
        player->inventory[type]++;
        return true;
    }
    return false;
}

bool player_drop_resource(player_t *player, map_t *map, resource_type_t type)
{
    if (!player || !map || type < 0 || type >= RESOURCE_TYPES)
        return false;
    if (player->inventory[type] > 0) {
        player->inventory[type]--;
        return map_add_resource(map, player->x, player->y, type);
    }
    return false;
}

void player_get_inventory(player_t *player, char *inventory)
{
    if (!player || !inventory)
        return;
    snprintf(inventory, 512,
        "[food %d, linemate %d, deraumere %d, sibur %d, "
        "mendiane %d, phiras %d, thystame %d]",
        player->inventory[FOOD], player->inventory[LINEMATE],
        player->inventory[DERAUMERE], player->inventory[SIBUR],
        player->inventory[MENDIANE], player->inventory[PHIRAS],
        player->inventory[THYSTAME]);
}
