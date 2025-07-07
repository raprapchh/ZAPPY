/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** server.h
*/

#ifndef SERVER_H_
    #define SERVER_H_

    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <errno.h>
    #include <signal.h>
    #include <time.h>
    #include <math.h>
    #include <poll.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <sys/time.h>
    #include <fcntl.h>
    #include <stdbool.h>
    #include <stdint.h>
    #include <limits.h>

    #define BUFFER_SIZE 8192
    #define CMD_MAX 512
    #define MAX_CLIENTS 64
    #define MAX_TEAMS 16
    #define MAX_TEAM_NAME 64
    #define MAX_COMMAND_QUEUE 10
    #define RESOURCE_TYPES 7

typedef enum {
    FOOD = 0,
    LINEMATE,
    DERAUMERE,
    SIBUR,
    MENDIANE,
    PHIRAS,
    THYSTAME
} resource_type_t;

typedef enum {
    NORTH = 1,
    EAST = 2,
    SOUTH = 3,
    WEST = 4
} orientation_t;

typedef enum {
    CLIENT_UNKNOWN = 0,
    CLIENT_AI,
    CLIENT_GUI
} client_type_t;

typedef enum {
    STATE_CONNECTING = 0,
    STATE_WAITING_TEAM,
    STATE_CONNECTED,
    STATE_DISCONNECTED
} client_state_t;

typedef enum {
    GAME_STARTING = 0,
    GAME_RUNNING,
    GAME_ENDED
} game_state_t;

typedef struct circular_buffer_s {
    char *data;
    size_t size;
    size_t read_pos;
    size_t write_pos;
    bool full;
} circular_buffer_t;

typedef struct command_s {
    char data[CMD_MAX];
    size_t len;
    uint64_t received_at;
    struct command_s *next;
} command_t;

typedef struct command_queue_s {
    command_t *head;
    command_t *tail;
    size_t count;
    size_t max_size;
} command_queue_t;

typedef struct resource_s {
    resource_type_t type;
    int x;
    int y;
    struct resource_s *next;
} resource_t;

typedef struct egg_s {
    int id;
    int x;
    int y;
    int team_id;
    uint64_t laid_at;
    struct egg_s *next;
} egg_t;

typedef struct player_s {
    int id;
    int x;
    int y;
    int level;
    orientation_t orientation;
    int inventory[RESOURCE_TYPES];
    int team_id;
    uint64_t last_meal;
    bool is_frozen;
    uint64_t frozen_until;
    bool is_dead;
    bool is_busy;
    uint64_t busy_until;
    struct player_s *next;
} player_t;

typedef struct tile_s {
    int x;
    int y;
    resource_t *resources;
    player_t *players;
    egg_t *eggs;
    int resource_count[RESOURCE_TYPES];
    bool dirty;
} tile_t;

typedef struct map_s {
    int width;
    int height;
    tile_t **tiles;
    uint64_t last_spawn;
} map_t;

typedef struct team_s {
    char name[MAX_TEAM_NAME];
    int id;
    int max_clients;
    int connected_clients;
    egg_t *eggs;
    player_t *players;
    struct team_s *next;
} team_t;

typedef struct event_s {
    uint64_t execute_at;
    int client_fd;
    char command[CMD_MAX];
    struct event_s *next;
} event_t;

typedef struct event_queue_s {
    event_t *head;
    size_t count;
} event_queue_t;

typedef struct client_s {
    int fd;
    struct sockaddr_in addr;
    circular_buffer_t *in_buf;
    circular_buffer_t *out_buf;
    client_type_t type;
    client_state_t state;
    char partial_cmd[CMD_MAX];
    size_t partial_len;
    player_t *player;
    team_t *team;
    command_queue_t *cmd_queue;
    time_t connect_time;
    size_t bytes_sent;
    size_t bytes_received;
    struct client_s *next;
} client_t;

typedef struct config_s {
    int port;
    int width;
    int height;
    char team_names[MAX_TEAMS][MAX_TEAM_NAME];
    int team_count;
    int clients_nb;
    int freq;
} config_t;

typedef struct server_s {
    config_t config;
    int listen_fd;
    struct pollfd poll_fds[MAX_CLIENTS + 1];
    size_t poll_count;
    client_t *clients;
    map_t *map;
    team_t *teams;
    event_queue_t *events;
    bool running;
    game_state_t state;
    uint64_t start_time;
    int next_player_id;
    int next_egg_id;
} server_t;

