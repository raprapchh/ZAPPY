/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** gui_send.c
*/

#include "server.h"

void gui_send_map_size(client_t *client, map_t *map)
{
    char response[64];

    if (!client || !map)
        return;
    snprintf(response, sizeof(response), "msz %d %d\n", map->width,
        map->height);
    client_send(client, response);
}

void gui_send_tile_content(client_t *client, map_t *map, int x, int y)
{
    tile_t *tile;
    char response[256];

    if (!client || !map)
        return;
    tile = map_get_tile(map, x, y);
    if (!tile)
        return;
    snprintf(response, sizeof(response), "bct %d %d %d %d %d %d %d %d %d\n",
        x, y, tile->resource_count[FOOD], tile->resource_count[LINEMATE],
        tile->resource_count[DERAUMERE], tile->resource_count[SIBUR],
        tile->resource_count[MENDIANE], tile->resource_count[PHIRAS],
        tile->resource_count[THYSTAME]);
    client_send(client, response);
}

void gui_send_map_content(client_t *client, map_t *map)
{
    if (!client || !map)
        return;
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++)
            gui_send_tile_content(client, map, x, y);
    }
}

void gui_send_team_names(client_t *client, team_t *teams)
{
    team_t *current = teams;
    char response[128];

    if (!client)
        return;
    while (current) {
        snprintf(response, sizeof(response), "tna %s\n", current->name);
        client_send(client, response);
        current = current->next;
    }
}

void gui_send_all_players(client_t *client, team_t *teams)
{
    team_t *current_team = teams;
    player_t *current_player;

    if (!client)
        return;
    while (current_team) {
        current_player = current_team->players;
        while (current_player) {
            gui_send_player_new(client, current_player, current_team);
            current_player = current_player->next;
        }
        current_team = current_team->next;
    }
}
