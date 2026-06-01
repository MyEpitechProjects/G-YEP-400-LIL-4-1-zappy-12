##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/send_action.py
## File description:
## Send action to the server
##

import sys
import socket

def send_move_up_one_tile(socket_file):
    socket_file.write("Forward\n")
    socket_file.flush()

def send_turn_90_right(socket_file):
    socket_file.write("Right\n")
    socket_file.flush()

def send_turn_90_left(socket_file):
    socket_file.write("Left\n")
    socket_file.flush()

def send_look(socket_file):
    socket_file.write("Look\n")
    socket_file.flush()

def send_inventory(socket_file):
    socket_file.write("Inventory\n")
    socket_file.flush()

def send_broadcast_text(socket_file, message: str):
    socket_file.write(f"Broadcast {message}\n")
    socket_file.flush()

def send_number_of_team_unused(socket_file):
    socket_file.write("Connect_nbr\n")
    socket_file.flush()

def send_fork_a_player(socket_file):
    socket_file.write("Fork\n")
    socket_file.flush()

def eject_a_player_from_this_tile(socket_file):
    socket_file.write("Eject\n")
    socket_file.flush()

def send_take_object(socket_file, object_name: str):
    socket_file.write(f"Take {object_name}\n")
    socket_file.flush()

def send_set_object_down(socket_file, object_name: str):
    socket_file.write(f"Set {object_name}\n")
    socket_file.flush()

def send_start_incantation(socket_file):
    socket_file.write("Incantation\n")
    socket_file.flush()
