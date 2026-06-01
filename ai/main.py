#!/usr/bin/env python3

##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/main.py
## File description:
## main ai file
##

# Standard lib import
import sys

# Function from other file import
from ai.server_connection import first_connection
from ai.trantorian import Trantorian

def main(arguments): #arguments -> sys.argv (char **av C/C++ equivalent)
    """Main function : parse arguments"""

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

    # Graphic team name verification
    if name == "GRAPHIC":
        print("Error: 'GRAPHIC' is a reserved team name for the GUI. Try with an other name.", file=sys.stderr)
        sys.exit(84)

    print(f"port : {port}")
    print(f"name : {name}")
    print(f"machine : {machine}")

    # Connection to server
    client_socket, socket_file, dimensions, client_num = first_connection(port, name, machine)
    width_str, height_str = dimensions.split()

    trantorian = Trantorian(team_name=name, map_width=int(width_str), map_height=int(height_str), initial_slots=client_num)

    print(f"Trantorian initialized.")
    print(f"Level: {trantorian.level}")
    print(f"Food : {trantorian.inventory['food']}")
    print("AI connected. Waiting... (Press Ctrl+C to quit)")
    try:
        while True:
            pass #will be updated, with AI behavior
    except KeyboardInterrupt:
        # Avoid red error
        print("\nDisconnection requested (Ctrl+C).")
    finally:
        socket_file.close()
        client_socket.close()
        print("Closing the AI client.")
    
if __name__ == "__main__":
    main(sys.argv)
