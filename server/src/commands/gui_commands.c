/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** gui_handlers.c
*/

#include "server.h"


static void handle_sgt(server_t *server, client_t *client)
{
    char response[64];

    snprintf(response, sizeof(response), "sgt %d\n", server->config.freq);
    client_send(client, response);
}

static void handle_sst(server_t *server, client_t *client, const char *args)
{
    int new_freq = 0;
    char response[64];

    if (sscanf(args, "%d", &new_freq) == 1 && new_freq > 0) {
        server->config.freq = new_freq;
        snprintf(response, sizeof(response), "sgt %d\n", new_freq);
        client_send(client, response);
    } else {
        client_send(client, "sbp\n");
    }
}

static bool handle_map_commands(server_t *server, client_t *client,
    const char *cmd, const char *args)
{
    if (strcmp(cmd, "msz") == 0) {
        handle_msz(server, client);
        return true;
    }
    if (strcmp(cmd, "bct") == 0) {
        handle_bct(server, client, args);
        return true;
    }
    if (strcmp(cmd, "mct") == 0) {
        handle_mct(server, client);
        return true;
    }
    return false;
}

static bool handle_player_info_commands(server_t *server, client_t *client,
    const char *cmd, const char *args)
{
    if (strcmp(cmd, "tna") == 0) {
        handle_tna(server, client);
        return true;
    }
    if (strcmp(cmd, "ppo") == 0) {
        handle_ppo(server, client, args);
        return true;
    }
    if (strcmp(cmd, "plv") == 0) {
        handle_plv(server, client, args);
        return true;
    }
    if (strcmp(cmd, "pin") == 0) {
        handle_pin(server, client, args);
        return true;
    }
    return false;
}

static bool handle_server_commands(server_t *server, client_t *client,
    const char *cmd, const char *args)
{
    if (strcmp(cmd, "sgt") == 0) {
        handle_sgt(server, client);
        return true;
    }
    if (strcmp(cmd, "sst") == 0) {
        handle_sst(server, client, args);
        return true;
    }
    return false;
}

player_t *find_player_by_id(server_t *server, int player_id)
{
    client_t *current = server->clients;

    while (current) {
        if (current->player && current->player->id == player_id) {
            return current->player;
        }
        current = current->next;
    }
    return NULL;
}

void handle_gui_command(server_t *server, client_t *client,
    const char *command)
{
    char cmd[64];
    char args[256] = {0};

    if (!client || !command)
        return;
    sscanf(command, "%s %[^\n]", cmd, args);
    if (handle_map_commands(server, client, cmd, args))
        return;
    if (handle_player_info_commands(server, client, cmd, args))
        return;
    if (handle_server_commands(server, client, cmd, args))
        return;
    client_send(client, "suc\n");
}

void handle_mct(server_t *server, client_t *client)
{
    gui_send_map_content(client, server->map);
}

void handle_tna(server_t *server, client_t *client)
{
    gui_send_team_names(client, server->teams);
}
