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
    char uuid[UUID_STR_LEN];
    myclock_t clock;
    char *buffer;
} pollfd_meta_t;


typedef struct arguments {
    uint16_t port;
    uint64_t width;
    uint64_t height;
    uint64_t team_number;
    char **teams_names;
    uint64_t client_number;
    uint8_t freq;
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
} cmd_t;

char *send_string(int fd, char *resp);

bool check_args(arguments_t *args);

bool free_array(void **arr);

bool execute_buffer(connection_t *_server, uint32_t *i, bool ret);

bool handle_request(connection_t *_server, uint32_t i,
    char **command);

char **str_to_word_array(const char *str);

void *myfree(void *ptr);

bool complete_buffer(connection_t *_server, uint32_t *i);

bool pop_poller(connection_t *_server, uint32_t *i);

bool push_poller(connection_t *_server, int fd,
    socket_t *sock, fd_type_t _type);
bool push_socket(connection_t *_server, uint32_t port,
    uint32_t listenMax, fd_type_t type);

int parse_arg(int argc, char **argv, arguments_t *args);

bool requese_handler(connection_t *_server, uint32_t *i, bool ret);

void get_sock_port(socket_t *_socket);

void gen_uuid(char *dest);

bool start_server(arguments_t *arguments, int sdf);

bool pop_buffer(connection_t *_server, uint32_t i, char *input);

int free_server(connection_t *_server, bool code);
unsigned int list_len(char **argv);

unit_type_t get_command_unit_type(invocation_t invoke, const cmd_t *functions);
invocation_t get_command_invoke(char *command, const cmd_t *functions);
cmd_handler_t get_command_function(invocation_t invoke,
    const cmd_t *functions);

static const cmd_t ia_commands[] = {
    {NULL, UNK_INVOCATION, protocol_ko, UNIT_NONE},
};
static const cmd_t gui_commands[] = {
    {"sgt", CLIENT_TIME_REQ, protocol_time_unit, UNIT_NONE},
    {"msz", CLIENT_MAP_SIZE, protocol_map_size, UNIT_NONE},
    {"bct", CLIENT_TILE_CONTENT, protocol_tile, UNIT_NONE},
    {"mct", CLIENT_MAP_CONTENT, protocol_map_tiles, UNIT_NONE},
    {"tna", CLIENT_TEAMS_NAMES, protocol_teams, UNIT_NONE},
    {NULL, UNK_INVOCATION, protocol_unk, UNIT_NONE},
};

#endif
