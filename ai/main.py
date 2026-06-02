#!/usr/bin/env python3

##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/main.py
## File description:
## main ai file
##

# Standard lib import
import sys
import random

# Function from other file import
from ai.server_connection import first_connection
from ai.trantorian import Trantorian
from ai.send_action import (
    send_move_up_one_tile,
    send_turn_90_left,
    send_turn_90_right,
    send_look,
    send_inventory,
    send_broadcast_text,
    send_number_of_team_unused,
    send_fork_a_player,
    eject_a_player_from_this_tile,
    send_take_object,
    send_set_object_down,
    send_start_incantation,
)

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
        action_costs = {"Forward": 7, "Right": 7, "Left": 7, "Look": 7, "Inventory": 1, "Broadcast": 7
                        , "Connect_nbr": 0, "Fork": 42, "Eject": 7, "Take": 7, "Set": 7, "Incantation": 300}
        resources = ["food", "linemate", "deraumere", "sibur" "mendiane", "phiras", "thystame"]

        while True:

            # Choose and execute random action
            action = random.choice(["Forward", "Right", "Left", "Look", "Inventory", "Broadcast", "Connect_nbr", "Fork", "Eject", "Take", "Set", "Incantation"])

            # Update local estimated time before sending
            trantorian.life_time_units -= action_costs[action]

            object_name = None

            if action == "Forward":
                send_move_up_one_tile(socket_file)
            elif action == "Right":
                send_turn_90_right(socket_file)
            elif action == "Left":
                send_turn_90_left(socket_file)
            elif action == "Look":
                send_look(socket_file)
            elif action == "Inventory":
                send_inventory(socket_file)
            elif action == "Broadcast":
                send_broadcast_text(socket_file, "random message")
            elif action == "Connect_nbr":
                send_number_of_team_unused(socket_file)
            elif action == "Fork":
                send_fork_a_player(socket_file)
            elif action == "Eject":
                eject_a_player_from_this_tile(socket_file)
            elif action == "Take":
                object_name = random.choice(resources)
                send_take_object(socket_file, object_name)
            elif action == "Set":
                object_name = random.choice(resources)
                send_set_object_down(socket_file, object_name)
            elif action == "Incantation":
                send_start_incantation(socket_file)
            
            # Update pending command counter
            trantorian.pending_commands += 1

            # Wait and read server response
            response = socket_file.readline().strip()

            if action == "Incantation" and response == "Elevation underway":
                second_response = socket_file.readline().strip()
                response = response + " / " + second_response

            if action == "Take" and object_name == "food" and response == "ok":
                trantorian.inventory["food"] += 1
                trantorian.life_time_units += 126

            if trantorian.life_time_units <= 0:
                print("The trantorian is dead")
                break

            # Process the response
            trantorian.pending_commands -= 1
            
            time_display = f"{trantorian.life_time_units:>4} TU"
            buffer_display = f"{trantorian.pending_commands}/10"
            print(f"[Time: {time_display}] [Buffer: {buffer_display}] {action:<7} -> {response}")

            if response == "dead":
                print("The trantorian is dead")
                break
            
    except KeyboardInterrupt:
        # Avoid red error
        print("\nDisconnection requested (Ctrl+C).")
    finally:
        socket_file.close()
        client_socket.close()
        print("Closing the AI client.")
    
if __name__ == "__main__":
    main(sys.argv)
