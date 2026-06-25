/*
** EPITECH PROJECT, 2026
** ftp server
** File description:
** none
*/

#include "myserver.h"
#include "zappy/zappy_internal.h"

unit_type_t get_command_unit_type(invocation_t invoke, const cmd_t *functions)
{
    for (uint8_t i = 0; functions[i].func != UNK_INVOCATION; i++){
        if (invoke != functions[i].func)
            continue;
        return functions[i].type;
    }
    return UNIT_NONE;
}

invocation_t get_command_invoke(char *command, const cmd_t *functions)
{
    if (!command || command[0] == '\0')
        return UNK_INVOCATION;
    for (uint8_t i = 0; functions[i].func != UNK_INVOCATION; i++){
        if (strcmp(command, functions[i].name) != 0)
            continue;
        return functions[i].func;
    }
    return UNK_INVOCATION;
}

cmd_handler_t get_command_function(invocation_t invoke, const cmd_t *functions)
{
    uint64_t counter = 0;

    for (uint8_t i = 0; functions[i].func != UNK_INVOCATION; i++){
        counter++;
        if (invoke != functions[i].func)
            continue;
        return functions[i].handler;
    }
    return functions[counter].handler;
}

uint8_t get_command_nb_args(invocation_t invoke, const cmd_t *functions)
{
    uint64_t counter = 0;

    for (uint8_t i = 0; functions[i].func != UNK_INVOCATION; i++){
        counter++;
        if (invoke != functions[i].func)
            continue;
        return functions[i].nb_args;
    }
    return functions[counter].nb_args;
}
