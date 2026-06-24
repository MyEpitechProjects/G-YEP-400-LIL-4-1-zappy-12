/*
** EPITECH PROJECT, 2026
** server ftp
** File description:
** none
*/

#include "myserver.h"

static bool set_signal(sigset_t *mask, int *sfd)
{
    if (sigemptyset(mask) < 0 ||
        sigaddset(mask, SIGINT) < 0 ||
        sigaddset(mask, SIGTERM) < 0 ||
        sigaddset(mask, SIGQUIT) < 0 ||
        sigprocmask(SIG_BLOCK, mask, NULL) < 0)
        return false;
    *(sfd) = signalfd(-1, mask, 0);
    if (*sfd < 0)
        return false;
    return true;
}

int main(int argc, char **argv)
{
    arguments_t arguments;
    sigset_t mask;
    int sfd = 0;
    int argCode = parse_arg(argc, argv, &arguments);

    if (argCode != 0 || !check_args(&arguments, argc, argv))
        return argCode;
    if (!set_signal(&mask, &sfd))
        return 84;
    if (start_server(&arguments, sfd))
        return 0;
    return 84;
}
