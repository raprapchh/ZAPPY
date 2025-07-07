/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** server_starvation.c
*/

#include "server.h"

static void handle_player_death(server_t *server, player_t *player)
{
    client_t *client;

    broadcast_player_death_to_guis(server, player);
    client = server->clients;
    while (client) {
        if (client->player == player) {
            client_send(client, "dead\n");
            break;
        }
        client = client->next;
    }
}

static void check_player_starvation(server_t *server, player_t *player,
    uint64_t current_time)
{
    bool food_consumed = false;
    bool survived = player_consume_food_and_notify(player, current_time,
        server->config.freq, &food_consumed);

    if (food_consumed) {
        broadcast_player_inventory_to_guis(server, player);
    }
    if (!survived) {
        if (player_check_death(player))
            handle_player_death(server, player);
    }
}

static void check_team_starvation(server_t *server, team_t *team,
    uint64_t current_time)
{
    player_t *player = team->players;

    while (player) {
        if (!player->is_dead)
            check_player_starvation(server, player, current_time);
        player = player->next;
    }
}

void server_check_players_starvation(server_t *server)
{
    uint64_t current_time;
    team_t *team;

    if (!server)
        return;
    current_time = get_timestamp();
    team = server->teams;
    while (team) {
        check_team_starvation(server, team, current_time);
        team = team->next;
    }
}
