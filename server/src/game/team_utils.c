/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** team.c
*/

#include "server.h"

team_t *team_find_by_name(team_t *teams, const char *name)
{
    team_t *current = teams;

    if (!name)
        return NULL;
    while (current) {
        if (strcmp(current->name, name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

int team_get_slots(team_t *team)
{
    if (!team)
        return 0;
    return team->max_clients - team->connected_clients;
}
