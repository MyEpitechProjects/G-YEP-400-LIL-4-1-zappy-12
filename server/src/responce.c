/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

void gen_uuid(char *dest)
{
    uuid_t u;

    uuid_generate_random(u);
    uuid_unparse(u, dest);
}

void set_message(message_t *resp, message_type_t type, status_t status,
    invocation_t func)
{
    resp->_func = func;
    resp->_status = status;
    resp->_type = type;
}

static bool write_response(int fd, message_t *resp, void *ptr)
{
    char r[sizeof(message_t) + resp->_body_size];

    memcpy(r, resp, sizeof(message_t));
    if (resp->_body_size > 0)
        memcpy(r + sizeof(message_t), ptr, resp->_body_size);
    write(fd, r, sizeof(message_t) + resp->_body_size);
    return true;
}

bool send_response(int fd, message_t *resp, void *ptr)
{
    if (!ptr)
        resp->_body_size = 0;
    write_response(fd, resp, ptr);
    return true;
}

bool err_response(int fd, message_t *resp, req_body_t *body)
{
    resp->_body_size = UUID_STR_LEN;
    if (resp->_status == USER_NOT_EXIST)
        return send_response(fd, resp, body->user_uuid);
    if (resp->_status == TEAM_NOT_EXIST)
        return send_response(fd, resp, body->team_uuid);
    if (resp->_status == CHANNEL_NOT_EXIST)
        return send_response(fd, resp, body->channel_uuid);
    if (resp->_status == THREAD_NOT_EXIST)
        return send_response(fd, resp, body->thread_uuid);
    resp->_body_size = 0;
    return send_response(fd, resp, NULL);
}
