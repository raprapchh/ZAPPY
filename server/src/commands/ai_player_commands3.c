/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** ai_player_commands2.c
*/

#include "server.h"

static void fork_create_egg(server_t *server, client_t *client)
{
    egg_t *egg;

    server->next_egg_id++;
    egg = egg_create(server->next_egg_id - 1, client->player->x,
        client->player->y, client->team->id);
    if (!egg) {
        client_send(client, "ko\n");
        return;
    }
    egg->next = client->team->eggs;
    client->team->eggs = egg;
    broadcast_egg_laid_to_guis(server, egg, client->team, client->player);
    client_send(client, "ok\n");
}

void execute_fork(server_t *server, client_t *client)
{
    if (!client->player || client->player->is_dead || !client->team) {
        client_send(client, "ko\n");
        return;
    }
    if (team_get_slots(client->team) <= 0) {
        client_send(client, "ko\n");
        return;
    }
    fork_create_egg(server, client);
    client->player->is_busy = false;
}

static bool can_eject_players(server_t *server, client_t *client)
{
    tile_t *tile;

    tile = map_get_tile(server->map, client->player->x, client->player->y);
    if (!tile || !tile->players || tile->players->next == NULL) {
        return false;
    }
    return true;
}

static void move_player_to_normalized_tile(server_t *server, player_t *player,
    int new_x, int new_y)
{
    map_normalize_coords(server->map, &new_x, &new_y);
    map_remove_player(server->map, player);
    player->x = new_x;
    player->y = new_y;
    map_add_player(server->map, player);
    broadcast_player_expelled_to_guis(server, player);
    broadcast_player_position_to_guis(server, player);
}

static void eject_single_player(server_t *server, player_t *ejector,
    player_t *target)
{
    int new_x = target->x;
    int new_y = target->y;

    switch (ejector->orientation) {
        case NORTH:
            new_y--;
            break;
        case EAST:
            new_x++;
            break;
        case SOUTH:
            new_y++;
            break;
        case WEST:
            new_x--;
            break;
    }
    move_player_to_normalized_tile(server, target, new_x, new_y);
}

static void eject_players_from_tile(server_t *server, player_t *ejector)
{
    tile_t *tile = map_get_tile(server->map, ejector->x, ejector->y);
    player_t *current;
    player_t *next;

    if (!tile || !tile->players)
        return;
    current = tile->players;
    while (current) {
        next = current->next;
        if (current != ejector)
            eject_single_player(server, ejector, current);
        current = next;
    }
}

void execute_eject(server_t *server, client_t *client)
{
    if (!client->player || client->player->is_dead) {
        client_send(client, "ko\n");
        return;
    }
    if (!can_eject_players(server, client)) {
        client_send(client, "ko\n");
        return;
    }
    eject_players_from_tile(server, client->player);
    client_send(client, "ok\n");
    client->player->is_busy = false;
}

void execute_incantation(server_t *server, client_t *client)
{
    char response[64];
    bool success = false;

    if (client->player && !client->player->is_dead) {
        if (player_can_level_up(client->player, server->map)) {
            player_level_up(client->player, server->map);
            snprintf(response, sizeof(response), "Current level: %d\n",
                client->player->level);
            client_send(client, response);
            broadcast_player_level_to_guis(server, client->player);
            broadcast_player_inventory_to_guis(server, client->player);
            success = true;
        } else {
            client_send(client, "ko\n");
        }
    } else {
        client_send(client, "ko\n");
    }
    broadcast_incantation_end_to_guis(server, client->player, success);
    client->player->is_busy = false;
}
