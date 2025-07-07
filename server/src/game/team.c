/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** team.c
*/

#include "server.h"

static int create_team_eggs(team_t *team, int max_clients, map_t *map)
{
    int i;
    egg_t *egg;

    for (i = 0; i < max_clients; i++) {
        egg = egg_create(i + 1, rand() % map->width, rand() % map->height,
        team->id);
        if (!egg) {
            team_destroy(team);
            return 0;
        }
        egg->next = team->eggs;
        team->eggs = egg;
    }
    return 1;
}

team_t *team_create(const char *name, int id, int max_clients, map_t *map)
{
    team_t *team = malloc(sizeof(team_t));

    if (!team || !name)
        return NULL;
    strncpy(team->name, name, MAX_TEAM_NAME - 1);
    team->name[MAX_TEAM_NAME - 1] = '\0';
    team->id = id;
    team->max_clients = max_clients;
    team->connected_clients = 0;
    team->eggs = NULL;
    team->players = NULL;
    team->next = NULL;
    if (!create_team_eggs(team, max_clients, map))
        return NULL;
    return team;
}

void team_destroy(team_t *team)
{
    egg_t *egg;
    egg_t *next_egg;
    player_t *player;
    player_t *next_player;

    if (!team)
        return;
    egg = team->eggs;
    while (egg) {
        next_egg = egg->next;
        egg_destroy(egg);
        egg = next_egg;
    }
    player = team->players;
    while (player) {
        next_player = player->next;
        player_destroy(player);
        player = next_player;
    }
    free(team);
}

bool team_add_client(team_t *team)
{
    if (!team || team->connected_clients >= team->max_clients)
        return false;
    team->connected_clients++;
    return true;
}

void team_remove_client(team_t *team)
{
    if (!team || team->connected_clients <= 0)
        return;
    team->connected_clients--;
}
