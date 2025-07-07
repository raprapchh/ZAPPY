/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** server_init.c
*/

#include "server.h"

static bool init_server_basics(server_t *server, config_t *config)
{
    memset(server, 0, sizeof(server_t));
    server->config = *config;
    server->running = true;
    server->state = GAME_STARTING;
    server->start_time = get_timestamp();
    server->next_player_id = 1;
    server->next_egg_id = 1;
    return true;
}

static bool init_server_socket(server_t *server, config_t *config)
{
    server->listen_fd = socket_create_server(config->port);
    if (server->listen_fd == -1)
        return false;
    server->poll_fds[0].fd = server->listen_fd;
    server->poll_fds[0].events = POLLIN;
    server->poll_count = 1;
    return true;
}

static bool init_server_teams(server_t *server, config_t *config)
{
    team_t *team;

    for (int i = 0; i < config->team_count; i++) {
        team = team_create(config->team_names[i], i,
            config->clients_nb, server->map);
        if (!team) {
            server_cleanup(server);
            return false;
        }
        team->next = server->teams;
        server->teams = team;
    }
    return true;
}

bool server_init(server_t *server, config_t *config)
{
    if (!init_server_basics(server, config))
        return false;
    if (!init_server_socket(server, config))
        return false;
    server->map = map_create(config->width, config->height);
    if (!server->map) {
        socket_close(server->listen_fd);
        return false;
    }
    if (!init_server_teams(server, config))
        return false;
    server->events = event_queue_create();
    if (!server->events) {
        server_cleanup(server);
        return false;
    }
    map_spawn_resources(server->map, config->freq, server);
    server->map->last_spawn = get_timestamp();
    server->state = GAME_RUNNING;
    return true;
}

void server_cleanup(server_t *server)
{
    client_t *client = server->clients;
    team_t *team = server->teams;
    void *next;

    while (client) {
        next = client->next;
        client_destroy(client);
        client = next;
    }
    while (team) {
        next = team->next;
        team_destroy(team);
        team = next;
    }
    if (server->map)
        map_destroy(server->map);
    if (server->events)
        event_queue_destroy(server->events);
    if (server->listen_fd != -1)
        socket_close(server->listen_fd);
}
