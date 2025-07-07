/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** socket.c
*/

#include "server.h"

static int create_socket_with_options(void)
{
    int server_fd;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return -1;
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
        &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }
    return server_fd;
}

static int bind_and_listen(int server_fd, int port)
{
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&server_addr,
        sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return -1;
    }
    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        return -1;
    }
    return 0;
}

int socket_create_server(int port)
{
    int server_fd;

    server_fd = create_socket_with_options();
    if (server_fd == -1)
        return -1;
    if (bind_and_listen(server_fd, port) == -1)
        return -1;
    socket_set_nonblocking(server_fd);
    return server_fd;
}

int socket_accept_client(int server_fd)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd;

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
        &client_len);
    if (client_fd == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("accept");
        }
        return -1;
    }
    socket_set_nonblocking(client_fd);
    return client_fd;
}

void socket_set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags == -1) {
        perror("fcntl F_GETFL");
        return;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
    }
}

void socket_close(int fd)
{
    if (fd != -1) {
        close(fd);
    }
}
