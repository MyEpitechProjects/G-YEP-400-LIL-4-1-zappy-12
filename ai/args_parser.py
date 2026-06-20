##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/args_parser.py
## File description:
## Parse command line arguments
##

import sys

def parse_arguments(arguments: list) -> tuple[int, str, str]:
    """Parse arguments and return (port, name, machine) or exit if invalid."""

    port = 0
    name = ""
    machine = "localhost"

    if len(arguments) == 1:
        print("USAGE: ./zappy_ai -p port -n name -h machine", file=sys.stderr)
        sys.exit(84)
    elif len(arguments) == 2 and arguments[1] == "--help":
        print("USAGE: ./zappy_ai -p port -n name -h machine")
        sys.exit(0)
    elif len(arguments) == 5 and arguments[1] == "-p" and arguments[3] == "-n":
        try:
            port = int(arguments[2])
        except ValueError:
            sys.exit(84)
        name = arguments[4]
    elif len(arguments) == 7 and arguments[1] == "-p" and arguments[3] == "-n" and arguments[5] == "-h":
        try:
            port = int(arguments[2])
        except ValueError:
            sys.exit(84)
        name = arguments[4]
        machine = arguments[6]
    else:
        print("USAGE: ./zappy_ai -p port -n name -h machine", file=sys.stderr)
        sys.exit(84)
    if name == "GRAPHIC":
        print("Error: 'GRAPHIC' is a reserved team name for the GUI. Try with another name.", file=sys.stderr)
        sys.exit(84)
    return port, name, machine
