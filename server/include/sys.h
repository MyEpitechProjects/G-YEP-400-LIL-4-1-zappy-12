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

void my_memset(char *value, uint8_t byte, size_t size);
uint64_t randint(uint64_t max);
uint64_t max(uint64_t x, uint64_t y);
uint64_t min(uint64_t x, uint64_t y);

#endif
