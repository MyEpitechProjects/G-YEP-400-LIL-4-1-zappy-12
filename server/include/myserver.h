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
    #include "map.h"
    #include "clock.h"
    #include "zappy_internal.h"

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
    char uuid[UUID_STR_LEN];
    myclock_t clock;
    char *buffer;
} pollfd_meta_t;

typedef struct instances_t {
    channel_t *_channel;
    user_t *_user;
    thread_t *_thread;
    team_t *_team;
    subscription_t *_subscription;
    reply_t *_reply;
    dm_t *_dm;
} instances_t;

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
} connection_t;

typedef bool (*cmd_handler_t)(connection_t *_server, uint32_t i,
    req_body_t *body);

typedef struct cmd_t {
    char *name;
    invocation_t func;
    cmd_handler_t handler;
    unit_type_t type;
} cmd_t;

bool check_args(arguments_t *args);

bool free_array(void **arr);

bool execute_buffer(connection_t *_server, uint32_t *i, bool ret);

bool handle_request(connection_t *_server, uint32_t i,
    char **command);

char **str_to_word_array(const char *str);

bool complete_buffer(connection_t *_server, uint32_t *i);

bool pop_poller(connection_t *_server, uint32_t *i);

bool push_poller(connection_t *_server, int fd,
    socket_t *sock, fd_type_t _type);
bool push_socket(connection_t *_server, uint32_t port,
    uint32_t listenMax, fd_type_t type);

int parse_arg(int argc, char **argv, arguments_t *args);

bool requese_handler(connection_t *_server, uint32_t *i, bool ret);

status_t check_request(connection_t *_server, pollfd_meta_t *client,
    req_body_t *body, message_t *req);

void get_sock_port(socket_t *_socket);

bool send_response(int fd, message_t *resp, void *ptr);
void gen_uuid(char *dest);
void set_message(message_t *resp, message_type_t type, status_t status,
    invocation_t func);

status_t body_check(const arg_body_t *args, req_body_t *body);

bool start_server(arguments_t *arguments, int sdf);
bool err_response(int fd, message_t *resp, req_body_t *body);

int free_server(connection_t *_server, bool code);
void free_instance(instances_t *_data);
void write_instance(instances_t *_data);


subscription_t *read_subscription_file(char *f);
uint32_t get_subscription_len(subscription_t *subscription);
subscription_t *add_subscription(subscription_t *old);
subscription_t *remove_subscription(subscription_t *old, uint32_t index);
bool write_subscription_file(char *f, subscription_t *subscription);

dm_t *read_dm_file(char *f);
uint32_t get_dm_len(dm_t *dm);
dm_t *add_dm(dm_t *old);
dm_t *remove_dm(dm_t *old, uint32_t index);
bool write_dm_file(char *f, dm_t *dm);

reply_t *read_reply_file(char *f);
uint32_t get_reply_len(reply_t *reply);
reply_t *add_reply(reply_t *old);
reply_t *remove_reply(reply_t *old, uint32_t index);
bool write_reply_file(char *f, reply_t *reply);

thread_t *read_thread_file(char *f);
uint32_t get_thread_len(thread_t *thread);
thread_t *add_thread(thread_t *old);
thread_t *remove_thread(thread_t *old, uint32_t index);
bool write_thread_file(char *f, thread_t *thread);

channel_t *read_channel_file(char *f);
uint32_t get_channel_len(channel_t *channel);
channel_t *add_channel(channel_t *old);
channel_t *remove_channel(channel_t *old, uint32_t index);
bool write_channel_file(char *f, channel_t *channel);

team_t *read_team_file(char *f);
uint32_t get_team_len(team_t *team);
team_t *add_team(team_t *old);
team_t *remove_team(team_t *old, uint32_t index);
bool write_team_file(char *f, team_t *team);

user_t *read_user_file(char *f);
uint32_t get_user_len(user_t *user);
user_t *add_user(user_t *old);
user_t *remove_user(user_t *old, uint32_t index);
bool write_user_file(char *f, user_t *user);


static const cmd_t commands[] = {
    {"Look", SERVER_IA_LOOK, NULL, UNIT_LOOK},
    {NULL, UNK_INVOCATION, NULL, UNIT_NONE},
};
/*
static const cmd_t commands[] = {
    {LOGIN, cmd_login, {YES, NO, NO, NO, NO, NO, NO}},
    {LOGGOUT, cmd_logout, {NO, NO, NO, NO, NO, NO, NO}},
    {LIST_USER, cmd_users, {NO, NO, NO, NO, NO, NO, NO}},
    {INFO_USER, cmd_user, {NO, YES, NO, NO, NO, NO, NO}},
    {SEND_MESSAGE, cmd_send, {NO, YES, NO, YES, NO, NO, NO}},
    {LIST_MESSAGE, cmd_messages, {NO, YES, NO, NO, NO, NO, NO}},
    {SUBSCRIBE, cmd_subscribe, {NO, NO, YES, NO, NO, NO, NO}},
    {SUBSCRIBED, cmd_subscribed, {NO, NO, MAY, NO, NO, NO, NO}},
    {UNSUBSCRIBE, cmd_unsubscribe, {NO, NO, YES, NO, NO, NO, NO}},
    {USE, cmd_use, {NO, NO, MAY, NO, MAY, MAY, NO}},
    {CREATE, cmd_create, {MAY, NO, NO, MAY, NO, NO, MAY}},
    {LIST, cmd_list, {NO, NO, NO, NO, NO, NO, NO}},
    {INFO, cmd_info, {NO, NO, NO, NO, NO, NO, NO}},
    {UNK_INVOCATION, NULL, {MAY, MAY, MAY, MAY, MAY, MAY, MAY}}
};
*/

#endif
