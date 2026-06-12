/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

char *string_create(char *base)
{
    char *string = malloc(1);

    if (!string)
        return NULL;
    string[0] = '\0';
    if (base)
        string = string_append(string, base);
    return string;
}

char *string_append(char *string, char *append)
{
    char *bak = string;

    if (!string)
        return NULL;
    string = realloc(string, sizeof(char) * (strlen(string) +
            strlen(append) + 2));
    if (!string){
        free(bak);
        return NULL;
    }
    strcat(string, append);
    strcat(string, " ");
    return string;
}

char *string_append_int(char *string, int64_t value)
{
    char str[20];

    snprintf(str, sizeof(str), "%ld", value);
    return string_append(string, str);
}

char *string_append_uint(char *string, uint64_t value)
{
    char str[20];

    snprintf(str, sizeof(str), "%lu", value);
    return string_append(string, str);
}

char *string_end(char *string)
{
    if (strlen(string) > 0){
        string[strlen(string) - 1] = '\n';
        return string;
    }
    string_append(string, "\n");
    if (string)
        string[strlen(string) - 1] = '\0';
    return string;
}
