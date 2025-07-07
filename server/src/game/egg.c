/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** egg.c
*/

#include "server.h"

egg_t *egg_create(int id, int x, int y, int team_id)
{
    egg_t *egg = malloc(sizeof(egg_t));

    if (!egg)
        return NULL;
    egg->id = id;
    egg->x = x;
    egg->y = y;
    egg->team_id = team_id;
    egg->laid_at = get_timestamp();
    egg->next = NULL;
    return egg;
}

void egg_destroy(egg_t *egg)
{
    if (egg)
        free(egg);
}

bool egg_hatch(egg_t *egg, player_t **player)
{
    if (!egg || !player)
        return false;
    *player = player_create(egg->id, egg->x, egg->y, egg->team_id);
    return *player != NULL;
}
