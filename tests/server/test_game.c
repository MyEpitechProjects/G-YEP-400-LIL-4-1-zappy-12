/*
** EPITECH PROJECT, 2026
** Zappy Server - Game Logic Unit Tests
** File description:
** Tests for game mechanics: map, player, inventory, incantation, time units
*/

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <string.h>
#include <stdlib.h>
#include "myserver.h"
#include "zappy/zappy_internal.h"
#include "sys.h"


static connection_t make_server(uint64_t w, uint64_t h, uint8_t freq)
{
    connection_t srv = {0};
    static arguments_t args_pool[32];
    static int idx = 0;
    arguments_t *args = &args_pool[idx++ % 32];

    memset(args, 0, sizeof(*args));
    args->width = w;
    args->height = h;
    args->freq = freq;
    args->port = 4242;
    args->client_number = 1;
    args->team_number = 1;
    srv.arguments = args;
    return srv;
}


Test(map, tile_pos_to_index_basic)
{
    connection_t srv = make_server(10, 10, 100);

    cr_assert_eq(tile_pos_to_index(&srv, 0, 0), 0);
    cr_assert_eq(tile_pos_to_index(&srv, 5, 0), 5);
    cr_assert_eq(tile_pos_to_index(&srv, 0, 3), 30);
    cr_assert_eq(tile_pos_to_index(&srv, 7, 2), 27);
}

Test(map, tile_pos_to_index_edge)
{
    connection_t srv = make_server(10, 10, 100);

    cr_assert_eq(tile_pos_to_index(&srv, 9, 9), 99);
}

Test(map, tile_pos_to_index_rectangular)
{
    connection_t srv = make_server(5, 3, 100);

    cr_assert_eq(tile_pos_to_index(&srv, 4, 2), 14);
    cr_assert_eq(tile_pos_to_index(&srv, 0, 1), 5);
}

Test(position, set_pos_xy_basic)
{
    pos_t pos = {0};

    set_pos_xy(&pos, 42, 84);
    cr_assert_eq(pos.x, 42);
    cr_assert_eq(pos.y, 84);
}

Test(position, set_pos_xy_zero)
{
    pos_t pos = {99, 99};

    set_pos_xy(&pos, 0, 0);
    cr_assert_eq(pos.x, 0);
    cr_assert_eq(pos.y, 0);
}

Test(timeunit, calculate_time_freq_basic)
{
    connection_t srv = make_server(10, 10, 100);

    cr_assert_eq(calculate_time_freq(&srv, 7), 70);
    cr_assert_eq(calculate_time_freq(&srv, 42), 420);
    cr_assert_eq(calculate_time_freq(&srv, 126), 1260);
    cr_assert_eq(calculate_time_freq(&srv, 300), 3000);
}

Test(timeunit, calculate_time_freq_different_freqs)
{
    connection_t srv10 = make_server(10, 10, 10);
    connection_t srv50 = make_server(10, 10, 50);

    cr_assert_eq(calculate_time_freq(&srv10, 7), 700);
    cr_assert_eq(calculate_time_freq(&srv50, 7), 140);
}

Test(timeunit, get_time_unit_forward)
{
    connection_t srv = make_server(10, 10, 100);
    uint64_t t = get_time_unit(&srv, UNIT_FORWARD);

    cr_assert_eq(t, 70);
}

Test(timeunit, get_time_unit_fork)
{
    connection_t srv = make_server(10, 10, 100);
    uint64_t t = get_time_unit(&srv, UNIT_FORK);

    cr_assert_eq(t, 420);
}

Test(timeunit, get_time_unit_inventory)
{
    connection_t srv = make_server(10, 10, 100);
    uint64_t t = get_time_unit(&srv, UNIT_INVENTORY);

    cr_assert_eq(t, 10);
}

Test(timeunit, get_time_unit_none_returns_zero)
{
    connection_t srv = make_server(10, 10, 100);
    uint64_t t = get_time_unit(&srv, UNIT_NONE);

    cr_assert_eq(t, 0);
}

Test(inventory, initial_values_zero)
{
    inventory_t inv = {0};

    cr_assert_eq(inv.food, 0);
    cr_assert_eq(inv.linemate, 0);
    cr_assert_eq(inv.deraumere, 0);
    cr_assert_eq(inv.sibur, 0);
    cr_assert_eq(inv.mendiane, 0);
    cr_assert_eq(inv.phiras, 0);
    cr_assert_eq(inv.thystame, 0);
}

Test(inventory, food_names_order)
{
    cr_assert_str_eq(food_names[0], "food");
    cr_assert_str_eq(food_names[1], "linemate");
    cr_assert_str_eq(food_names[2], "deraumere");
    cr_assert_str_eq(food_names[3], "sibur");
    cr_assert_str_eq(food_names[4], "mendiane");
    cr_assert_str_eq(food_names[5], "phiras");
    cr_assert_str_eq(food_names[6], "thystame");
    cr_assert_null(food_names[7]);
}

