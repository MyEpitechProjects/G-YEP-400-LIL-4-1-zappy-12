/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static team_t *empty_team(int fd, team_t *a)
{
    team_t *new = NULL;

    if (fd != -1){
        close(fd);
        if (!a)
            return NULL;
    }
    free(a);
    new = calloc(1, sizeof(team_t));
    if (!new)
        return NULL;
    return new;
}

team_t *read_team_file(char *f)
{
    int fd = open(f, O_RDONLY);
    uint32_t len = 0;
    uint32_t byteRead = 0;
    team_t *team = NULL;

    if (fd == -1)
        return empty_team(fd, team);
    byteRead = read(fd, &len, sizeof(uint32_t));
    if (byteRead != sizeof(uint32_t))
        return empty_team(fd, team);
    team = calloc(len + 1, sizeof(team_t));
    if (!team || read(fd, team, sizeof(team_t) * len) !=
        (ssize_t)(sizeof(team_t) * len)){
        return empty_team(fd, team);
    }
    close(fd);
    return team;
}

uint32_t get_team_len(team_t *team)
{
    uint32_t c = 0;

    if (!team)
        return c;
    while (team[c].used){
        c++;
    }
    return c;
}

team_t *add_team(team_t *old)
{
    uint32_t len = get_team_len(old);
    team_t *new = NULL;

    new = realloc(old, sizeof(team_t) * (len + 2));
    if (!new)
        return NULL;
    memset(&new[len], 0, sizeof(team_t));
    memset(&new[len + 1], 0, sizeof(team_t));
    new[len].used = true;
    new[len + 1].used = false;
    return new;
}

team_t *remove_team(team_t *old, uint32_t index)
{
    uint32_t len = get_team_len(old);
    team_t *new = NULL;

    if (!old || index >= len)
        return old;
    for (uint32_t i = index; i < len; i++) {
        old[i] = old[i + 1];
    }
    new = realloc(old, sizeof(team_t) * len);
    if (!new)
        return old;
    return new;
}

bool write_team_file(char *f, team_t *team)
{
    int fd = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    uint32_t len = 0;

    if (fd == -1 || !team)
        return false;
    len = get_team_len(team);
    if (write(fd, &len, sizeof(uint32_t)) != sizeof(uint32_t)) {
        close(fd);
        return false;
    }
    if (write(fd, team, sizeof(team_t) * len) !=
        (ssize_t)(sizeof(team_t) * len)) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
