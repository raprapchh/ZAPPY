/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** ai_resource_commands.c
*/

#include "server.h"

resource_type_t parse_resource(const char *name)
{
    if (!name)
        return -1;
    if (strcmp(name, "food") == 0)
        return FOOD;
    if (strcmp(name, "linemate") == 0)
        return LINEMATE;
    if (strcmp(name, "deraumere") == 0)
        return DERAUMERE;
    if (strcmp(name, "sibur") == 0)
        return SIBUR;
    if (strcmp(name, "mendiane") == 0)
        return MENDIANE;
    if (strcmp(name, "phiras") == 0)
        return PHIRAS;
    if (strcmp(name, "thystame") == 0)
        return THYSTAME;
    return -1;
}

void cmd_take(server_t *server, client_t *client, const char *object)
{
    uint64_t execute_time = get_timestamp() + (7000 / server->config.freq);
    char command[CMD_MAX];

    if (!client->player || client->player->is_dead) {
        client_send(client, "ko\n");
        return;
    }
    if ((int)parse_resource(object) == -1) {
        client_send(client, "ko\n");
        return;
    }
    client->player->is_busy = true;
    client->player->busy_until = execute_time;
    snprintf(command, sizeof(command), "take_exec %s", object);
    event_queue_add(server->events, execute_time, client->fd, command);
}

void cmd_set(server_t *server, client_t *client, const char *object)
{
    uint64_t execute_time = get_timestamp() + (7000 / server->config.freq);
    char command[CMD_MAX];

    if (!client->player || client->player->is_dead) {
        client_send(client, "ko\n");
        return;
    }
    if ((int)parse_resource(object) == -1) {
        client_send(client, "ko\n");
        return;
    }
    client->player->is_busy = true;
    client->player->busy_until = execute_time;
    snprintf(command, sizeof(command), "set_exec %s", object);
    event_queue_add(server->events, execute_time, client->fd, command);
}

void execute_take(server_t *server, client_t *client, const char *object)
{
    resource_type_t type = parse_resource(object);

    if (!client->player || client->player->is_dead || (int)type == -1) {
        client_send(client, "ko\n");
        client->player->is_busy = false;
        return;
    }
    if (player_take_resource(client->player, server->map, type)) {
        client_send(client, "ok\n");
        broadcast_player_inventory_to_guis(server, client->player);
        broadcast_tile_content_to_guis(server, client->player->x,
            client->player->y);
    } else {
        client_send(client, "ko\n");
    }
    client->player->is_busy = false;
}

void execute_set(server_t *server, client_t *client, const char *object)
{
    resource_type_t type = parse_resource(object);

    if (!client->player || client->player->is_dead || (int)type == -1) {
        client_send(client, "ko\n");
        client->player->is_busy = false;
        return;
    }
    if (player_drop_resource(client->player, server->map, type)) {
        client_send(client, "ok\n");
        broadcast_player_inventory_to_guis(server, client->player);
        broadcast_tile_content_to_guis(server, client->player->x,
            client->player->y);
    } else {
        client_send(client, "ko\n");
    }
    client->player->is_busy = false;
}
