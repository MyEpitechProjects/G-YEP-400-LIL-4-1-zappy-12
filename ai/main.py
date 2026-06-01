#!/usr/bin/env python3

##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/main.py
## File description:
## main ai file
##

import sys

def main(arguments): #arguments -> sys.argv (char **av C/C++ equivalent)
    """Main function : parse arguments"""

    port = 0
    name = ""
    machine = "localhost"

    if len(arguments) == 1:
        print("USAGE: ./zappy_ai -p port -n name -h machine")
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
        print("USAGE: ./zappy_ai -p port -n name -h machine")
        sys.exit(84)

    print(f"port : {port}")
    print(f"name : {name}")
    print(f"machine : {machine}")
    
if __name__ == "__main__":
    main(sys.argv)
