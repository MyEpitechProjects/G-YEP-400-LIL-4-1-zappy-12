##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/args_parser.py
## File description:
## Parse command line arguments
##

import sys

from ai.constants import SUCCESS, FAILURE

def parse_arguments(arguments: list) -> tuple[int, str, str]:
    """Parse -p/-n/-h given in ANY order and return (port, name, machine).
    -p (port) and -n (name) are mandatory; -h (machine) defaults to localhost.
    Exit 84 on any malformed flag or value, exit 0 on --help.
    """

    if len(arguments) == 1:
        print("USAGE: ./zappy_ai -p port -n name -h machine", file=sys.stderr)
        sys.exit(FAILURE)
    if len(arguments) == 2 and arguments[1] == "--help":
        print("USAGE: ./zappy_ai -p port -n name -h machine")
        sys.exit(SUCCESS)
    
    port = None # None until a valid -p is seen (mandatory)
    name = None # None until a valid -n is seen (mandatory)
    machine = "localhost" # default machine when -h is not precised

    options = arguments[1:]
    # Options are (flag, value) pairs in any order, so an odd token count is invalid.
    if len(options) % 2 != 0:
        print("USAGE: ./zappy_ai -p port -n name -h machine", file=sys.stderr)
        sys.exit(FAILURE)
    # Walk the tokens two by two: options[i] is a flag, options[i + 1] its value.
    for index in range(0, len(options), 2):
        flag = options[index]
        value = options[index + 1]
        if flag == "-p":
            try:
                port = int(value) # the port must be a valid integer
            except ValueError:
                print("USAGE: ./zappy_ai -p port -n name -h machine", file=sys.stderr)
                sys.exit(FAILURE)
        elif flag == "-n":
            name = value
        elif flag == "-h":
            machine = value
        else: # any flag that isn't -p/-n/-h is rejected
            print("USAGE: ./zappy_ai -p port -n name -h machine", file=sys.stderr)
            sys.exit(FAILURE)
    
    # -p and -n are required; a port must be a positive number.
    if port is None or name is None or port <= 0:
        print("USAGE: ./zappy_ai -p port -n name -h machine", file=sys.stderr)
        sys.exit(FAILURE)
    # "GRAPHIC" is reserved by the protocol for the GUI client, forbidden for an AI.
    if name == "GRAPHIC":
        print("Error: 'GRAPHIC' is a reserved team name for the GUI. Try with another name.", file=sys.stderr)
        sys.exit(FAILURE)
    return port, name, machine
        
    
