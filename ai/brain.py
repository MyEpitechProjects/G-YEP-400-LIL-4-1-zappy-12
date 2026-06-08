##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/brain.py
## File description:
## AI logic and main loop
##

import random
from ai.trantorian import Trantorian
from ai.send_action import (send_move_up_one_tile, send_turn_90_left, send_turn_90_right, send_look, send_inventory, send_broadcast_text, send_number_of_team_unused, send_fork_a_player, eject_a_player_from_this_tile, send_take_object, send_set_object_down, send_start_incantation)

FOOD_LOW_LIMIT = 5
SURVIVE = "SURVIVE"
INCANT = "INCANT"
PREPARE_RITUAL = "PREPARE_RITUAL"
COLLECT_STONE = "COLLECT_STONE"
EXPLORE = "EXPLORE"
# requirements dict with the level to be reached  : nb_players - linemate - deraumere - sibur - mendiane - phiras - thystame
REQUIREMENTS = {
    2: {"players": 1, "linemate": 1, "deraumere": 0, "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0},
    3: {"players": 2, "linemate": 1, "deraumere": 1, "sibur": 1, "mendiane": 0, "phiras": 0, "thystame": 0},
    4: {"players": 2, "linemate": 2, "deraumere": 0, "sibur": 1, "mendiane": 0, "phiras": 2, "thystame": 0},
    5: {"players": 4, "linemate": 1, "deraumere": 1, "sibur": 2, "mendiane": 0, "phiras": 1, "thystame": 0},
    6: {"players": 4, "linemate": 1, "deraumere": 2, "sibur": 1, "mendiane": 3, "phiras": 0, "thystame": 0},
    7: {"players": 6, "linemate": 1, "deraumere": 2, "sibur": 3, "mendiane": 0, "phiras": 1, "thystame": 0},
    8: {"players": 6, "linemate": 2, "deraumere": 2, "sibur": 2, "mendiane": 2, "phiras": 2, "thystame": 1}
}

def parse_inventory(response):
    """Transform a string into a dictionary (inventory)"""

    inventory = {}

    if response == "dead":
        return None
    response = response[1:-1]
    items = response.split(",")
    for item in items:
        item = item.strip()
        parts = item.split()
        if len(parts) != 2:
            continue
        resource_name = parts[0]
        quantity = int(parts[1])
        inventory[resource_name] = quantity
    return inventory

def parse_look(response):
    if response == "dead":
        return None
    list_tiles = []
    response = response[1:-1]
    raw_tiles = response.split(",")
    for raw_tile in raw_tiles:
        tile = {}
        raw_tile = raw_tile.strip()
        if raw_tile == "":
            list_tiles.append(tile)
            continue
        objects = raw_tile.split()
        for object_name in objects:
            if object_name not in tile:
                tile[object_name] = 0
            tile[object_name] += 1
        list_tiles.append(tile)
    return list_tiles

def can_start_incantation(level, inventory, vision):
    return False

def has_required_stones_in_inventory(level, inventory):
    return False

def missing_stone_visible(level, inventory, vision):
    return False

def choose_state(inventory, vision, level):
    """Choose the current AI state from known inventory and vision."""
    food = inventory.get("food", 0)

    if food <= FOOD_LOW_LIMIT:
        return SURVIVE
    if can_start_incantation(level, inventory, vision):
        return INCANT
    if has_required_stones_in_inventory(level, inventory):
        return PREPARE_RITUAL
    if missing_stone_visible(level, inventory, vision):
        return COLLECT_STONE
    return EXPLORE

def update_memory(trantorian, action, response, object_name=None):
    if response == "dead":
        return False

    if action == "Inventory":
        inventory = parse_inventory(response)
        if inventory is not None:
            trantorian.inventory = inventory
    elif action == "Look":
        vision = parse_look(response)
        if vision is not None:
            trantorian.vision = vision
    elif action == "Connect_nbr":
        if response.isdigit():
            trantorian.available_slots = int(response)
    elif action == "Incantation":
        if "Current level:" in response:
            level = response.split("Current level:")[-1].strip()
            if level.isdigit():
                trantorian.level = int(level)
        trantorian.vision = []
    elif action in ["Forward", "Right", "Left", "Take", "Set", "Eject"]:
        trantorian.vision = []
    elif action == "Fork":
        trantorian.available_slots = None
    return True

def run_ai(trantorian: Trantorian, socket_file):
    """Main loop for the random walker AI."""
    
    actions = ["Forward", "Right", "Left", "Look", "Inventory", "Broadcast", "Connect_nbr", "Fork", "Eject", "Take", "Set", "Incantation"]
    resources = ["food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]

    while True:
        action = random.choice(actions)
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


        trantorian.pending_commands -= 1
        
        # Display and death check
        buffer_display = f"{trantorian.pending_commands}/10"
        print(f"[Buffer: {buffer_display}] {action:<11} -> {response}")

        if response == "dead":
            print("The trantorian is dead")
            break
