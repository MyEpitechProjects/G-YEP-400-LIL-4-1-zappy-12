##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/brain.py
## File description:
## AI logic and main loop
##

import random
from ai.trantorian import Trantorian
from ai.send_action import (
    send_move_up_one_tile, send_turn_90_left, send_turn_90_right,
    send_look, send_inventory, send_broadcast_text,
    send_number_of_team_unused, send_fork_a_player,
    eject_a_player_from_this_tile, send_take_object,
    send_set_object_down, send_start_incantation,
)

def run_ai(trantorian: Trantorian, socket_file):
    """Main loop for the random walker AI."""
    
    action_costs = {
        "Forward": 7, "Right": 7, "Left": 7, "Look": 7, "Inventory": 1, "Broadcast": 7, "Connect_nbr": 0, "Fork": 42, "Eject": 7, "Take": 7, "Set": 7, "Incantation": 300}
    resources = ["food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]

    while True:
        action = random.choice(list(action_costs.keys()))
        trantorian.life_time_units -= action_costs[action]
        object_name = None

        # Send Action
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
        
        trantorian.pending_commands += 1

        # Read response
        response = socket_file.readline().strip()

        if action == "Incantation" and response == "Elevation underway":
            second_response = socket_file.readline().strip()
            response = f"{response} / {second_response}"

        if action == "Take" and object_name == "food" and response == "ok":
            trantorian.inventory["food"] += 1
            trantorian.life_time_units += 126

        trantorian.pending_commands -= 1
        
        # Display and death check
        time_display = f"{trantorian.life_time_units:>4} TU"
        buffer_display = f"{trantorian.pending_commands}/10"
        print(f"[Time: {time_display}] [Buffer: {buffer_display}] {action:<11} -> {response}")

        if response == "dead" or trantorian.life_time_units <= 0:
            print("The trantorian is dead")
            break