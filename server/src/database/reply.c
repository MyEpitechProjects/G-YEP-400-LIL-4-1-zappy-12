/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static reply_t *empty_reply(int fd, reply_t *a)
{
    reply_t *new = NULL;

    if (fd != -1){
        close(fd);
        if (!a)
            return NULL;
    }
    free(a);
    new = calloc(1, sizeof(reply_t));
    if (!new)
        return NULL;
    return new;
}

reply_t *read_reply_file(char *f)
{
    int fd = open(f, O_RDONLY);
    uint32_t len = 0;
    uint32_t byteRead = 0;
    reply_t *reply = NULL;

    if (fd == -1)
        return empty_reply(fd, reply);
    byteRead = read(fd, &len, sizeof(uint32_t));
    if (byteRead != sizeof(uint32_t))
        return empty_reply(fd, reply);
    reply = calloc(len + 1, sizeof(reply_t));
    if (!reply || read(fd, reply, sizeof(reply_t) * len) !=
        (ssize_t)(sizeof(reply_t) * len)){
        return empty_reply(fd, reply);
    }
    close(fd);
    return reply;
}

reply_t *add_reply(reply_t *old)
{
    uint32_t len = get_reply_len(old);
    reply_t *new = NULL;

    new = realloc(old, sizeof(reply_t) * (len + 2));
    if (!new)
        return NULL;
    memset(&new[len], 0, sizeof(reply_t));
    memset(&new[len + 1], 0, sizeof(reply_t));
    new[len].used = true;
    new[len + 1].used = false;
    return new;
}

reply_t *remove_reply(reply_t *old, uint32_t index)
{
    uint32_t len = get_reply_len(old);
    reply_t *new = NULL;

    if (!old || index >= len)
        return old;
    for (uint32_t i = index; i < len; i++) {
        old[i] = old[i + 1];
    }
    new = realloc(old, sizeof(reply_t) * len);
    if (!new)
        return old;
    return new;
}

uint32_t get_reply_len(reply_t *reply)
{
    uint32_t c = 0;

    if (!reply)
        return c;
    while (reply[c].used){
        c++;
    }
    return c;
}

bool write_reply_file(char *f, reply_t *reply)
{
    int fd = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    uint32_t len = 0;

    if (fd == -1 || !reply)
        return false;
    len = get_reply_len(reply);
    if (write(fd, &len, sizeof(uint32_t)) != sizeof(uint32_t)) {
        close(fd);
        return false;
    }
    if (write(fd, reply, sizeof(reply_t) * len) !=
        (ssize_t)(sizeof(reply_t) * len)) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
