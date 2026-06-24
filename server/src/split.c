/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include <stdlib.h>
#include <string.h>
#include "sys.h"

unsigned int list_len(char **argv)
{
    unsigned int res = 0;

    for (int i = 0; argv[i] != NULL; i++) {
        res++;
    }
    return res;
}

static int is_sep(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

static int word_count(const char *str)
{
    int i = 0;
    int count = 0;
    int in_word = 0;

    while (str[i]){
        if (is_sep(str[i]))
            in_word = 0;
        if (!is_sep(str[i]) && !in_word){
            in_word = 1;
            count = count + 1;
        }
        i = i + 1;
    }
    return count;
}

static char *word_dup(const char *str, int start, int end)
{
    int len = end - start;
    char *word = malloc(sizeof(char) * (len + 1));
    int i = 0;

    if (!word)
        return NULL;
    while (i < len){
        word[i] = str[start + i];
        i = i + 1;
    }
    word[i] = '\0';
    return word;
}

static void fill_words(char **array, const char *str)
{
    int i = 0;
    int j = 0;
    int start;

    while (str[i]){
        while (str[i] && is_sep(str[i]))
            i = i + 1;
        start = i;
        while (str[i] && !is_sep(str[i]))
            i = i + 1;
        if (start < i){
            array[j] = word_dup(str, start, i);
            j = j + 1;
        }
    }
    array[j] = NULL;
}

static char **empty(void)
{
    char **array = malloc(sizeof(char *) * 2);

    if (!array)
        return NULL;
    array[0] = malloc(2);
    if (array[0])
        my_strcpy(array[0], " ");
    array[1] = NULL;
    return array;
}

char **str_to_word_array(const char *str)
{
    char **array;

    if (!str)
        return NULL;
    if (str[0] == '\0')
        return empty();
    array = malloc(sizeof(char *) * (word_count(str) + 1));
    if (!array)
        return NULL;
    fill_words(array, str);
    return array;
}
