/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** config_validation.c
*/

#include "server.h"

static bool validate_port(config_t *config)
{
    if (config->port == 0) {
        fprintf(stderr, "Error: Port is required\n");
        return false;
    }
    return true;
}

static bool validate_dimensions(config_t *config)
{
    if (config->width == 0) {
        fprintf(stderr, "Error: Width is required\n");
        return false;
    }
    if (config->height == 0) {
        fprintf(stderr, "Error: Height is required\n");
        return false;
    }
    return true;
}

static bool validate_teams(config_t *config)
{
    if (config->team_count == 0) {
        fprintf(stderr, "Error: At least one team name is required\n");
        return false;
    }
    return true;
}

static bool validate_clients_and_freq(config_t *config)
{
    if (config->clients_nb == 0) {
        fprintf(stderr, "Error: Number of clients is required\n");
        return false;
    }
    if (config->freq == 0) {
        fprintf(stderr, "Error: Frequency is required\n");
        return false;
    }
    return true;
}

bool validate_config(config_t *config)
{
    if (!validate_port(config))
        return false;
    if (!validate_dimensions(config))
        return false;
    if (!validate_teams(config))
        return false;
    if (!validate_clients_and_freq(config))
        return false;
    return true;
}
