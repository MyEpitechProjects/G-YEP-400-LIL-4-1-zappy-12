/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "sys.h"

void my_memset(char *value, uint8_t byte, size_t size)
{
    if (!value)
        return;
    for (size_t i = 0; i < size; i++){
        value[i] = byte;
    }
}
