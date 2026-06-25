/*
** EPITECH PROJECT, 2026
** ftp server
** File description:
** none
*/

#ifndef __MYSERVER__
    #define __MYSERVER__
    #define _GNU_SOURCE
    #define MAX_CONN 1024
    #include "protocol.h"
    #include "zappy/map.h"
    #include "clock.h"
    #include "zappy/zappy_internal.h"

typedef enum {
    UNDEFINED,
    FD_SERVER,
    FD_CONTROL,
    FD_DATA,
    FD_QUIT,
} fd_type_t;

typedef struct pollfd_meta {
    socket_t *_socket;
    fd_type_t _type;
    int _data_fd;
    int _control_idx;
    client_type_t client_type;
    uint64_t id;
    myclock_t clock;
    uint64_t targetElapsed;
    char *buffer;
} pollfd_meta_t;


typedef struct arguments {
    uint16_t port;
    uint64_t width;
    uint64_t height;
    uint64_t team_number;
    char **teams_names;
    uint64_t client_number;
    uint64_t freq;
} arguments_t;

typedef struct connection {
    socket_t *_sockets;
    pollfd_meta_t *_fdsm;
    struct pollfd *_fds;
    uint32_t nfds;
    uint32_t nsock;
    arguments_t *arguments;
    map_t map;
    zappy_t zappy;
} connection_t;

bool protocol_unk(connection_t *_server, uint32_t i, char **command);


typedef bool (*cmd_handler_t)(connection_t *_server, uint32_t i,
    char **command);

typedef struct cmd_t {
    char *name;
    invocation_t func;
    cmd_handler_t handler;
    unit_type_t type;
    uint8_t nb_args;
} cmd_t;

char *send_string(int fd, char *resp);

bool check_args(arguments_t *args, int argc, char **argv);

bool free_array(void **arr);

bool execute_buffer(connection_t *_server, uint32_t *i, bool ret);

char *get_buffer(char *input);

bool handle_request(connection_t *_server, uint32_t i,
    char **command);

char **str_to_word_array(const char *str);

void *myfree(void *ptr);
void my_close(int fd);

bool complete_buffer(connection_t *_server, uint32_t *i);

bool pop_poller(connection_t *_server, uint32_t *i);

bool push_poller(connection_t *_server, int fd,
    socket_t *sock, fd_type_t _type);
bool push_socket(connection_t *_server, uint32_t port,
    uint32_t listenMax, fd_type_t type);

int parse_arg(int argc, char **argv, arguments_t *args);

bool requese_handler(connection_t *_server, uint32_t *i, bool ret);

bool start_server(arguments_t *arguments, int sdf);

bool pop_buffer(connection_t *_server, uint32_t i, char *input);


int free_server(connection_t *_server, bool code);
unsigned int list_len(char **argv);
uint64_t get_buffer_length(char *input);

unit_type_t get_command_unit_type(invocation_t invoke, const cmd_t *functions);
invocation_t get_command_invoke(char *command, const cmd_t *functions);
cmd_handler_t get_command_function(invocation_t invoke,
    const cmd_t *functions);
uint8_t get_command_nb_args(invocation_t invoke, const cmd_t *functions);
int64_t get_timedout(connection_t *_server);
bool client_disconnect(connection_t *_server, uint32_t *i);

static const cmd_t ia_commands[] = {
    {"Forward", SERVER_IA_FORWARD, protocol_ia_move, UNIT_FORWARD, 0},
    {"Left", SERVER_IA_LEFT, protocol_ia_rotate_left, UNIT_TURN, 0},
    {"Right", SERVER_IA_RIGHT, protocol_ia_rotate_right, UNIT_TURN, 0},
    {"Look", SERVER_IA_LOOK, protocol_ia_look, UNIT_LOOK, 0},
    {"Broadcast", SERVER_IA_BROADCAST, protocol_ia_broadcast,
        UNIT_BROADCAST, 1},
    {"Connect_nbr", SERVER_IA_CONNECT_NBR, protocol_ia_connect_nbr,
        UNIT_CONNECT_NBR, 0},
    {"Inventory", SERVER_IA_INVENTORY, protocol_ia_inventory,
        UNIT_INVENTORY, 0},
    {"Fork", SERVER_IA_FORK, protocol_ia_fork, UNIT_FORK, 0},
    {"Set", SERVER_IA_SET, protocol_ia_set, UNIT_SET, 1},
    {"Take", SERVER_IA_TAKE, protocol_ia_take, UNIT_TAKE, 1},
    {"Incantation", SERVER_IA_INCANTATION,
        protocol_ia_incantation, UNIT_NONE, 0},
    {"Eject", SERVER_IA_EJECT, protocol_ia_eject, UNIT_EJECT, 1},
    {NULL, UNK_INVOCATION, protocol_ko, UNIT_NONE, 0},
};

static const cmd_t gui_commands[] = {
    {"sgt", CLIENT_TIME_REQ, protocol_time_unit, UNIT_NONE, 0},
    {"msz", CLIENT_MAP_SIZE, protocol_map_size, UNIT_NONE, 0},
    {"bct", CLIENT_TILE_CONTENT, protocol_tile, UNIT_NONE, 2},
    {"mct", CLIENT_MAP_CONTENT, protocol_map_tiles, UNIT_NONE, 0},
    {"tna", CLIENT_TEAMS_NAMES, protocol_teams, UNIT_NONE, 0},
    {"ppo", CLIENT_PLAYER_POS, protocol_player_pos, UNIT_NONE, 1},
    {"plv", CLIENT_PLAYER_LEVEL, protocol_player_level,
        UNIT_NONE, 1},
    {"pin", CLIENT_PLAYER_INV, protocol_player_inventory,
        UNIT_NONE, 1},
    {"sst", CLIENT_TIME_EDIT, protocol_time_unit_edit,
        UNIT_NONE, 1},
    {NULL, UNK_INVOCATION, protocol_unk, UNIT_NONE, 0},
};

#endif
