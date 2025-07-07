/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** gui_send.c
*/

#include "server.h"

void gui_send_player_new(client_t *client, player_t *player, team_t *team)
{
    char response[128];

    if (!client || !player || !team)
        return;
    snprintf(response, sizeof(response), "pnw #%d %d %d %d %d %s\n",
        player->id, player->x, player->y, player->orientation,
        player->level, team->name);
    client_send(client, response);
}

void gui_send_player_position(client_t *client, player_t *player)
{
    char response[64];

    if (!client || !player)
        return;
    snprintf(response, sizeof(response), "ppo #%d %d %d %d\n",
        player->id, player->x, player->y, player->orientation);
    client_send(client, response);
}

void gui_send_player_level(client_t *client, player_t *player)
{
    char response[64];

    if (!client || !player)
        return;
    snprintf(response, sizeof(response), "plv #%d %d\n", player->id,
        player->level);
    client_send(client, response);
}

void gui_send_player_inventory(client_t *client, player_t *player)
{
    char response[128];

    if (!client || !player)
        return;
    snprintf(response, sizeof(response),
        "pin #%d %d %d %d %d %d %d %d %d %d\n", player->id, player->x,
        player->y, player->inventory[FOOD], player->inventory[LINEMATE],
        player->inventory[DERAUMERE], player->inventory[SIBUR],
        player->inventory[MENDIANE], player->inventory[PHIRAS],
        player->inventory[THYSTAME]);
    client_send(client, response);
}
