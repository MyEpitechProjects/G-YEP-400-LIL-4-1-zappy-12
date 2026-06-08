/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static subscription_t *empty_subscription(int fd, subscription_t *a)
{
    subscription_t *new = NULL;

    if (fd != -1){
        close(fd);
        if (!a)
            return NULL;
    }
    free(a);
    new = calloc(1, sizeof(subscription_t));
    if (!new)
        return NULL;
    return new;
}

subscription_t *read_subscription_file(char *f)
{
    int fd = open(f, O_RDONLY);
    uint32_t len = 0;
    uint32_t byteRead = 0;
    subscription_t *subscription = NULL;

    if (fd == -1)
        return empty_subscription(fd, subscription);
    byteRead = read(fd, &len, sizeof(uint32_t));
    if (byteRead != sizeof(uint32_t))
        return empty_subscription(fd, subscription);
    subscription = calloc(len + 1, sizeof(subscription_t));
    if (!subscription || read(fd, subscription,
            sizeof(subscription_t) * len) !=
        (ssize_t)(sizeof(subscription_t) * len)){
        return empty_subscription(fd, subscription);
    }
    close(fd);
    return subscription;
}

uint32_t get_subscription_len(subscription_t *subscription)
{
    uint32_t c = 0;

    if (!subscription)
        return c;
    while (subscription[c].used){
        c++;
    }
    return c;
}

subscription_t *add_subscription(subscription_t *old)
{
    uint32_t len = get_subscription_len(old);
    subscription_t *new = NULL;

    new = realloc(old, sizeof(subscription_t) * (len + 2));
    if (!new)
        return NULL;
    memset(&new[len], 0, sizeof(subscription_t));
    memset(&new[len + 1], 0, sizeof(subscription_t));
    new[len].used = true;
    new[len + 1].used = false;
    return new;
}

subscription_t *remove_subscription(subscription_t *old, uint32_t index)
{
    uint32_t len = get_subscription_len(old);
    subscription_t *new = NULL;

    if (!old || index >= len)
        return old;
    for (uint32_t i = index; i < len; i++) {
        old[i] = old[i + 1];
    }
    new = realloc(old, sizeof(subscription_t) * len);
    if (!new)
        return old;
    return new;
}

bool write_subscription_file(char *f, subscription_t *subscription)
{
    int fd = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    uint32_t len = 0;

    if (fd == -1 || !subscription)
        return false;
    len = get_subscription_len(subscription);
    if (write(fd, &len, sizeof(uint32_t)) != sizeof(uint32_t)) {
        close(fd);
        return false;
    }
    if (write(fd, subscription, sizeof(subscription_t) * len) !=
        (ssize_t)(sizeof(subscription_t) * len)) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
