/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** buffer.c
*/

#include "server.h"

size_t buffer_space(const circular_buffer_t *buf)
{
    if (!buf) {
        return 0;
    }
    return buf->size - buffer_available(buf);
}

circular_buffer_t *buffer_create(size_t size)
{
    circular_buffer_t *buf = malloc(sizeof(circular_buffer_t));

    if (!buf) {
        return NULL;
    }
    buf->data = malloc(size);
    if (!buf->data) {
        free(buf);
        return NULL;
    }
    buf->size = size;
    buf->read_pos = 0;
    buf->write_pos = 0;
    buf->full = false;
    return buf;
}

void buffer_destroy(circular_buffer_t *buf)
{
    if (buf) {
        free(buf->data);
        free(buf);
    }
}

bool buffer_write(circular_buffer_t *buf, const char *data, size_t len)
{
    if (!buf || !data || len == 0) {
        return false;
    }
    if (buffer_space(buf) < len) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        buf->data[buf->write_pos] = data[i];
        buf->write_pos = (buf->write_pos + 1) % buf->size;
        if (buf->write_pos == buf->read_pos) {
            buf->full = true;
        }
    }
    return true;
}

size_t buffer_read(circular_buffer_t *buf, char *data, size_t len)
{
    size_t read_count = 0;
    size_t available = buffer_available(buf);

    if (!buf || !data || len == 0) {
        return 0;
    }
    if (len > available) {
        len = available;
    }
    for (size_t i = 0; i < len; i++) {
        data[i] = buf->data[buf->read_pos];
        buf->read_pos = (buf->read_pos + 1) % buf->size;
        read_count++;
    }
    buf->full = false;
    return read_count;
}
