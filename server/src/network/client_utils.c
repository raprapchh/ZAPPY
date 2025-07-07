/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** client.c
*/

#include "server.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>

void client_destroy(client_t *client)
{
    if (!client)
        return;
    if (client->fd != -1)
        socket_close(client->fd);
    buffer_destroy(client->in_buf);
    buffer_destroy(client->out_buf);
    command_queue_destroy(client->cmd_queue);
    free(client);
}

bool client_read(client_t *client)
{
    char temp_buf[BUFFER_SIZE];
    ssize_t bytes_read;

    if (!client || client->fd == -1)
        return false;
    bytes_read = recv(client->fd, temp_buf, sizeof(temp_buf), 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0 || (errno != EAGAIN && errno != EWOULDBLOCK))
            return false;
        return true;
    }
    client->bytes_received += bytes_read;
    return buffer_write(client->in_buf, temp_buf, bytes_read);
}

static size_t get_write_amount(client_t *client)
{
    size_t available;

    available = buffer_available(client->out_buf);
    if (available == 0)
        return 0;
    if (available > BUFFER_SIZE)
        available = BUFFER_SIZE;
    return available;
}

static bool handle_partial_write(client_t *client, char *temp_buf,
    size_t available, ssize_t bytes_written)
{
    if ((size_t)bytes_written < available) {
        buffer_write(client->out_buf, temp_buf + bytes_written,
            available - bytes_written);
    }
    client->bytes_sent += bytes_written;
    return true;
}

bool client_write(client_t *client)
{
    char temp_buf[BUFFER_SIZE];
    size_t available;
    ssize_t bytes_written;

    if (!client || client->fd == -1)
        return false;
    available = get_write_amount(client);
    if (available == 0)
        return true;
    available = buffer_read(client->out_buf, temp_buf, available);
    bytes_written = send(client->fd, temp_buf, available, MSG_NOSIGNAL);
    if (bytes_written <= 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            return false;
        }
        buffer_write(client->out_buf, temp_buf, available);
        return true;
    }
    return handle_partial_write(client, temp_buf, available, bytes_written);
}

void client_send(client_t *client, const char *message)
{
    size_t len;
    size_t attempts = 0;
    const size_t max_attempts = 100;

    if (!client || !message)
        return;
    len = strlen(message);
    while (attempts < max_attempts) {
        if (buffer_write(client->out_buf, message, len)) {
            return;
        }
        if (buffer_space(client->out_buf) < len) {
            client_write(client);
            attempts++;
            usleep(1000);
        } else {
            break;
        }
    }
}

static char *create_command_string(client_t *client)
{
    char *command;
    size_t cmd_len;

    command = malloc(client->partial_len + 1);
    if (!command) {
        client->partial_len = 0;
        return NULL;
    }
    memcpy(command, client->partial_cmd, client->partial_len);
    command[client->partial_len] = '\0';
    cmd_len = client->partial_len;
    client->partial_len = 0;
    if (cmd_len > 0 && command[cmd_len - 1] == '\r')
        command[cmd_len - 1] = '\0';
    return command;
}

static char *process_newline_char(client_t *client)
{
    if (client->partial_len == 0)
        return NULL;
    return create_command_string(client);
}

static char *process_character(client_t *client, char ch)
{
    if (ch == '\n')
        return process_newline_char(client);
    if (client->partial_len < CMD_MAX - 1) {
        client->partial_cmd[client->partial_len] = ch;
        client->partial_len++;
    }
    return NULL;
}

char *client_get_command(client_t *client)
{
    char ch;
    char *result;

    if (!client)
        return NULL;
    while (buffer_read(client->in_buf, &ch, 1) == 1) {
        result = process_character(client, ch);
        if (result)
            return result;
    }
    return NULL;
}
