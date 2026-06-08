/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"


status_t check_request(connection_t *_server, pollfd_meta_t *client,
    req_body_t *body, message_t *req)
{
    (void)req;
    (void)body;
    (void)client;
    (void)_server;
    return COMMAND_OK;
}

static status_t body_check_no(const arg_body_t *args,
    req_body_t *body)
{
    if (args->channel_uuid == NO && body->channel_uuid[0])
        return INVALID_BODY;
    if (args->body == NO && body->body[0])
        return INVALID_BODY;
    if (args->description == NO && body->description[0])
        return INVALID_BODY;
    if (args->team_uuid == NO && body->team_uuid[0])
        return INVALID_BODY;
    if (args->thread_uuid == NO && body->thread_uuid[0])
        return INVALID_BODY;
    if (args->name == NO && body->name[0])
        return INVALID_BODY;
    if (args->user_uuid == NO && body->user_uuid[0])
        return INVALID_BODY;
    return COMMAND_OK;
}

status_t body_check(const arg_body_t *args,
    req_body_t *body)
{
    if (args->channel_uuid == YES && !body->channel_uuid[0])
        return INVALID_BODY;
    if (args->body == YES && !body->body[0])
        return INVALID_BODY;
    if (args->description == YES && !body->description[0])
        return INVALID_BODY;
    if (args->team_uuid == YES && !body->team_uuid[0])
        return INVALID_BODY;
    if (args->thread_uuid == YES && !body->thread_uuid[0])
        return INVALID_BODY;
    if (args->name == YES && !body->name[0])
        return INVALID_BODY;
    if (args->user_uuid == YES && !body->user_uuid[0])
        return INVALID_BODY;
    return body_check_no(args, body);
}
