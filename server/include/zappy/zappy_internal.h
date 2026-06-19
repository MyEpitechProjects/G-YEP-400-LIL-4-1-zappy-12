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
    #include "incantation.h"
    #define FOOD_PERCENTAGE 10
    #define BASE_FOOD 10
    #define WIN_LEVEL 8
    #define WIN_PLAYER_COUNT 6
    #define FOOD_SPAWN_UNIT 20
    #define EAT_UNIT 126
    #define INCANTATION_UNIT 300
    #define HATCH_UNIT 600
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
    UNIT_HATCH,
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
    uint64_t nb_incantation;
    incantation_t *incantation;
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
    {UNIT_SURVIE, 126},
    {UNIT_RESPAWN_RESOURCES, 20},
    {UNIT_HATCH, 600},
    {UNIT_NONE, 0},
};

typedef struct connection connection_t;

uint64_t get_time_unit(connection_t *_server, unit_type_t type);

uint64_t get_command_time(connection_t *_server, unit_type_t type);

uint64_t calculate_time_freq(connection_t *_server, uint64_t value);

bool add_food_tile(connection_t *_server, zappy_t *zappy);

bool spawn_food(connection_t *_server, zappy_t *zappy);


char *string_create(const char *base);
char *string_append(char *string, const char *append, bool spaced);
char *string_append_int(char *string, const int64_t value, bool spaced);
char *string_append_uint(char *string, const uint64_t value, bool spaced);
char *string_end(char *string);

bool init_zappy(connection_t *_server, zappy_t *zappy);
bool need_food_respawn(connection_t *_server, zappy_t *zappy);
bool check_eggs_hatch(connection_t *_server, zappy_t *zappy);
pos_t *set_pos_xy(pos_t *pos, uint64_t x, uint64_t y);
uint64_t tile_pos_to_index(connection_t *_server, uint64_t x, uint64_t y);
egg_t *get_egg_by_team(connection_t *_server, char *team);
player_t *get_player_by_id(connection_t *_server, uint64_t id);
player_t *get_last_player(connection_t *_server);
egg_t *get_last_egg(connection_t *_server);
incantation_t *get_last_incantation(connection_t *_server);
uint64_t get_available_eggs(connection_t *_server, char *team);
uint64_t **set_resources_array(uint64_t **src, inventory_t *inv);


uint64_t extract_num(char *ptr, char **endptr, int base, bool ok);
void *list_push(void *original, uint64_t *size, uint64_t itemSize,
    bool *status);
bool switch_item(connection_t *_server, uint32_t i, uint64_t *inv1,
    uint64_t *inv2);
bool check_incantation(connection_t *_server, const incantation_type_t *target,
    pos_t *incantationPos, bool startCheck);
bool check_incantation_finish(connection_t *_server, zappy_t *zappy);
bool player_move(connection_t *_server, player_t *player,
    orientation_t orientation);

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
bool intern_protocol_graphics_broadcast(connection_t *_server, uint32_t i,
    char *msg, player_t *sender);
bool intern_protocol_graphics_egg_layed(connection_t *_server, uint32_t i,
    egg_t *egg);
bool intern_protocol_graphics_egg_laying(connection_t *_server, uint32_t i,
    player_t *player);
bool intern_protocol_graphic_drop(connection_t *_server, uint32_t i,
    player_t *player, uint8_t resource_index);
bool intern_protocol_graphic_collect(connection_t *_server, uint32_t i,
    player_t *player, uint8_t resource_index);
bool intern_protocol_map_tile(connection_t *_server, uint32_t i, tile_t *tile);
bool intern_protocol_graphic_start_incantation(connection_t *_server,
    uint64_t i, uint64_t level, pos_t *pos);
bool intern_protocol_graphic_end_incantation(connection_t *_server,
    uint64_t i, bool status, incantation_t *inc);
bool intern_protocol_killed_egg(connection_t *_server, uint32_t i,
    egg_t *egg);
bool intern_protocol_ejected(connection_t *_server, uint32_t i,
    player_t *player);
bool intern_protocol_game_end(connection_t *_server, uint32_t i,
    const char *team);



bool protocol_ia_start(connection_t *_server, uint32_t i,
    char **command, char *team);
bool protocol_ia_bad_param(connection_t *_server, uint32_t i,
    char *original_resp);
bool protocol_ia_death(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_connect_nbr(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_inventory(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_rotate_left(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_rotate_right(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_move(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_look(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_broadcast(connection_t *_server, uint32_t i,
    char **command);
bool intern_protocol_ia_broadcast(connection_t *_server, uint32_t i,
    char *msg, player_t *sender);
bool protocol_ia_fork(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_take(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_set(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_incantation_start(connection_t *_server,
    uint32_t i);
bool protocol_ia_incantation(connection_t *_server, uint32_t i,
    char **command);
bool protocol_ia_incantation_end(connection_t *_server,
    uint32_t i, player_t *me);
bool protocol_ia_eject(connection_t *_server, uint32_t i,
    char **command);

bool trigger_graphic_player_login(connection_t *_server, player_t *player,
    egg_t *egg);
bool trigger_graphic_player_inv(connection_t *_server, player_t *player);
bool trigger_graphic_player_death(connection_t *_server, player_t *player);
bool trigger_broadcast(connection_t *_server, uint64_t me,
    player_t *sender, char *msg);
bool trigger_graphic_player_move(connection_t *_server, player_t *player);
bool trigger_graphic_egg_laying(connection_t *_server, player_t *player);
bool trigger_graphic_egg_layed(connection_t *_server, egg_t *egg);
bool trigger_graphic_drop(connection_t *_server, player_t *player,
    uint8_t resources_index);
bool trigger_graphic_collect(connection_t *_server, player_t *player,
    uint8_t resources_index);
bool trigger_graphic_tile_change(connection_t *_server,
    tile_t *tile);
bool trigger_graphic_start_incantation(connection_t *_server,
    uint64_t level, pos_t *pos);
bool trigger_ia_start_incantation(connection_t *_server,
    uint64_t level, pos_t *pos, uint64_t inc_index);
bool trigger_ia_end_incantation_ko(connection_t *_server,
    uint64_t inc_index);
bool trigger_ia_end_incantation_ok(connection_t *_server,
    uint64_t inc_index);
bool trigger_gui_end_incantation(connection_t *_server,
    zappy_t *zappy, uint64_t incidx, bool status);
bool trigger_graphic_killed_eggs(connection_t *_server,
    egg_t *egg);
bool trigger_graphic_ejected(connection_t *_server,
    player_t *player);
bool trigger_graphic_game_end(connection_t *_server,
    const char *team);
bool trigger_graphic_player_level(connection_t *_server,
    player_t *player);

bool protocol_ko(connection_t *_server, uint32_t i, char **command);
bool protocol_ok(connection_t *_server, uint32_t i, char **command);

bool free_zappy(connection_t *_server, zappy_t *zappy);

#endif
