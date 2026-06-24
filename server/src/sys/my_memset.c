/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "sys.h"

void *my_memset(void *dest, uint8_t byte, size_t size)
{
    uint8_t *value = (uint8_t *)dest;

    if (!value || size < 0)
        return NULL;
    for (size_t i = 0; i < size; i++){
        value[i] = byte;
    }
    return value;
}

void *my_memcpy(void *dest, void *src, size_t size)
{
    uint8_t *value = (uint8_t *)dest;
    uint8_t *value2 = (uint8_t *)src;

    if (!value || !value2 || size < 0)
        return NULL;
    for (size_t i = 0; i < size; i++){
        value[i] = value2[i];
    }
    return value;
}

char *my_strcpy(char *dest, const char *src)
{
    size_t i = 0;

    if (!dest || !src)
        return NULL;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

char *my_strncpy(char *dest, const char *src, size_t n)
{
    size_t i = 0;

    if (!dest || !src || n < 0)
        return NULL;
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

char *my_strcat(char *dest, const char *src)
{
    char *ptr = dest;

    if (!dest || !src)
        return NULL;
    while (*ptr != '\0')
        ptr++;
    while (*src != '\0') {
        *(ptr) = *src;
        ptr++;
        src++;
    }
    *(ptr) = '\0';
    return dest;
}
