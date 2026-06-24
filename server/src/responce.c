/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

char *send_string(int fd, char *resp)
{
    ssize_t rs = 0;

    resp = string_end(resp);
    if (!resp)
        return NULL;
    rs = send(fd, resp, strlen(resp), MSG_NOSIGNAL);
    if (rs < 0 || (unsigned long)rs != strlen(resp)){
        printf("Failed to write (%ld, %lu)\n", rs, strlen(resp));
    }
    resp = myfree(resp);
    return resp;
}
