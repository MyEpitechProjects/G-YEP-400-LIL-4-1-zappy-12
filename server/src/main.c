/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static int print_help(int argCode)
{
    printf("USAGE: ./myteams_server port\n\n"
        "port is the port number on which the"
        " server socket listens.\n");
    return argCode;
}

int main(int argc, char **argv)
{
    arguments_t arguments;
    sigset_t mask;
    int sfd;
    int argCode = parse_arg(argc, argv, &arguments);

    if (argCode != -1)
        return print_help(argCode);
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