Test(player, default_values)
{
    player_t player = {0};

    cr_assert_eq(player.pos.x, 0);
    cr_assert_eq(player.pos.y, 0);
    cr_assert_eq(player.level, 0);
    cr_assert_eq(player.dead, false);
    cr_assert_eq(player.incanting, false);
    cr_assert_eq(player.inventory.food, 0);
}

Test(player, initialization_values)
{
    player_t player = {0};

    player.level = 1;
    player.inventory.food = BASE_FOOD;
    player.o = ORIENTATION_NORTH;
    cr_assert_eq(player.level, 1);
    cr_assert_eq(player.inventory.food, 10);
    cr_assert_eq(player.o, ORIENTATION_NORTH);
}

Test(orientation, values)
{
    cr_assert_eq(ORIENTATION_NORTH, 0);
    cr_assert_eq(ORIENTATION_EST, 1);
    cr_assert_eq(ORIENTATION_SOUTH, 2);
    cr_assert_eq(ORIENTATION_WEST, 3);
}

Test(incantation, level_1_requirements)
{
    cr_assert_eq(incantation_types[0].level, 1);
    cr_assert_eq(incantation_types[0].nextLevel, 2);
    cr_assert_eq(incantation_types[0].nbPlayer, 1);
    cr_assert_eq(incantation_types[0].inv.linemate, 1);
    cr_assert_eq(incantation_types[0].inv.deraumere, 0);
    cr_assert_eq(incantation_types[0].inv.sibur, 0);
}

Test(incantation, level_7_requirements)
{
    cr_assert_eq(incantation_types[6].level, 7);
    cr_assert_eq(incantation_types[6].nextLevel, 8);
    cr_assert_eq(incantation_types[6].nbPlayer, 6);
    cr_assert_eq(incantation_types[6].inv.linemate, 2);
    cr_assert_eq(incantation_types[6].inv.deraumere, 2);
    cr_assert_eq(incantation_types[6].inv.sibur, 2);
    cr_assert_eq(incantation_types[6].inv.mendiane, 2);
    cr_assert_eq(incantation_types[6].inv.phiras, 2);
}

Test(incantation, table_terminator)
{
    cr_assert_eq(incantation_types[7].level, 0);
}

Test(incantation, all_levels_present)
{
    for (int i = 0; i < 7; i++) {
        cr_assert_eq(incantation_types[i].level, (uint64_t)(i + 1));
        cr_assert_eq(incantation_types[i].nextLevel, (uint64_t)(i + 2));
    }
}

Test(unit_values, forward_cost_7)
{
    cr_assert_eq(unit_values[0].type, UNIT_FORWARD);
    cr_assert_eq(unit_values[0].value, 7);
}

Test(unit_values, fork_cost_42)
{
    bool found = false;

    for (int i = 0; unit_values[i].type != UNIT_NONE; i++) {
        if (unit_values[i].type == UNIT_FORK) {
            cr_assert_eq(unit_values[i].value, 42);
            found = true;
        }
    }
    cr_assert(found, "UNIT_FORK not found in unit_values");
}

Test(unit_values, inventory_cost_1)
{
    bool found = false;

    for (int i = 0; unit_values[i].type != UNIT_NONE; i++) {
        if (unit_values[i].type == UNIT_INVENTORY) {
            cr_assert_eq(unit_values[i].value, 1);
            found = true;
        }
    }
    cr_assert(found, "UNIT_INVENTORY not found in unit_values");
}

Test(unit_values, survie_cost_126)
{
    bool found = false;

    for (int i = 0; unit_values[i].type != UNIT_NONE; i++) {
        if (unit_values[i].type == UNIT_SURVIE) {
            cr_assert_eq(unit_values[i].value, 126);
            found = true;
        }
    }
    cr_assert(found, "UNIT_SURVIE not found in unit_values");
}

Test(list_push, push_first_element)
{
    uint64_t size = 0;
    bool status = false;
    int *arr = list_push(NULL, &size, sizeof(int), &status);

    cr_assert(status);
    cr_assert_eq(size, 1);
    cr_assert_not_null(arr);
    arr[0] = 42;
    cr_assert_eq(arr[0], 42);
    free(arr);
}

Test(list_push, push_multiple)
{
    uint64_t size = 0;
    bool status = false;
    int *arr = NULL;

    arr = list_push(arr, &size, sizeof(int), &status);
    cr_assert(status);
    cr_assert_eq(size, 1);
    arr[0] = 10;
    arr = list_push(arr, &size, sizeof(int), &status);
    cr_assert(status);
    cr_assert_eq(size, 2);
    arr[1] = 20;
    arr = list_push(arr, &size, sizeof(int), &status);
    cr_assert(status);
    cr_assert_eq(size, 3);
    arr[2] = 30;
    cr_assert_eq(arr[0], 10);
    cr_assert_eq(arr[1], 20);
    cr_assert_eq(arr[2], 30);
    free(arr);
}

