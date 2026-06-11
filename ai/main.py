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
from ai.args_parser import parse_arguments
from ai.server_connection import first_connection
from ai.trantorian import Trantorian
from ai.brain import run_ai

def main(arguments): #arguments -> sys.argv (char **av C/C++ equivalent)

    # Initialization and parsing
    port, name, machine = parse_arguments(arguments)
    print(f"port : {port}\nname : {name}\nmachine : {machine}")

    # Connection to server
    client_socket, socket_file, dimensions, client_num = first_connection(port, name, machine)
    width_str, height_str = dimensions.split()

    # Create a trantorian
    trantorian = Trantorian(team_name=name, map_width=int(width_str), map_height=int(height_str), initial_slots=client_num)
    trantorian.port = port
    trantorian.machine = machine
    trantorian.binary = sys.argv[0]

    print(f"Trantorian initialized.")
    print(f"Level: {trantorian.level}")
    print("AI connected. Waiting... (Press Ctrl+C to quit)")

    # Start AI loop
    try:
        run_ai(trantorian, socket_file)
    except KeyboardInterrupt:
        print("\nDisconnection requested (Ctrl+C).")
    finally:
        socket_file.close()
        client_socket.close()
        print("Closing the AI client.")
    
if __name__ == "__main__":
    main(sys.argv)
