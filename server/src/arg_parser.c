/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"
#include <stdlib.h>

static void print_help(char *prog)
{
    printf("USAGE: %s -p port -x width -y height "
        "-n name1 name2 ... -c clientsNb -f freq\n", prog);
    printf("option description\n");
    printf("-p port port number\n");
    printf("-x width width of the world\n");
    printf("-y height height of the world\n");
    printf("-n name1 name2 ... name of the team\n");
    printf("-c clientsNb number of initial clients per team\n");
    printf("-f freq reciprocal of time unit for execution of actions\n");
}

static void parse_team_names(arguments_t *args, int argc, char **argv)
{
    args->teams_names = &argv[optind - 1];
    args->team_number = 1;
    while (optind < argc && argv[optind][0] != '-') {
        args->team_number++;
        optind++;
    }
}

static void opt_switch(arguments_t *args, int opt, int argc, char **argv)
{
    switch (opt) {
        case 'p':
            args->port = atoi(optarg);
            break;
        case 'x':
            args->width = atoi(optarg);
            break;
        case 'y':
            args->height = atoi(optarg);
            break;
        case 'c':
            args->client_number = atoi(optarg);
            break;
        case 'f':
            args->freq = atoi(optarg);
            break;
        case 'n':
            parse_team_names(args, argc, argv);
            break;
    }
}

static bool check_help(int argc, char **argv)
{
    if ((argc == 2 && !strcmp(argv[1], "--help"))){
        print_help(argv[0]);
        return true;
    }
    return false;
}

static bool default_freq(int argc, char **argv)
{
    int opt = 0;

    while (opt != -1) {
        opt = getopt(argc, argv, "f:");
        if (opt == -1)
            break;
        if (opt == '?' || opt == 'f')
            return false;
    }
    return true;
}

bool check_args(arguments_t *args, int argc, char **argv)
{
    if (default_freq(argc, argv)){
        args->freq = 100;
    }
    if (args->port > 0 && args->freq > 0 && args->height > 0 &&
        args->client_number > 0 && args->team_number > 0 && args->width > 0){
        return true;
    }
    return false;
}

int parse_arg(int argc, char **argv, arguments_t *args)
{
    int opt = 0;

    if (argc <= 1){
        print_help(argv[0]);
        return 84;
    }
    if (check_help(argc, argv))
        return 0;
    while (opt != -1) {
        opt = getopt(argc, argv, "p:x:y:n:c:f:");
        if (opt == -1)
            break;
        if (opt == '?')
            return 84;
        opt_switch(args, opt, argc, argv);
    }
    if (!check_args(args, argc, argv))
        return 84;
    return 0;
}
