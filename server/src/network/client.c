/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** client.c
*/

#include "server.h"

command_queue_t *command_queue_create(size_t max_size)
{
    command_queue_t *queue = malloc(sizeof(command_queue_t));

    if (!queue)
        return NULL;
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
    queue->max_size = max_size;
    return queue;
}

void command_queue_destroy(command_queue_t *queue)
{
    command_t *cmd;

    if (!queue)
        return;
    while (queue->head) {
        cmd = queue->head;
        queue->head = cmd->next;
        free(cmd);
    }
    free(queue);
}

static void init_command_data(command_t *command, const char *cmd)
{
    strncpy(command->data, cmd, CMD_MAX - 1);
    command->data[CMD_MAX - 1] = '\0';
    command->len = strlen(command->data);
    command->received_at = get_timestamp();
    command->next = NULL;
}

static void add_command_to_queue(command_queue_t *queue, command_t *command)
{
    if (!queue->head) {
        queue->head = command;
        queue->tail = command;
    } else {
        queue->tail->next = command;
        queue->tail = command;
    }
    queue->count++;
}

bool command_queue_push(command_queue_t *queue, const char *cmd)
{
    command_t *command;

    if (!queue || !cmd || queue->count >= queue->max_size)
        return false;
    command = malloc(sizeof(command_t));
    if (!command)
        return false;
    init_command_data(command, cmd);
    add_command_to_queue(queue, command);
    return true;
}

command_t *command_queue_pop(command_queue_t *queue)
{
    command_t *cmd;

    if (!queue || !queue->head)
        return NULL;
    cmd = queue->head;
    queue->head = cmd->next;
    if (!queue->head)
        queue->tail = NULL;
    queue->count--;
    cmd->next = NULL;
    return cmd;
}

bool command_queue_is_full(const command_queue_t *queue)
{
    return queue && queue->count >= queue->max_size;
}
