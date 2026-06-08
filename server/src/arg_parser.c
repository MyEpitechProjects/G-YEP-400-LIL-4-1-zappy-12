/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

int parse_arg(int argc, char **argv, arguments_t *args)
{
    memset(args, 0, sizeof(arguments_t));
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--help") == 0){
            return 0;
        }
        if (i == 1) {
            args->port = atoi(argv[i]);
        }
    }
    if (argc != 2 || args->port == 0)
        return 84;
    return -1;
}
