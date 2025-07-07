/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** ai_player_commands2.c
*/

#include "server.h"

void cmd_broadcast(server_t *server, client_t *client, const char *text)
{
    uint64_t execute_time = get_timestamp() + (7000 / server->config.freq);
    char command[CMD_MAX];

    if (!client->player || client->player->is_dead) {
        client_send(client, "ko\n");
        return;
    }
    client->player->is_busy = true;
    client->player->busy_until = execute_time;
    snprintf(command, sizeof(command), "broadcast_exec %s", text);
    event_queue_add(server->events, execute_time, client->fd, command);
}

void cmd_fork(server_t *server, client_t *client)
{
    uint64_t execute_time = get_timestamp() + (42000 / server->config.freq);

    if (!client->player || client->player->is_dead || !client->team) {
        client_send(client, "ko\n");
        return;
    }
    if (team_get_slots(client->team) <= 0) {
        client_send(client, "ko\n");
        return;
    }
    client->player->is_busy = true;
    client->player->busy_until = execute_time;
    event_queue_add(server->events, execute_time, client->fd, "fork_exec");
}

void cmd_eject(server_t *server, client_t *client)
{
    uint64_t execute_time = get_timestamp() + (7000 / server->config.freq);

    if (!client->player || client->player->is_dead) {
        client_send(client, "ko\n");
        return;
    }
    client->player->is_busy = true;
    client->player->busy_until = execute_time;
    event_queue_add(server->events, execute_time, client->fd, "eject_exec");
}

void cmd_incantation(server_t *server, client_t *client)
{
    uint64_t execute_time;

    execute_time = get_timestamp() + (300000 / server->config.freq);
    if (!client->player || client->player->is_dead) {
        client_send(client, "ko\n");
        return;
    }
    if (!player_can_level_up(client->player, server->map)) {
        client_send(client, "ko\n");
        return;
    }
    broadcast_incantation_start_to_guis(server, client->player,
        client->player->level);
    client->player->is_busy = true;
    client->player->busy_until = execute_time;
    event_queue_add(server->events, execute_time, client->fd,
        "incantation_exec");
    client_send(client, "Elevation underway\n");
}

void execute_broadcast(server_t *server, client_t *client, const char *text)
{
    if (!client->player || client->player->is_dead || !text ||
        strlen(text) == 0) {
        client_send(client, "ko\n");
        return;
    }
    broadcast_message(server, client->player, text);
    client_send(client, "ok\n");
    client->player->is_busy = false;
}
