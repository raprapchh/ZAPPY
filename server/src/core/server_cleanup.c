/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** server_cleanup.c
*/

#include "server.h"

static void cleanup_dead_player(server_t *server, player_t **current)
{
    player_t *dead_player = *current;

    *current = dead_player->next;
    map_remove_player(server->map, dead_player);
    player_destroy(dead_player);
}

static void cleanup_team_players(server_t *server, team_t *team)
{
    player_t **current = &team->players;

    while (*current) {
        if ((*current)->is_dead)
            cleanup_dead_player(server, current);
        else
            current = &(*current)->next;
    }
}

void server_cleanup_dead_players(server_t *server)
{
    team_t *team;

    if (!server)
        return;
    team = server->teams;
    while (team) {
        cleanup_team_players(server, team);
        team = team->next;
    }
}
