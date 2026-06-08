/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static thread_t *empty_thread(int fd, thread_t *a)
{
    thread_t *new = NULL;

    if (fd != -1){
        close(fd);
        if (!a)
            return NULL;
    }
    free(a);
    new = calloc(1, sizeof(thread_t));
    if (!new)
        return NULL;
    return new;
}

thread_t *read_thread_file(char *f)
{
    int fd = open(f, O_RDONLY);
    uint32_t len = 0;
    uint32_t byteRead = 0;
    thread_t *thread = NULL;

    if (fd == -1)
        return empty_thread(fd, thread);
    byteRead = read(fd, &len, sizeof(uint32_t));
    if (byteRead != sizeof(uint32_t))
        return empty_thread(fd, thread);
    thread = calloc(len + 1, sizeof(thread_t));
    if (!thread || read(fd, thread, sizeof(thread_t) * len) !=
        (ssize_t)(sizeof(thread_t) * len)){
        return empty_thread(fd, thread);
    }
    close(fd);
    return thread;
}

uint32_t get_thread_len(thread_t *thread)
{
    uint32_t c = 0;

    if (!thread)
        return c;
    while (thread[c].used){
        c++;
    }
    return c;
}

thread_t *add_thread(thread_t *old)
{
    uint32_t len = get_thread_len(old);
    thread_t *new = NULL;

    new = realloc(old, sizeof(thread_t) * (len + 2));
    if (!new)
        return NULL;
    memset(&new[len], 0, sizeof(thread_t));
    memset(&new[len + 1], 0, sizeof(thread_t));
    new[len].used = true;
    new[len + 1].used = false;
    return new;
}

thread_t *remove_thread(thread_t *old, uint32_t index)
{
    uint32_t len = get_thread_len(old);
    thread_t *new = NULL;

    if (!old || index >= len)
        return old;
    for (uint32_t i = index; i < len; i++) {
        old[i] = old[i + 1];
    }
    new = realloc(old, sizeof(thread_t) * len);
    if (!new)
        return old;
    return new;
}

bool write_thread_file(char *f, thread_t *thread)
{
    int fd = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    uint32_t len = 0;

    if (fd == -1 || !thread)
        return false;
    len = get_thread_len(thread);
    if (write(fd, &len, sizeof(uint32_t)) != sizeof(uint32_t)) {
        close(fd);
        return false;
    }
    if (write(fd, thread, sizeof(thread_t) * len) !=
        (ssize_t)(sizeof(thread_t) * len)) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
