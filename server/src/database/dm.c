/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static dm_t *empty_dm(int fd, dm_t *a)
{
    dm_t *new = NULL;

    if (fd != -1){
        close(fd);
        if (!a)
            return NULL;
    }
    free(a);
    new = calloc(1, sizeof(dm_t));
    if (!new)
        return NULL;
    return new;
}

dm_t *read_dm_file(char *f)
{
    int fd = open(f, O_RDONLY);
    uint32_t len = 0;
    uint32_t byteRead = 0;
    dm_t *dm = NULL;

    if (fd == -1)
        return empty_dm(fd, dm);
    byteRead = read(fd, &len, sizeof(uint32_t));
    if (byteRead != sizeof(uint32_t))
        return empty_dm(fd, dm);
    dm = calloc(len + 1, sizeof(dm_t));
    if (!dm || read(fd, dm, sizeof(dm_t) * len) !=
        (ssize_t)(sizeof(dm_t) * len)){
        return empty_dm(fd, dm);
    }
    close(fd);
    return dm;
}

dm_t *add_dm(dm_t *old)
{
    uint32_t len = get_dm_len(old);
    dm_t *new = NULL;

    new = realloc(old, sizeof(dm_t) * (len + 2));
    if (!new)
        return NULL;
    memset(&new[len], 0, sizeof(dm_t));
    memset(&new[len + 1], 0, sizeof(dm_t));
    new[len].used = true;
    new[len + 1].used = false;
    return new;
}

dm_t *remove_dm(dm_t *old, uint32_t index)
{
    uint32_t len = get_dm_len(old);
    dm_t *new = NULL;

    if (!old || index >= len)
        return old;
    for (uint32_t i = index; i < len; i++) {
        old[i] = old[i + 1];
    }
    new = realloc(old, sizeof(dm_t) * len);
    if (!new)
        return old;
    return new;
}

uint32_t get_dm_len(dm_t *dm)
{
    uint32_t c = 0;

    if (!dm)
        return c;
    while (dm[c].used){
        c++;
    }
    return c;
}

bool write_dm_file(char *f, dm_t *dm)
{
    int fd = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    uint32_t len = 0;

    if (fd == -1 || !dm)
        return false;
    len = get_dm_len(dm);
    if (write(fd, &len, sizeof(uint32_t)) != sizeof(uint32_t)) {
        close(fd);
        return false;
    }
    if (write(fd, dm, sizeof(dm_t) * len) !=
        (ssize_t)(sizeof(dm_t) * len)) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
