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
    #include <uuid/uuid.h>

    #define GRAPHICS_CODE "GRAPHIC"


typedef enum message_type_t {
    UNK_TYPE,
    INVOCATION,
    RESPONSE,
    FAILED,
} message_type_t;

typedef enum status_t {
    UNK_STATUS,
    OK,
    KO
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
    SERVER_IA_FORWARD,
    SERVER_IA_RIGHT,
    SERVER_IA_LEFT,
    SERVER_IA_LOOK,
    SERVER_IA_INVENTORY,
    SERVER_IA_BROADCAST,
    SERVER_IA_CONNECT_NBR,
    SERVER_IA_FORK,
    SERVER_IA_EJECT,
    SERVER_IA_TAKE,
    SERVER_IA_SET,
    SERVER_IA_INCANTATION,
    ERROR_INVALID_PARAM,
    SET_GRAPHICS_CLIENT,
    SET_IA_CLIENT,
} invocation_t;

typedef enum argument_t {
    NO,
    MAY,
    YES,
} argument_t;

typedef enum client_type_t {
    CLIENT_UNDEFINED,
    CLIENT_GUI,
    CLIENT_IA,
} client_type_t;

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

#endif
