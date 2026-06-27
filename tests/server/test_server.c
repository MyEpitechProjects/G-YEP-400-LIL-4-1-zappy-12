/*
** EPITECH PROJECT, 2026
** Zappy Server - Unit Tests
** File description:
** Tests for server infrastructure: arg parsing, buffer, split, clock
*/

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <string.h>
#include <stdlib.h>
#include "myserver.h"
#include "sys.h"

Test(split, basic_two_words)
{
    char **result = str_to_word_array("Hello World");

    cr_assert_not_null(result);
    cr_assert_str_eq(result[0], "Hello");
    cr_assert_str_eq(result[1], "World");
    cr_assert_null(result[2]);
    free(result[0]);
    free(result[1]);
    free(result);
}

Test(split, single_word)
{
    char **result = str_to_word_array("Forward");

    cr_assert_not_null(result);
    cr_assert_str_eq(result[0], "Forward");
    cr_assert_null(result[1]);
    free(result[0]);
    free(result);
}

Test(split, multiple_spaces)
{
    char **result = str_to_word_array("  Take   linemate  ");

    cr_assert_not_null(result);
    cr_assert_str_eq(result[0], "Take");
    cr_assert_str_eq(result[1], "linemate");
    cr_assert_null(result[2]);
    free(result[0]);
    free(result[1]);
    free(result);
}

Test(split, tabs_and_spaces)
{
    char **result = str_to_word_array("Set\tderaumere");

    cr_assert_not_null(result);
    cr_assert_str_eq(result[0], "Set");
    cr_assert_str_eq(result[1], "deraumere");
    cr_assert_null(result[2]);
    free(result[0]);
    free(result[1]);
    free(result);
}

Test(split, empty_string)
{
    char **result = str_to_word_array("");

    cr_assert_not_null(result);
    cr_assert_not_null(result[0]);
    cr_assert_null(result[1]);
    free(result[0]);
    free(result);
}

Test(split, null_string)
{
    char **result = str_to_word_array(NULL);

    cr_assert_null(result);
}

Test(list_len, basic_list)
{
    char *list[] = {"a", "b", "c", NULL};

    cr_assert_eq(list_len(list), 3);
}

Test(list_len, single_element)
{
    char *list[] = {"hello", NULL};

    cr_assert_eq(list_len(list), 1);
}

Test(list_len, empty_list)
{
    char *list[] = {NULL};

    cr_assert_eq(list_len(list), 0);
}

Test(clock, reset_sets_zero)
{
    myclock_t clk = {0};

    start_clock(&clk);
    reset_clock(&clk);
    cr_assert_eq(clk.base, 0);
    cr_assert_eq(clk.offset, 0);
    cr_assert_eq(clk.active, false);
}

Test(clock, start_sets_active)
{
    myclock_t clk = {0};

    start_clock(&clk);
    cr_assert_eq(clk.active, true);
    cr_assert_neq(clk.base, 0);
}

Test(clock, elapsed_zero_when_not_started)
{
    myclock_t clk = {0};

    cr_assert_eq(get_elapsed(&clk), 0);
}

Test(clock, null_clock_returns_null)
{
    cr_assert_null(reset_clock(NULL));
    cr_assert_null(start_clock(NULL));
    cr_assert_null(stop_clock(NULL));
    cr_assert_eq(get_elapsed(NULL), 0);
}

Test(sys, max_basic)
{
    cr_assert_eq(max(5, 10), 10);
    cr_assert_eq(max(10, 5), 10);
    cr_assert_eq(max(7, 7), 7);
}

Test(sys, min_basic)
{
    cr_assert_eq(min(5, 10), 5);
    cr_assert_eq(min(10, 5), 5);
    cr_assert_eq(min(7, 7), 7);
}

Test(sys, subsctract_basic)
{
    cr_assert_eq(subsctract(10, 3), 7);
    cr_assert_eq(subsctract(5, 5), 0);
}

Test(sys, subsctract_underflow_returns_zero)
{
    cr_assert_eq(subsctract(3, 10), 0);
}

Test(sys, randint_in_range)
{
    srand(42);
    for (int i = 0; i < 100; i++) {
        uint64_t val = randint(10);
        cr_assert(val < 10, "randint(10) returned %lu", val);
    }
}

