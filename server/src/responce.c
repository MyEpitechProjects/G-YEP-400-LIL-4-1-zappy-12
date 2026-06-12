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

char *send_string(int fd, char *resp)
{
    resp = string_end(resp);
    if (!resp)
        return NULL;
    write(fd, resp, strlen(resp));
    free(resp);
    return NULL;
}
