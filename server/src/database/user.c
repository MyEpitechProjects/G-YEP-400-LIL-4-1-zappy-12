/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static user_t *empty_user(int fd, user_t *a)
{
    user_t *new = NULL;

    if (fd != -1){
        close(fd);
        if (!a)
            return NULL;
    }
    free(a);
    new = calloc(1, sizeof(user_t));
    if (!new)
        return NULL;
    return new;
}

user_t *read_user_file(char *f)
{
    int fd = open(f, O_RDONLY);
    uint32_t len = 0;
    uint32_t byteRead = 0;
    user_t *user = NULL;

    if (fd == -1)
        return empty_user(fd, user);
    byteRead = read(fd, &len, sizeof(uint32_t));
    if (byteRead != sizeof(uint32_t))
        return empty_user(fd, user);
    user = calloc(len + 1, sizeof(user_t));
    if (!user || read(fd, user, sizeof(user_t) * len) !=
        (ssize_t)(sizeof(user_t) * len)){
        return empty_user(fd, user);
    }
    close(fd);
    return user;
}

uint32_t get_user_len(user_t *user)
{
    uint32_t c = 0;

    if (!user)
        return c;
    while (user[c].used){
        c++;
    }
    return c;
}

user_t *add_user(user_t *old)
{
    uint32_t len = get_user_len(old);
    user_t *new = NULL;

    new = realloc(old, sizeof(user_t) * (len + 2));
    if (!new)
        return NULL;
    memset(&new[len], 0, sizeof(user_t));
    memset(&new[len + 1], 0, sizeof(user_t));
    new[len].used = true;
    new[len + 1].used = false;
    return new;
}

user_t *remove_user(user_t *old, uint32_t index)
{
    uint32_t len = get_user_len(old);
    user_t *new = NULL;

    if (!old || index >= len)
        return old;
    for (uint32_t i = index; i < len; i++) {
        old[i] = old[i + 1];
    }
    new = realloc(old, sizeof(user_t) * len);
    if (!new)
        return old;
    return new;
}

bool write_user_file(char *f, user_t *user)
{
    int fd = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    uint32_t len = 0;

    if (fd == -1 || !user)
        return false;
    len = get_user_len(user);
    if (write(fd, &len, sizeof(uint32_t)) != sizeof(uint32_t)) {
        close(fd);
        return false;
    }
    if (write(fd, user, sizeof(user_t) * len) !=
        (ssize_t)(sizeof(user_t) * len)) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
