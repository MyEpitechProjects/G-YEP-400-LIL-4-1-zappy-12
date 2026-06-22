/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "sys.h"

uint64_t max(uint64_t x, uint64_t y)
{
    if (x > y)
        return x;
    return y;
}

uint64_t min(uint64_t x, uint64_t y)
{
    if (x > y)
        return y;
    return x;
}