Test(torus, wrap_positive_overflow)
{
    uint64_t width = 10;
    int64_t x = 12;
    uint64_t result = ((x % (int64_t)width) + (int64_t)width)
        % (int64_t)width;

    cr_assert_eq(result, 2);
}

Test(torus, wrap_negative_overflow)
{
    uint64_t width = 10;
    int64_t x = -3;
    uint64_t result = ((x % (int64_t)width) + (int64_t)width)
        % (int64_t)width;

    cr_assert_eq(result, 7);
}

Test(torus, wrap_exact_boundary)
{
    uint64_t width = 10;
    int64_t x = 10;
    uint64_t result = ((x % (int64_t)width) + (int64_t)width)
        % (int64_t)width;

    cr_assert_eq(result, 0);
}

Test(torus, wrap_zero)
{
    uint64_t width = 10;
    int64_t x = 0;
    uint64_t result = ((x % (int64_t)width) + (int64_t)width)
        % (int64_t)width;

    cr_assert_eq(result, 0);
}

Test(density, constants_defined)
{
    cr_assert_eq(DENSITY_FOOD, 50);
    cr_assert_eq(DENSITY_LINEMATE, 30);
    cr_assert_eq(DENSITY_DERAUMERE, 15);
    cr_assert_eq(DENSITY_SIBUR, 10);
    cr_assert_eq(DENSITY_MENDIANE, 10);
    cr_assert_eq(DENSITY_PHIRAS, 8);
    cr_assert_eq(DENSITY_THYSTAME, 5);
}

Test(density, game_constants)
{
    cr_assert_eq(BASE_FOOD, 10);
    cr_assert_eq(WIN_LEVEL, 8);
    cr_assert_eq(WIN_PLAYER_COUNT, 6);
    cr_assert_eq(FOOD_SPAWN_UNIT, 20);
    cr_assert_eq(EAT_UNIT, 126);
    cr_assert_eq(INCANTATION_UNIT, 300);
}

Test(egg, default_values)
{
    egg_t egg = {0};

    cr_assert_eq(egg.pos.x, 0);
    cr_assert_eq(egg.pos.y, 0);
    cr_assert_eq(egg.taken, false);
    cr_assert_null(egg.team);
}

Test(tile, zeroed_inventory)
{
    tile_t tile = {0};

    cr_assert_eq(tile.inventory.food, 0);
    cr_assert_eq(tile.inventory.linemate, 0);
    cr_assert_eq(tile.pos.x, 0);
    cr_assert_eq(tile.pos.y, 0);
}

Test(tile, modify_resources)
{
    tile_t tile = {0};

    tile.inventory.food = 5;
    tile.inventory.linemate = 3;
    tile.inventory.thystame = 1;
    cr_assert_eq(tile.inventory.food, 5);
    cr_assert_eq(tile.inventory.linemate, 3);
    cr_assert_eq(tile.inventory.thystame, 1);
    cr_assert_eq(tile.inventory.deraumere, 0);
}

Test(commands, ia_forward_lookup)
{
    invocation_t inv = get_command_invoke("Forward", ia_commands);

    cr_assert_eq(inv, SERVER_IA_FORWARD);
}

Test(commands, ia_look_lookup)
{
    invocation_t inv = get_command_invoke("Look", ia_commands);

    cr_assert_eq(inv, SERVER_IA_LOOK);
}

Test(commands, ia_unknown_command)
{
    invocation_t inv = get_command_invoke("NotACommand", ia_commands);

    cr_assert_eq(inv, UNK_INVOCATION);
}

Test(commands, gui_msz_lookup)
{
    invocation_t inv = get_command_invoke("msz", gui_commands);

    cr_assert_eq(inv, CLIENT_MAP_SIZE);
}

Test(commands, gui_tna_lookup)
{
    invocation_t inv = get_command_invoke("tna", gui_commands);

    cr_assert_eq(inv, CLIENT_TEAMS_NAMES);
}

Test(commands, ia_unit_type_forward)
{
    unit_type_t type = get_command_unit_type(SERVER_IA_FORWARD, ia_commands);

    cr_assert_eq(type, UNIT_FORWARD);
}

Test(commands, ia_unit_type_inventory)
{
    unit_type_t type = get_command_unit_type(SERVER_IA_INVENTORY, ia_commands);

    cr_assert_eq(type, UNIT_INVENTORY);
}

Test(commands, gui_commands_unit_none)
{
    unit_type_t type = get_command_unit_type(CLIENT_MAP_SIZE, gui_commands);

    cr_assert_eq(type, UNIT_NONE);
}

Test(commands, gui_commands_unit_none_2)
{
    unit_type_t type = get_command_unit_type(CLIENT_TEAMS_NAMES, gui_commands);

    cr_assert_eq(type, UNIT_NONE);
}
