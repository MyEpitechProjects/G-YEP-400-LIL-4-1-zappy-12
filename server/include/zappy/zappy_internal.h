/*
** EPITECH PROJECT, 2026
** ftp server
** File description:
** none
*/

#ifndef __ZAPPY_INTERNAL__
    #define __ZAPPY_INTERNAL__
    #include <stdint.h>
    #include <stdbool.h>
    #include <time.h>
    #include "tiles.h"
    #include "player.h"
    #include "egg.h"
    #include "clock.h"
    #define BASE_FOOD 10

    #define FOOD_SPAWN_UNIT 20
    #define EAT_UNIT 126
    #define DENSITY_FOOD 50
    #define DENSITY_LINEMATE 30
    #define DENSITY_DERAUMERE 15
    #define DENSITY_SIBUR 10
    #define DENSITY_MENDIANE 10
    #define DENSITY_PHIRAS 8
    #define DENSITY_THYSTAME 5

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

typedef enum resource_type_t {
    RES_LINEMATE = 1,
    RES_DERAUMERE,
    RES_SIBUR,
    RES_MENDIANE,
    RES_PHIRAS,
    RES_THYSTAME,
} resource_type_t;

typedef struct unit_t {
    unit_type_t type;
    uint16_t value;
} unit_t;

typedef struct zappy_t {
    uint64_t w;
    uint64_t h;
    tile_t *tiles;
    uint64_t nb_players;
    player_t *players;
    uint64_t nb_eggs;
    egg_t *eggs;
    myclock_t foodClock;
} zappy_t;

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

uint64_t get_time_unit(connection_t *_server, unit_type_t type);

uint64_t get_command_time(connection_t *_server, unit_type_t type);

uint64_t calculate_time_freq(connection_t *_server, uint64_t value);

bool add_food_tile(connection_t *_server, zappy_t *zappy);

bool spawn_food(connection_t *_server, zappy_t *zappy);


char *string_create(char *base);
char *string_append(char *string, char *append, bool spaced);
char *string_append_int(char *string, int64_t value, bool spaced);
char *string_append_uint(char *string, uint64_t value, bool spaced);
char *string_end(char *string);

bool zappy(connection_t *_server, zappy_t *zappy);
uint64_t tile_pos_to_index(connection_t *_server, uint64_t x, uint64_t y);
uint64_t get_egg_index(connection_t *_server, egg_t *egg, bool *ok);


uint64_t extract_num(char *ptr, char **endptr, int base, bool ok);
player_t *get_player(connection_t *_server, uint64_t index);
void *list_push(void *original, uint64_t *size, uint64_t itemSize,
    bool *status);

bool protocol_map_size(connection_t *_server, uint32_t i, char **command);
bool protocol_graphic_start(connection_t *_server, uint32_t i, char **command);
bool protocol_tile(connection_t *_server, uint32_t i, char **command);
bool protocol_time_unit(connection_t *_server, uint32_t i, char **command);
bool protocol_graphic_bad_param(connection_t *_server, uint32_t i,
    char *original_resp);
bool protocol_map_tiles(connection_t *_server, uint32_t i, char **command);
bool protocol_unk(connection_t *_server, uint32_t i, char **command);
bool protocol_teams(connection_t *_server, uint32_t i, char **command);
bool protocol_player_inventory(connection_t *_server, uint32_t i,
    char **command);
bool protocol_player_level(connection_t *_server, uint32_t i, char **command);
bool protocol_player_pos(connection_t *_server, uint32_t i, char **command);
bool protocol_time_unit_edit(connection_t *_server, uint32_t i,
    char **command);


bool intern_protocol_player_connected(connection_t *_server, uint32_t i,
    player_t *player);
bool intern_protocol_player_inventory(connection_t *_server, uint32_t i,
    player_t *player);
bool intern_protocol_player_level(connection_t *_server, uint32_t i,
    player_t *player);
bool intern_protocol_player_pos(connection_t *_server, uint32_t i,
    player_t *player);
bool intern_protocol_egg_take(connection_t *_server, uint32_t i,
    egg_t *egg);
bool intern_protocol_player_death(connection_t *_server, uint32_t i,
    player_t *player);


bool protocol_ia_start(connection_t *_server, uint32_t i,
    char **command, char *team);
bool protocol_ia_bad_param(connection_t *_server, uint32_t i,
    char *original_resp);

bool trigger_graphic_player_login(connection_t *_server, player_t *player,
    egg_t *egg);
bool trigger_graphic_player_eat(connection_t *_server, player_t *player);
bool trigger_graphic_player_death(connection_t *_server, player_t *player);


bool protocol_ko(connection_t *_server, uint32_t i, char **command);

bool free_zappy(connection_t *_server, zappy_t *zappy);

#endif
