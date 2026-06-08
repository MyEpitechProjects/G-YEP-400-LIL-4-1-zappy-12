/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static channel_t *empty_channel(int fd, channel_t *a)
{
    channel_t *new = NULL;

    if (fd != -1){
        close(fd);
        if (!a)
            return NULL;
    }
    free(a);
    new = calloc(1, sizeof(channel_t));
    if (!new)
        return NULL;
    return new;
}

channel_t *read_channel_file(char *f)
{
    int fd = open(f, O_RDONLY);
    uint32_t len = 0;
    uint32_t byteRead = 0;
    channel_t *channel = NULL;

    if (fd == -1)
        return empty_channel(fd, channel);
    byteRead = read(fd, &len, sizeof(uint32_t));
    if (byteRead != sizeof(uint32_t))
        return empty_channel(fd, channel);
    channel = calloc(len + 1, sizeof(channel_t));
    if (!channel || read(fd, channel, sizeof(channel_t) * len) !=
        (ssize_t)(sizeof(channel_t) * len)){
        return empty_channel(fd, channel);
    }
    close(fd);
    return channel;
}

uint32_t get_channel_len(channel_t *channel)
{
    uint32_t c = 0;

    if (!channel)
        return c;
    while (channel[c].used){
        c++;
    }
    return c;
}

channel_t *add_channel(channel_t *old)
{
    uint32_t len = get_channel_len(old);
    channel_t *new = NULL;

    new = realloc(old, sizeof(channel_t) * (len + 2));
    if (!new)
        return NULL;
    memset(&new[len], 0, sizeof(channel_t));
    memset(&new[len + 1], 0, sizeof(channel_t));
    new[len].used = true;
    new[len + 1].used = false;
    return new;
}

channel_t *remove_channel(channel_t *old, uint32_t index)
{
    uint32_t len = get_channel_len(old);
    channel_t *new = NULL;

    if (!old || index >= len)
        return old;
    for (uint32_t i = index; i < len; i++) {
        old[i] = old[i + 1];
    }
    new = realloc(old, sizeof(channel_t) * len);
    if (!new)
        return old;
    return new;
}

bool write_channel_file(char *f, channel_t *channel)
{
    int fd = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    uint32_t len = 0;

    if (fd == -1 || !channel)
        return false;
    len = get_channel_len(channel);
    if (write(fd, &len, sizeof(uint32_t)) != sizeof(uint32_t)) {
        close(fd);
        return false;
    }
    if (write(fd, channel, sizeof(channel_t) * len) !=
        (ssize_t)(sizeof(channel_t) * len)) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
