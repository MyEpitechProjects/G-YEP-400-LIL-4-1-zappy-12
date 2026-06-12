/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** fixed buffer management
*/

#include "myserver.h"
#include "clock.h"
#include "zappy/zappy_internal.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static bool new_buffer(connection_t *_server, uint32_t i, char *req)
{
    _server->_fdsm[i].buffer = calloc(strlen(req) + 1, sizeof(char));
    if (!_server->_fdsm[i].buffer)
        return false;
    strcpy(_server->_fdsm[i].buffer, req);
    return true;
}

static bool append_buffer(connection_t *_server, uint32_t i, char *req)
{
    char *backup = _server->_fdsm[i].buffer;
    size_t old_len = backup ? strlen(backup) : 0;
    size_t req_len = strlen(req);
    char *new_ptr = realloc(backup, old_len + req_len + 1);

    if (!new_ptr)
        return false;
    _server->_fdsm[i].buffer = new_ptr;
    strcpy(_server->_fdsm[i].buffer + old_len, req);
    return true;
}

bool complete_buffer(connection_t *_server, uint32_t *i)
{
    char req[1024];
    ssize_t rd = 0;

    memset(req, 0, 1024);
    rd = read(_server->_fds[*i].fd, req, 1023);
    if (rd == 0)
        return pop_poller(_server, i);
    if (rd < 0)
        return true;
    if (!_server->_fdsm[*i].buffer) {
        new_buffer(_server, *i, req);
    } else {
        append_buffer(_server, *i, req);
    }
    return true;
}

static uint64_t get_buffer_length(char *input)
{
    if (!input)
        return 0;
    for (uint64_t i = 0; input[i] != '\0'; i++) {
        if (input[i] == '\n')
            return i + 1;
    }
    return 0;
}

bool pop_buffer(connection_t *_server, uint32_t i, char *input)
{
    uint64_t w = get_buffer_length(input);
    size_t remaining_len = 0;
    char *new_buff = NULL;

    if (w == 0)
        return true;
    remaining_len = strlen(input) - w;
    if (remaining_len == 0) {
        input = myfree(input);
        _server->_fdsm[i].buffer = NULL;
        return true;
    }
    new_buff = malloc(remaining_len + 1);
    if (!new_buff)
        return false;
    strcpy(new_buff, input + w);
    input = myfree(input);
    _server->_fdsm[i].buffer = new_buff;
    return true;
}

char *get_buffer(char *input)
{
    uint64_t w = get_buffer_length(input);
    char *buffer = NULL;

    if (w == 0)
        return NULL;
    buffer = malloc(w);
    if (!buffer)
        return NULL;
    strncpy(buffer, input, w);
    buffer[w - 1] = '\0';
    return buffer;
}

bool execute_buffer(connection_t *_server, uint32_t *i, bool ret)
{
    char *command = NULL;
    char **command_build = NULL;

    if (ret == false || !_server->_fdsm[*i].buffer)
        return false;
    command = get_buffer(_server->_fdsm[*i].buffer);
    if (!command)
        return true;
    command_build = str_to_word_array(command);
    if (!command_build) {
        command = myfree(command);
        return true;
    }
    handle_request(_server, *i, command_build);
    free_array((void **)command_build);
    command = myfree(command);
    return true;
}
