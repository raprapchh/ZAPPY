/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** config_parsing.c
*/

#include "server.h"

bool parse_port(const char *str, int *port)
{
    char *endptr;
    long value = strtol(str, &endptr, 10);

    if (*endptr != '\0' || value <= 0 || value > 65535) {
        fprintf(stderr, "Error: Invalid port number\n");
        return false;
    }
    *port = (int)value;
    return true;
}

bool parse_dimension(const char *str, int *dim, const char *name)
{
    char *endptr;
    long value = strtol(str, &endptr, 10);

    if (*endptr != '\0' || value <= 0 || value > 1000) {
        fprintf(stderr, "Error: Invalid %s\n", name);
        return false;
    }
    *dim = (int)value;
    return true;
}

bool parse_clients_nb(const char *str, int *clients_nb)
{
    char *endptr;
    long value = strtol(str, &endptr, 10);

    if (*endptr != '\0' || value <= 0 || value > 100) {
        fprintf(stderr, "Error: Invalid number of clients\n");
        return false;
    }
    *clients_nb = (int)value;
    return true;
}

bool parse_freq(const char *str, int *freq)
{
    char *endptr;
    long value = strtol(str, &endptr, 10);

    if (*endptr != '\0' || value <= 0 || value > 10000) {
        fprintf(stderr, "Error: Invalid frequency\n");
        return false;
    }
    *freq = (int)value;
    return true;
}

static bool is_valid_team_name(const char *name)
{
    if (strlen(name) >= MAX_TEAM_NAME || strcmp(name, "GRAPHIC") == 0)
        return false;
    return true;
}

bool parse_team_names(int argc, char **argv, int start_idx, config_t *config)
{
    int i = start_idx;

    config->team_count = 0;
    while (i < argc && argv[i][0] != '-' && config->team_count < MAX_TEAMS) {
        if (!is_valid_team_name(argv[i])) {
            fprintf(stderr, "Error: Team name too long or invalid\n");
            return false;
        }
        strcpy(config->team_names[config->team_count], argv[i]);
        config->team_count++;
        i++;
    }
    if (config->team_count == 0) {
        fprintf(stderr, "Error: At least one team name is required\n");
        return false;
    }
    return true;
}