circular_buffer_t *buffer_create(size_t size);
void buffer_destroy(circular_buffer_t *buf);
bool buffer_write(circular_buffer_t *buf, const char *data, size_t len);
size_t buffer_read(circular_buffer_t *buf, char *data, size_t len);
size_t buffer_available(const circular_buffer_t *buf);
size_t buffer_space(const circular_buffer_t *buf);

command_queue_t *command_queue_create(size_t max_size);
void command_queue_destroy(command_queue_t *queue);
bool command_queue_push(command_queue_t *queue, const char *cmd);
command_t *command_queue_pop(command_queue_t *queue);
bool command_queue_is_full(const command_queue_t *queue);
player_t *create_and_setup_player(server_t *server, team_t *team);

int socket_create_server(int port);
int socket_accept_client(int server_fd);
void socket_set_nonblocking(int fd);
void socket_close(int fd);

client_t *client_create(int fd, struct sockaddr_in *addr);
void client_destroy(client_t *client);
bool client_read(client_t *client);
bool client_write(client_t *client);
void client_send(client_t *client, const char *message);
void client_disconnect(client_t *client);
char *client_get_command(client_t *client);

map_t *map_create(int width, int height);
void map_destroy(map_t *map);
tile_t *map_get_tile(map_t *map, int x, int y);
void map_normalize_coords(map_t *map, int *x, int *y);
void map_spawn_resources(map_t *map, int freq, server_t *server);
void map_maintain_resources(map_t *map, int freq, server_t *server);
int map_count_resource_type(map_t *map, resource_type_t type);
bool map_add_resource(map_t *map, int x, int y, resource_type_t type);
bool map_remove_resource(map_t *map, int x, int y, resource_type_t type);
void map_add_player(map_t *map, player_t *player);
void map_remove_player(map_t *map, player_t *player);

extern const int elevation_requirements[8][RESOURCE_TYPES];
extern const int elevation_players_required[8];

player_t *player_create(int id, int x, int y, int team_id);
void player_destroy(player_t *player);
bool player_move_forward(player_t *player, map_t *map);
void player_turn_left(player_t *player);
void player_turn_right(player_t *player);
bool player_take_resource(player_t *player, map_t *map, resource_type_t type);
bool player_drop_resource(player_t *player, map_t *map, resource_type_t type);
void player_get_vision(player_t *player, map_t *map, char *vision);
void append_resource_name(char *temp, resource_type_t resource,
    size_t max_len);
void append_resource_name_part2(char *temp, resource_type_t resource,
    size_t max_len);
void get_vision_coords(player_t *player, int dist, int i, int *coords);
void get_vision_coords_north_south(player_t *player, int dist, int *coords);
void get_vision_coords_east_west(player_t *player, int dist, int *coords);
void player_get_inventory(player_t *player, char *inventory);
bool player_can_level_up(player_t *player, map_t *map);
void player_level_up(player_t *player, map_t *map);
void player_freeze(player_t *player, uint64_t duration);
bool player_is_frozen(player_t *player);

team_t *team_create(const char *name, int id, int max_clients, map_t *map);
void team_destroy(team_t *team);
team_t *team_find_by_name(team_t *teams, const char *name);
bool team_add_client(team_t *team);
void team_remove_client(team_t *team);
int team_get_slots(team_t *team);

egg_t *egg_create(int id, int x, int y, int team_id);
void egg_destroy(egg_t *egg);
bool egg_hatch(egg_t *egg, player_t **player);

event_queue_t *event_queue_create(void);
void event_queue_destroy(event_queue_t *queue);
bool event_queue_add(event_queue_t *queue, uint64_t when, int client_fd,
    const char *command);
event_t *event_queue_get_ready(event_queue_t *queue, uint64_t now);
void event_queue_remove(event_queue_t *queue, event_t *event);

uint64_t get_timestamp(void);
int calculate_timeout(event_queue_t *events, uint64_t now);

bool parse_args(int argc, char **argv, config_t *config);
void print_usage(const char *prog_name);

bool parse_port(const char *str, int *port);
bool parse_dimension(const char *str, int *dim, const char *name);
bool parse_clients_nb(const char *str, int *clients_nb);
bool parse_freq(const char *str, int *freq);
bool parse_team_names(int argc, char **argv, int start_idx, config_t *config);
bool validate_config(config_t *config);

