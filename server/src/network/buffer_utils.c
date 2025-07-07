/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-raphael.chanliongco
** File description:
** buffer_utils.c
*/

#include "server.h"

size_t buffer_available(const circular_buffer_t *buf)
{
    if (!buf) {
        return 0;
    }
    if (buf->full) {
        return buf->size;
    }
    if (buf->write_pos >= buf->read_pos) {
        return buf->write_pos - buf->read_pos;
    }
    return buf->size - buf->read_pos + buf->write_pos;
}
