/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** gui_send3.c
*/

#include "server.h"

void gui_send_egg_laid(client_t *client, egg_t *egg, team_t *team)
{
    char response[128];

    if (!client || !egg || !team)
        return;
    snprintf(response, sizeof(response), "enw #%d #%d %d %d\n",
        egg->id, 0, egg->x, egg->y);
    client_send(client, response);
}

void gui_send_incantation_start(client_t *client, player_t *player, int level)
{
    char response[128];

    if (!client || !player)
        return;
    snprintf(response, sizeof(response), "pic %d %d %d #%d\n",
        player->x, player->y, level, player->id);
    client_send(client, response);
}

void gui_send_incantation_end(client_t *client, player_t *player, bool success)
{
    char response[128];

    if (!client || !player)
        return;
    snprintf(response, sizeof(response), "pie %d %d %d\n",
        player->x, player->y, success ? 1 : 0);
    client_send(client, response);
}

void gui_send_player_expelled(client_t *client, player_t *player)
{
    char response[64];

    if (!client || !player)
        return;
    snprintf(response, sizeof(response), "pex #%d\n", player->id);
    client_send(client, response);
}
