/*
** EPITECH PROJECT, 2026
** ftp server
** File description:
** none
*/

#ifndef __ZAPPY_INTERNAL__
    #define __ZAPPY_INTERNAL__
    #include "myserver.h"
    #include <stdint.h>
    #include <stdbool.h>
    #include <time.h>

typedef enum unit_type_t {
    UNIT_NONE,
    UNIT_FORWARD,
    UNIT_TURN,
    UNIT_LOOK,
    UNIT_BROADCAST,
    UNIT_EJECT,
    UNIT_TAKE,
    UNIT_SET,
    UNIT_INVENTORY,
    UNIT_CONNECT_NBR,
    UNIT_FORK,
    UNIT_INCANTATION,
    UNIT_SURVIE,
    UNIT_RESPAWN_RESOURCES,
} unit_type_t;

typedef struct unit_t {
    unit_type_t type;
    uint16_t value;
} unit_t;

static const unit_t unit_values[] = {
    {UNIT_FORWARD, 7},
    {UNIT_TURN, 7},
    {UNIT_LOOK, 7},
    {UNIT_BROADCAST, 7},
    {UNIT_EJECT, 7},
    {UNIT_TAKE, 7},
    {UNIT_SET, 7},
    {UNIT_INVENTORY, 1},
    {UNIT_FORK, 42},
    {UNIT_INCANTATION, 300},
    {UNIT_SURVIE, 126},
    {UNIT_RESPAWN_RESOURCES, 20},
    {UNIT_NONE, 0},
};

typedef struct connection connection_t;

time_t get_time_unit(connection_t *_server, unit_type_t type);

time_t get_command_time(connection_t *_server, char *command_name);

#endif
