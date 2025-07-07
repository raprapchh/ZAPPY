/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** server_handler.c
*/

#include "server.h"

static void handle_graphic_client(server_t *server, client_t *client)
{
    client->type = CLIENT_GUI;
    client->state = STATE_CONNECTED;
    gui_send_map_size(client, server->map);
    gui_send_map_content(client, server->map);
    gui_send_team_names(client, server->teams);
    gui_send_all_players(client, server->teams);
}

static bool setup_ai_client(server_t *server, client_t *client, team_t *team)
{
    player_t *player = create_and_setup_player(server, team);

    if (!player)
        return false;
    client->player = player;
    player->next = team->players;
    team->players = player;
    map_add_player(server->map, player);
    broadcast_player_new_to_guis(server, player, team);
    return true;
}

static void handle_team_connection(server_t *server, client_t *client,
    const char *command)
{
    team_t *team = team_find_by_name(server->teams, command);
    char response[64];

    if (team && team_get_slots(team) > 0) {
        client->type = CLIENT_AI;
        client->team = team;
        client->state = STATE_CONNECTED;
        team_add_client(team);
        if (setup_ai_client(server, client, team)) {
            snprintf(response, sizeof(response), "%d\n", team_get_slots(team));
            client_send(client, response);
            snprintf(response, sizeof(response), "%d %d\n",
                server->map->width, server->map->height);
            client_send(client, response);
        }
    } else {
        client_send(client, "ko\n");
        server_remove_client(server, client);
    }
}

static bool process_waiting_team_command(server_t *server, client_t *client,
    const char *command)
{
    if (strcmp(command, "GRAPHIC") == 0) {
        handle_graphic_client(server, client);
        return true;
    }
    handle_team_connection(server, client, command);
    return client->state == STATE_CONNECTED;
}

static void process_connected_client_command(server_t *server,
    client_t *client, char *command)
{
    if (client->type == CLIENT_AI) {
        if (!command_queue_is_full(client->cmd_queue)) {
            command_queue_push(client->cmd_queue, command);
        }
    } else if (client->type == CLIENT_GUI) {
        handle_gui_command(server, client, command);
    }
}

void server_handle_client_data(server_t *server, client_t *client)
{
    char *command;

    if (!client_read(client)) {
        server_remove_client(server, client);
        return;
    }
    command = client_get_command(client);
    while (command != NULL) {
        if (client->state == STATE_WAITING_TEAM &&
            !process_waiting_team_command(server, client, command)) {
            free(command);
            return;
        }
        if (client->state == STATE_CONNECTED) {
            process_connected_client_command(server, client, command);
        }
        free(command);
        command = client_get_command(client);
    }
}

static client_t *find_client_by_fd(client_t *clients, int fd)
{
    client_t *client = clients;

    while (client) {
        if (client->fd == fd)
            return client;
        client = client->next;
    }
    return NULL;
}

static void process_client_command_queue(server_t *server, client_t *client)
{
    command_t *cmd;

    if (client->type != CLIENT_AI || client->state != STATE_CONNECTED)
        return;
    if (player_is_frozen(client->player))
        return;
    if (client->player->is_busy && get_timestamp() <
        client->player->busy_until)
        return;
    cmd = command_queue_pop(client->cmd_queue);
    if (cmd) {
        handle_ai_command(server, client, cmd->data);
        free(cmd);
    }
}

static void process_events_and_clients(server_t *server, uint64_t now)
{
    event_t *event;
    client_t *client;

    event = event_queue_get_ready(server->events, now);
    while (event != NULL) {
        client = find_client_by_fd(server->clients, event->client_fd);
        if (client && client->state == STATE_CONNECTED &&
            client->type == CLIENT_AI) {
            handle_ai_command(server, client, event->command);
        }
        event_queue_remove(server->events, event);
        event = event_queue_get_ready(server->events, now);
    }
    client = server->clients;
    while (client) {
        process_client_command_queue(server, client);
        client = client->next;
    }
}

void server_process_events(server_t *server)
{
    uint64_t now = get_timestamp();
    client_t *client;

    process_events_and_clients(server, now);
    if (now - server->map->last_spawn >=
        (uint64_t)(20 * 1000 / server->config.freq)) {
        map_maintain_resources(server->map, server->config.freq, server);
        server->map->last_spawn = now;
    }
    server_check_players_starvation(server);
    client = server->clients;
    while (client) {
        if (buffer_available(client->out_buf) > 0) {
            client_write(client);
        }
        client = client->next;
    }
}