Test(arg_parser, valid_args)
{
    arguments_t args = {0};
    char *argv[] = {"./zappy_server", "-p", "4242", "-x", "10", "-y", "10",
        "-n", "team1", "team2", "-c", "2", "-f", "100", NULL};
    int argc = 14;
    int ret = 0;

    optind = 1;
    ret = parse_arg(argc, argv, &args);
    cr_assert_eq(ret, 0);
    cr_assert_eq(args.port, 4242);
    cr_assert_eq(args.width, 10);
    cr_assert_eq(args.height, 10);
    cr_assert_eq(args.client_number, 2);
    cr_assert_eq(args.freq, 100);
    cr_assert_eq(args.team_number, 2);
}

Test(arg_parser, missing_args_returns_84)
{
    arguments_t args = {0};
    char *argv[] = {"./zappy_server", NULL};
    int argc = 1;
    int ret = 0;

    optind = 1;
    ret = parse_arg(argc, argv, &args);
    cr_assert_eq(ret, 84);
}

Test(arg_parser, default_freq_when_not_specified)
{
    arguments_t args = {0};
    char *argv[] = {"./zappy_server", "-p", "4242", "-x", "10", "-y", "10",
        "-n", "team1", "-c", "1", NULL};
    int argc = 11;
    int ret = 0;

    optind = 1;
    ret = parse_arg(argc, argv, &args);
    cr_assert_eq(ret, 0);
    cr_assert_eq(args.freq, 100);
}

Test(arg_parser, width_too_large_returns_84)
{
    arguments_t args = {0};
    char *argv[] = {"./zappy_server", "-p", "4242", "-x", "100", "-y", "10",
        "-n", "team1", "-c", "1", "-f", "10", NULL};
    int argc = 13;
    int ret = 0;

    optind = 1;
    ret = parse_arg(argc, argv, &args);
    cr_assert_eq(ret, 84);
}


Test(buffer, get_buffer_length_null)
{
    cr_assert_eq(get_buffer_length(NULL), 0);
}

Test(buffer, get_buffer_length_no_newline)
{
    cr_assert_eq(get_buffer_length("hello"), 0);
}

Test(buffer, get_buffer_length_with_newline)
{
    cr_assert_eq(get_buffer_length("Forward\n"), 8);
}

Test(buffer, get_buffer_length_two_commands)
{
    cr_assert_eq(get_buffer_length("Forward\nLeft\n"), 8);
}

Test(buffer, get_buffer_extracts_command)
{
    char *buf = strdup("Forward\nLeft\n");
    char *cmd = get_buffer(buf);

    cr_assert_not_null(cmd);
    cr_assert_str_eq(cmd, "Forward");
    free(cmd);
    free(buf);
}

Test(buffer, get_buffer_returns_null_on_empty)
{
    char *cmd = get_buffer("incomplete");

    cr_assert_null(cmd);
}

Test(server_push, push_poller_basic)
{
    connection_t srv = {0};
    socket_t sock = {0};
    bool res = push_poller(&srv, 42, &sock, FD_SERVER);
    cr_assert_eq(res, true);
    cr_assert_eq(srv.nfds, 1);
    cr_assert_eq(srv._fds[0].fd, 42);
    cr_assert_eq(srv._fds[0].events, POLLIN);
    cr_assert_eq(srv._fdsm[0]._socket, &sock);
    cr_assert_eq(srv._fdsm[0]._type, FD_SERVER);
}

Test(server_push, push_socket_basic)
{
    connection_t srv = {0};
    bool res = push_socket(&srv, 0, 10, FD_SERVER);
    cr_assert_eq(res, true);
    cr_assert_eq(srv.nsock, 1);
    cr_assert_eq(srv.nfds, 1);
    cr_assert_eq(srv._sockets[0]._address.sin_family, AF_INET);
    cr_assert_eq(srv._fdsm[0]._type, FD_SERVER);
}

Test(server_pop, pop_poller_basic)
{
    connection_t srv = {0};
    socket_t sock = {0};
    push_poller(&srv, 42, &sock, FD_SERVER);
    push_poller(&srv, 43, &sock, FD_CONTROL);
    
    uint32_t index = 0;
    bool res = pop_poller(&srv, &index);
    cr_assert_eq(res, true);
    cr_assert_eq(srv.nfds, 1);
    cr_assert_eq(srv._fds[0].fd, 43);
    cr_assert_eq(srv._fdsm[0]._type, FD_CONTROL);
}
