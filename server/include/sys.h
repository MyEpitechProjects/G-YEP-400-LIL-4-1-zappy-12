/*
** EPITECH PROJECT, 2026
** ftp server
** File description:
** none
*/

#ifndef __SYS__
    #define __SYS__
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <time.h>

void *my_memset(void *value, uint8_t byte, size_t size);
void *my_memcpy(void *value, void *value2, size_t size);
char *my_strcpy(char *dest, const char *src);
char *my_strncpy(char *dest, const char *src, size_t n);
char *my_strcat(char *dest, const char *src);
uint64_t randint(uint64_t max);
uint64_t max(uint64_t x, uint64_t y);
uint64_t min(uint64_t x, uint64_t y);
uint64_t subsctract(uint64_t a, uint64_t b);

#endif
