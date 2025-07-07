/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** ai_player_commands.c
*/

#include "server.h"

void cmd_look(server_t *server, client_t *client)
{
    char vision[4096];
    size_t len;

    if (!client->player || client->player->is_dead) {
        client_send(client, "ko\n");
        return;
    }
    player_get_vision(client->player, server->map, vision);
    len = strlen(vision);
    if (len > 0 && vision[len - 1] != '\n') {
        strcat(vision, "\n");
    }
    client_send(client, vision);
}

void cmd_inventory(server_t *server, client_t *client)
{
    uint64_t execute_time = get_timestamp() + (1000 / server->config.freq);

    if (!client->player || client->player->is_dead) {
        client_send(client, "ko\n");
        return;
    }
    client->player->is_busy = true;
    client->player->busy_until = execute_time;
    event_queue_add(server->events, execute_time, client->fd,
        "inventory_exec");
}

void cmd_connect_nbr(server_t *server, client_t *client)
{
    char response[64];

    (void)server;
    if (!client->player || client->player->is_dead || !client->team) {
        client_send(client, "ko\n");
        return;
    }
    snprintf(response, sizeof(response), "%d\n", team_get_slots(client->team));
    client_send(client, response);
}

void execute_look(server_t *server, client_t *client)
{
    char vision[4096];
    size_t len;

    if (client->player && !client->player->is_dead) {
        player_get_vision(client->player, server->map, vision);
        len = strlen(vision);
        if (len > 0 && vision[len - 1] != '\n') {
            strcat(vision, "\n");
        }
        client_send(client, vision);
        client->player->is_busy = false;
    } else {
        client_send(client, "ko\n");
    }
}

void execute_inventory(server_t *server, client_t *client)
{
    char inventory[512];
    size_t len;

    (void)server;
    if (client->player && !client->player->is_dead) {
        player_get_inventory(client->player, inventory);
        len = strlen(inventory);
        if (len > 0 && inventory[len - 1] != '\n') {
            strcat(inventory, "\n");
        }
        client_send(client, inventory);
        client->player->is_busy = false;
    } else {
        client_send(client, "ko\n");
    }
}
