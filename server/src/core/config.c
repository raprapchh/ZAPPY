/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** config.c
*/

#include "server.h"

void print_usage(const char *prog_name)
{
    printf("USAGE: %s -p port -x width -y height -n name1 name2 ... "
            "-c clientsNb -f freq\n", prog_name);
    printf("\toption\t\tdescription\n");
    printf("\t-p port\t\tport number\n");
    printf("\t-x width\twidth of the world\n");
    printf("\t-y height\theight of the world\n");
    printf("\t-n name1 name2 ...\tname of the team\n");
    printf("\t-c clientsNb\tnumber of authorized clients per team\n");
    printf("\t-f freq\t\treciprocal of time unit for execution of actions\n");
}

static bool handle_port_arg(int argc, char **argv, int *i, config_t *config)
{
    if (*i + 1 >= argc || !parse_port(argv[*i + 1], &config->port))
        return false;
    *i += 2;
    return true;
}

static bool handle_width_arg(int argc, char **argv, int *i, config_t *config)
{
    if (*i + 1 >= argc || !parse_dimension(argv[*i + 1],
        &config->width, "width"))
        return false;
    *i += 2;
    return true;
}

static bool handle_height_arg(int argc, char **argv, int *i, config_t *config)
{
    if (*i + 1 >= argc || !parse_dimension(argv[*i + 1],
        &config->height, "height"))
        return false;
    *i += 2;
    return true;
}

static bool handle_team_names_arg(int argc, char **argv, int *i,
    config_t *config)
{
    if (*i + 1 >= argc || !parse_team_names(argc, argv, *i + 1, config))
        return false;
    while (*i + 1 < argc && argv[*i + 1][0] != '-')
        (*i)++;
    (*i)++;
    return true;
}

static bool handle_clients_arg(int argc, char **argv, int *i, config_t *config)
{
    if (*i + 1 >= argc || !parse_clients_nb(argv[*i + 1], &config->clients_nb))
        return false;
    *i += 2;
    return true;
}

static bool handle_freq_arg(int argc, char **argv, int *i, config_t *config)
{
    if (*i + 1 >= argc || !parse_freq(argv[*i + 1], &config->freq))
        return false;
    *i += 2;
    return true;
}

static bool process_argument(int argc, char **argv, int *i, config_t *config)
{
    if (strcmp(argv[*i], "-p") == 0)
        return handle_port_arg(argc, argv, i, config);
    if (strcmp(argv[*i], "-x") == 0)
        return handle_width_arg(argc, argv, i, config);
    if (strcmp(argv[*i], "-y") == 0)
        return handle_height_arg(argc, argv, i, config);
    if (strcmp(argv[*i], "-n") == 0)
        return handle_team_names_arg(argc, argv, i, config);
    if (strcmp(argv[*i], "-c") == 0)
        return handle_clients_arg(argc, argv, i, config);
    if (strcmp(argv[*i], "-f") == 0)
        return handle_freq_arg(argc, argv, i, config);
    fprintf(stderr, "Error: Unknown option %s\n", argv[*i]);
    return false;
}

bool parse_args(int argc, char **argv, config_t *config)
{
    int i = 1;

    if (argc == 2 && strcmp(argv[1], "help") == 0)
        return false;
    memset(config, 0, sizeof(config_t));
    while (i < argc) {
        if (!process_argument(argc, argv, &i, config))
            return false;
    }
    return validate_config(config);
}
