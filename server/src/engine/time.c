/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** time.c
*/

#include "server.h"

event_queue_t *event_queue_create(void)
{
    event_queue_t *queue = malloc(sizeof(event_queue_t));

    if (!queue)
        return NULL;
    queue->head = NULL;
    queue->count = 0;
    return queue;
}

void event_queue_destroy(event_queue_t *queue)
{
    event_t *event;
    event_t *next;

    if (!queue)
        return;
    event = queue->head;
    while (event) {
        next = event->next;
        free(event);
        event = next;
    }
    free(queue);
}

bool event_queue_add(event_queue_t *queue, uint64_t when, int client_fd,
    const char *command)
{
    event_t *new_event;
    event_t **current;

    if (!queue || !command)
        return false;
    new_event = malloc(sizeof(event_t));
    if (!new_event)
        return false;
    new_event->execute_at = when;
    new_event->client_fd = client_fd;
    strncpy(new_event->command, command, CMD_MAX - 1);
    new_event->command[CMD_MAX - 1] = '\0';
    new_event->next = NULL;
    current = &queue->head;
    while (*current && (*current)->execute_at <= when)
        current = &(*current)->next;
    new_event->next = *current;
    *current = new_event;
    queue->count++;
    return true;
}

event_t *event_queue_get_ready(event_queue_t *queue, uint64_t now)
{
    if (!queue || !queue->head)
        return NULL;
    if (queue->head->execute_at <= now)
        return queue->head;
    return NULL;
}

void event_queue_remove(event_queue_t *queue, event_t *event)
{
    event_t **current;

    if (!queue || !event)
        return;
    current = &queue->head;
    while (*current && *current != event)
        current = &(*current)->next;
    if (*current) {
        *current = event->next;
        queue->count--;
        free(event);
    }
}
