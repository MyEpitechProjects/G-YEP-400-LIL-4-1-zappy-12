/*
** EPITECH PROJECT, 2026
** myteams common protocol
** File description:
** Shared structs and macros between client and server
*/

#ifndef __PROTOCOL_H__
    #define __PROTOCOL_H__
    #define _GNU_SOURCE
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <poll.h>
    #include <argp.h>
    #include <signal.h>
    #include <sys/signalfd.h>
    #include <err.h>
    #include <errno.h>
    #include <dirent.h>
    #include <stdint.h>
    #include <arpa/inet.h>
    //#include "logging_client.h"
    //#include "logging_server.h"
    #include <uuid/uuid.h>
    #define MAX_NAME_LENGTH 32
    #define MAX_DESCRIPTION_LENGTH 255
    #define MAX_BODY_LENGTH 512
    #define USER_FILE "user.bin"
    #define CHANNEL_FILE "channel.bin"
    #define THREAD_FILE "thread.bin"
    #define SUBSCRIPTION_FILE "subscription.bin"
    #define TEAM_FILE "team.bin"
    #define REPLY_FILE "reply.bin"
    #define DM_FILE "dm.bin"

typedef enum message_type_t {
    UNK_TYPE,
    INVOCATION,
    RESPONSE,
    FAILED,
} message_type_t;

typedef enum status_t {
    UNK_STATUS,
    READY = 2,
    LOGGED = 463,
    LOGGEDOUT = 465,
    COMMAND_OK = 403,

    TEAM_ALREADY_EXIST = 847,
    CHANNEL_ALREADY_EXIST = 849,
    THREAD_ALREADY_EXIST = 851,
    USER_NOT_EXIST = 853,
    TEAM_NOT_EXIST = 855,
    CHANNEL_NOT_EXIST = 857,
    THREAD_NOT_EXIST = 859,
    NOT_LOGGED = 63,
    INTERNAL = 860,
    UNK_COMMAND = 3,
    INVALID_BODY = 5,
} status_t;

typedef enum invocation_t {
    UNK_INVOCATION,
    CLIENT_MAP_SIZE,
    CLIENT_TILE_CONTENT,
    CLIENT_MAP_CONTENT,
    CLIENT_TEAMS_NAMES,
    CLIENT_PLAYER_POS,
    CLIENT_PLAYER_LEVEL,
    CLIENT_PLAYER_INV,
    CLIENT_TIME_REQ,
    CLIENT_TIME_EDIT,
    SERVER_MAP_SIZE,
    SERVER_TILE_CONTENT,
    SERVER_MAP_CONTENT,
    SERVER_TEAMS_NAMES,
    SERVER_PLAYER_LOGIN,
    SERVER_PLAYER_POS,
    SERVER_PLAYER_LEVEL,
    SERVER_PLAYER_INV,
    SERVER_PLAYER_KICK,
    SERVER_BROADCAST,
    SERVER_INVOK_START,
    SERVER_INVOK_END,
    SERVER_EGG_LAID_ME,
    SERVER_EGG_LAID_OTHER,
    SERVER_RESSOURCE_DROP,
    SERVER_RESSOURCE_COLLECT,
    SERVER_PLAYER_DEATH,
    SERVER_EGG_HATCH,
    SERVER_EGG_DEATH,
    SERVER_TIME_REQ,
    SERVER_TIME_EDIT,
    SERVER_GAME_END,
    SERVER_MESSAGE,
    ERROR_UNKNOWN_COMMAND,
    ERROR_INVALID_PARAM,
} invocation_t;

typedef enum argument_t {
    NO,
    MAY,
    YES,
} argument_t;



typedef struct port {
    int _p1;
    int _p2;
} port_t;


typedef struct socket {
    int _fd;
    struct sockaddr_in _address;
    socklen_t _addrlen;
    port_t _port;
} socket_t;

typedef struct arg_body_t {
    argument_t name;
    argument_t user_uuid;
    argument_t team_uuid;
    argument_t body;
    argument_t channel_uuid;
    argument_t thread_uuid;
    argument_t description;
} arg_body_t;

typedef enum context_type_t {
    CTX_NULL,
    CTX_TEAM,
    CTX_TEAM_CHANNEL,
    CTX_ALL,
} context_type_t;

typedef struct context_t {
    char team_uuid[UUID_STR_LEN];
    char channel_uuid[UUID_STR_LEN];
    char thread_uuid[UUID_STR_LEN];
} context_t;

typedef struct message_t {
    message_type_t _type;
    invocation_t _func;
    status_t _status;
    uint32_t _body_size;
} message_t;

typedef struct req_body_t {
    char name[MAX_NAME_LENGTH];
    char user_uuid[UUID_STR_LEN];
    char team_uuid[UUID_STR_LEN];
    char body[MAX_BODY_LENGTH];
    char channel_uuid[UUID_STR_LEN];
    char thread_uuid[UUID_STR_LEN];
    char description[MAX_DESCRIPTION_LENGTH];
} req_body_t;

typedef struct user_t {
    bool used;
    char uuid[UUID_STR_LEN];
    char name[MAX_NAME_LENGTH];
    bool online;
} user_t;

typedef struct team_t {
    bool used;
    char uuid[UUID_STR_LEN];
    char creator_uuid[UUID_STR_LEN];
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
} team_t;

typedef struct channel_t {
    bool used;
    char uuid[UUID_STR_LEN];
    char team_uuid[UUID_STR_LEN];
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
} channel_t;

typedef struct thread_t {
    bool used;
    char uuid[UUID_STR_LEN];
    char channel_uuid[UUID_STR_LEN];
    char creator_uuid[UUID_STR_LEN];
    char title[MAX_NAME_LENGTH];
    char body[MAX_BODY_LENGTH];
    uint64_t timestamp;
} thread_t;

typedef struct reply_t {
    bool used;
    char team_uuid[UUID_STR_LEN];
    char thread_uuid[UUID_STR_LEN];
    char creator_uuid[UUID_STR_LEN];
    char body[MAX_BODY_LENGTH];
    uint64_t timestamp;
} reply_t;

typedef struct dm_t {
    bool used;
    char sender_uuid[UUID_STR_LEN];
    char receiver_uuid[UUID_STR_LEN];
    char body[MAX_BODY_LENGTH];
    uint64_t timestamp;
} dm_t;

typedef struct subscription_t {
    bool used;
    char user_uuid[UUID_STR_LEN];
    char team_uuid[UUID_STR_LEN];
} subscription_t;

#endif
