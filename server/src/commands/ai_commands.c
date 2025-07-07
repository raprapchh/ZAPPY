/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** ai_commands.c
*/

#include "server.h"

static bool handle_exec_commands(server_t *server, client_t *client,
    const char *command, const char *arg)
{
    (void)arg;
    if (strncmp(command, "forward_exec", 12) == 0) {
        execute_forward(server, client);
        return true;
    }
    if (strncmp(command, "right_exec", 10) == 0) {
        execute_right(server, client);
        return true;
    }
    if (strncmp(command, "left_exec", 9) == 0) {
        execute_left(server, client);
        return true;
    }
    if (strncmp(command, "look_exec", 9) == 0) {
        execute_look(server, client);
        return true;
    }
    return false;
}

static bool handle_exec_commands2(server_t *server, client_t *client,
    const char *command, const char *arg)
{
    (void)arg;
    if (strncmp(command, "inventory_exec", 14) == 0) {
        execute_inventory(server, client);
        return true;
    }
    if (strncmp(command, "broadcast_exec", 14) == 0) {
        execute_broadcast(server, client, command + 15);
        return true;
    }
    if (strncmp(command, "fork_exec", 9) == 0) {
        execute_fork(server, client);
        return true;
    }
    if (strncmp(command, "eject_exec", 10) == 0) {
        execute_eject(server, client);
        return true;
    }
    return false;
}

static bool handle_exec_commands3(server_t *server, client_t *client,
    const char *command, const char *arg)
{
    (void)arg;
    if (strncmp(command, "take_exec", 9) == 0) {
        execute_take(server, client, command + 10);
        return true;
    }
    if (strncmp(command, "set_exec", 8) == 0) {
        execute_set(server, client, command + 9);
        return true;
    }
    if (strncmp(command, "incantation_exec", 16) == 0) {
        execute_incantation(server, client);
        return true;
    }
    return false;
}

static bool handle_movement_commands(server_t *server, client_t *client,
    const char *cmd)
{
    if (strcmp(cmd, "Forward") == 0) {
        cmd_forward(server, client);
        return true;
    }
    if (strcmp(cmd, "Right") == 0) {
        cmd_right(server, client);
        return true;
    }
    if (strcmp(cmd, "Left") == 0) {
        cmd_left(server, client);
        return true;
    }
    if (strcmp(cmd, "Look") == 0) {
        cmd_look(server, client);
        return true;
    }
    return false;
}

static bool handle_player_commands(server_t *server, client_t *client,
    const char *cmd, const char *arg)
{
    (void)arg;
    if (strcmp(cmd, "Inventory") == 0) {
        cmd_inventory(server, client);
        return true;
    }
    if (strcmp(cmd, "Connect_nbr") == 0) {
        cmd_connect_nbr(server, client);
        return true;
    }
    return false;
}

static bool handle_action_commands(server_t *server, client_t *client,
    const char *cmd, const char *arg)
{
    if (strcmp(cmd, "Broadcast") == 0) {
        if (strlen(arg) == 0) {
            return false;
        } else {
            cmd_broadcast(server, client, arg);
            return true;
        }
    }
    if (strcmp(cmd, "Fork") == 0) {
        cmd_fork(server, client);
        return true;
    }
    if (strcmp(cmd, "Eject") == 0) {
        cmd_eject(server, client);
        return true;
    }
    return false;
}

static bool handle_resource_commands(server_t *server, client_t *client,
    const char *cmd, const char *arg)
{
    if (strcmp(cmd, "Take") == 0) {
        cmd_take(server, client, arg);
        return true;
    }
    if (strcmp(cmd, "Set") == 0) {
        cmd_set(server, client, arg);
        return true;
    }
    if (strcmp(cmd, "Incantation") == 0) {
        cmd_incantation(server, client);
        return true;
    }
    return false;
}

void handle_ai_command(server_t *server, client_t *client, const char *command)
{
    char cmd[CMD_MAX];
    char arg[CMD_MAX] = {0};

    if (!client || !command)
        return;
    if (sscanf(command, "%s %[^\n]", cmd, arg) < 1) {
        client_send(client, "ko\n");
        return;
    }
    if (!(handle_exec_commands(server, client, command, arg) ||
            handle_exec_commands2(server, client, command, arg) ||
            handle_exec_commands3(server, client, command, arg) ||
            handle_action_commands(server, client, cmd, arg) ||
            handle_movement_commands(server, client, cmd) ||
            handle_player_commands(server, client, cmd, arg) ||
            handle_resource_commands(server, client, cmd, arg))) {
        client_send(client, "ko\n");
    }
}
