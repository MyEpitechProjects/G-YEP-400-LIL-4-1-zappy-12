##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/connection.py
## File description:
## Connection to server for the 1st time
##

import socket
import sys

def first_connection(port: int, name: str, machine: str):
    """Connect for the 1st time to the server."""

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        client_socket.connect((machine, port))
        print(f"Connection established with server {machine}:{port}")
        socket_file = client_socket.makefile('rw')

        # 1 - Receive WELCOME
        welcome = socket_file.readline().strip()
        if not welcome:
            print("Error: Server closed the connection immediately.", file=sys.stderr)
            sys.exit(84)
        print(f"Server: {welcome}")

        # 2 - Send TEAM-NAME
        socket_file.write(f"{name}\n")
        socket_file.flush()

        # 3 - Receive CLIENT-NUM
        client_num = socket_file.readline().strip()

        # ko gestion
        if client_num == "ko":
            print(f"Error: Connection refused. Team '{name}' is full or does not exist.", file=sys.stderr)
            socket_file.close()
            client_socket.close()
            sys.exit(84)

        print(f"Available slots for the team: {client_num}")

        # 4 - Receive world dimension X Y
        dimensions = socket_file.readline().strip()
        print(f"World dimensions: {dimensions}")

        return client_socket, socket_file, dimensions, int(client_num)

    # Error management
    except Exception as e:
        print(f"An error occurred: {e}", file=sys.stderr)
        sys.exit(84)
