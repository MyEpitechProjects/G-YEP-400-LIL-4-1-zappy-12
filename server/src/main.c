/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

int main(int argc, char **argv)
{
    arguments_t arguments;
    sigset_t mask;
    int sfd;
    int argCode = parse_arg(argc, argv, &arguments);

    if (argCode != 0 || !check_args(&arguments, argc, argv))
        return argCode;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGQUIT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sfd = signalfd(-1, &mask, 0);
    if (sfd == -1)
        return 84;
    if (start_server(&arguments, sfd))
        return 0;
    return 84;
}
