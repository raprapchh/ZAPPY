/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** server.c
*/

#include "server.h"
#include <time.h>

static void setup_signals(void)
{
    signal(SIGPIPE, SIG_IGN);
}

static void handle_new_connection(server_t *server)
{
    int client_fd = socket_accept_client(server->listen_fd);
    client_t *client;
    struct sockaddr_in addr = {0};

    if (client_fd != -1) {
        client = client_create(client_fd, &addr);
        if (client) {
            client->state = STATE_WAITING_TEAM;
            server_add_client(server, client);
        }
    }
}

static client_t *find_client_by_fd(server_t *server, int fd)
{
    client_t *client = server->clients;

    while (client && client->fd != fd)
        client = client->next;
    return client;
}

static void handle_client_events(server_t *server, size_t *i)
{
    client_t *client;

    if (server->poll_fds[*i].revents & (POLLIN | POLLHUP | POLLERR)) {
        client = find_client_by_fd(server, server->poll_fds[*i].fd);
        if (!client) {
            return;
        }
        if (server->poll_fds[*i].revents & (POLLHUP | POLLERR)) {
            server_remove_client(server, client);
            (*i)--;
            return;
        }
        server_handle_client_data(server, client);
    }
    if (*i < server->poll_count && server->poll_fds[*i].revents & POLLOUT) {
        client = find_client_by_fd(server, server->poll_fds[*i].fd);
        if (client)
            client_write(client);
    }
}

void server_run(server_t *server)
{
    uint64_t now;
    int timeout;
    int ret;

    while (server->running) {
        now = get_timestamp();
        timeout = calculate_timeout(server->events, now);
        ret = poll(server->poll_fds, server->poll_count, timeout);
        if (ret == -1) {
            perror("poll");
            break;
        }
        if (server->poll_fds[0].revents & POLLIN)
            handle_new_connection(server);
        for (size_t i = 1; i < server->poll_count; i++)
            handle_client_events(server, &i);
        server_process_events(server);
    }
}

int main(int argc, char **argv)
{
    config_t config;
    server_t server;

    if (!parse_args(argc, argv, &config)) {
        print_usage(argv[0]);
        return 84;
    }
    setup_signals();
    srand(time(NULL));
    if (!server_init(&server, &config)) {
        perror("Failed to initialize server");
        return 84;
    }
    server_run(&server);
    server_cleanup(&server);
    return 0;
}