bool server_init(server_t *server, config_t *config);
void server_cleanup(server_t *server);
void server_run(server_t *server);
void server_add_client(server_t *server, client_t *client);
void server_remove_client(server_t *server, client_t *client);
void server_handle_client_data(server_t *server, client_t *client);
void server_process_events(server_t *server);

void handle_ai_command(server_t *server, client_t *client,
    const char *command);
void handle_gui_command(server_t *server, client_t *client,
    const char *command);

void cmd_forward(server_t *server, client_t *client);
void cmd_right(server_t *server, client_t *client);
void cmd_left(server_t *server, client_t *client);
void cmd_look(server_t *server, client_t *client);
void cmd_inventory(server_t *server, client_t *client);
void cmd_broadcast(server_t *server, client_t *client, const char *text);
void cmd_connect_nbr(server_t *server, client_t *client);
void cmd_fork(server_t *server, client_t *client);
void cmd_eject(server_t *server, client_t *client);
void cmd_take(server_t *server, client_t *client, const char *object);
void cmd_set(server_t *server, client_t *client, const char *object);
void cmd_incantation(server_t *server, client_t *client);

void execute_forward(server_t *server, client_t *client);
void execute_right(server_t *server, client_t *client);
void execute_left(server_t *server, client_t *client);
void execute_look(server_t *server, client_t *client);
void execute_inventory(server_t *server, client_t *client);
void execute_broadcast(server_t *server, client_t *client,
    const char *text);
void execute_fork(server_t *server, client_t *client);
void execute_eject(server_t *server, client_t *client);
void execute_take(server_t *server, client_t *client, const char *object);
void execute_set(server_t *server, client_t *client, const char *object);
void execute_incantation(server_t *server, client_t *client);

resource_type_t parse_resource(const char *name);

player_t *find_player_by_id(server_t *server, int player_id);
void handle_msz(server_t *server, client_t *client);
void handle_bct(server_t *server, client_t *client, const char *args);
void handle_mct(server_t *server, client_t *client);
void handle_tna(server_t *server, client_t *client);
void handle_ppo(server_t *server, client_t *client, const char *args);
void handle_plv(server_t *server, client_t *client, const char *args);
void handle_pin(server_t *server, client_t *client, const char *args);

void gui_send_map_size(client_t *client, map_t *map);
void gui_send_tile_content(client_t *client, map_t *map, int x, int y);
void gui_send_map_content(client_t *client, map_t *map);
void gui_send_team_names(client_t *client, team_t *teams);
void gui_send_player_new(client_t *client, player_t *player, team_t *team);
void gui_send_player_position(client_t *client, player_t *player);
void gui_send_player_level(client_t *client, player_t *player);
void gui_send_player_inventory(client_t *client, player_t *player);
void gui_send_all_players(client_t *client, team_t *teams);

void broadcast_to_guis(server_t *server, const char *message);
void broadcast_player_new_to_guis(server_t *server, player_t *player,
    team_t *team);
void broadcast_player_position_to_guis(server_t *server, player_t *player);
void broadcast_player_level_to_guis(server_t *server, player_t *player);
void broadcast_player_inventory_to_guis(server_t *server, player_t *player);
void broadcast_message(server_t *server, player_t *sender,
    const char *message);
void broadcast_tile_content_to_guis(server_t *server, int x, int y);

bool player_consume_food(player_t *player, uint64_t current_time, int freq);
bool player_consume_food_and_notify(player_t *player, uint64_t current_time,
    int freq, bool *food_consumed);
bool player_check_death(player_t *player);
void player_kill(player_t *player);
void broadcast_player_death_to_guis(server_t *server, player_t *player);
void server_check_players_starvation(server_t *server);
void server_cleanup_dead_players(server_t *server);

void broadcast_egg_laid_to_guis(server_t *server, egg_t *egg,
    team_t *team, player_t *layer);
void broadcast_incantation_start_to_guis(server_t *server,
    player_t *player, int level);
void broadcast_incantation_end_to_guis(server_t *server,
    player_t *player, bool success);
void broadcast_player_expelled_to_guis(server_t *server,
    player_t *player);

void gui_send_egg_laid(client_t *client, egg_t *egg, team_t *team);
void gui_send_incantation_start(client_t *client, player_t *player,
    int level);
void gui_send_incantation_end(client_t *client, player_t *player,
    bool success);
void gui_send_player_expelled(client_t *client, player_t *player);

#endif /* !SERVER_H_ */
