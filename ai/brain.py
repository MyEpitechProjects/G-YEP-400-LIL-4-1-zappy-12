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
STONES = ["linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]
HIGH_FOOD_PRIORITY = 100
NORMAL_FOOD_PRIORITY = 20
STONE_PRIORITY = 80
DISTANCE_PENALTY = 10
CURRENT_TILE_BONUS = 5

def send_command(socket_file, command):
    socket_file.write(command + "\n")
    socket_file.flush()

def is_async_message(response):
    """Ligne non sollicitée : ne correspond à AUCUNE commande envoyée."""
    return response.startswith("message ") or response.startswith("eject:")

def parse_inventory(response):
    """Parse an inventory response into a resource dictionary."""

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
    """Parse a look response into a list of visible tiles."""

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

def has_required_stones_in_inventory(level, inventory):
    """Check if inventory contains all stones required for next level."""

    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:
        return False
    requirements = REQUIREMENTS[level_to_reach]
    for stone in STONES:
        if inventory.get(stone, 0) < requirements.get(stone, 0):
            return False
    return True

def get_missing_stones(level, inventory):
    """Return stones missing from inventory for next incantation."""

    level_to_reach = level + 1
    missing = {}
    if level_to_reach not in REQUIREMENTS:
        return missing
    requirements = REQUIREMENTS[level_to_reach]
    for stone in STONES:
        amount = requirements.get(stone, 0) - inventory.get(stone, 0)
        if amount > 0:
            missing[stone] = amount
    return missing

def missing_stone_visible(level, inventory, vision):
    """Check if a missing stone is visible in current vision."""

    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:
        return False
    requirements = REQUIREMENTS[level_to_reach]
    for stone in STONES:
        required_amount = requirements.get(stone, 0)
        current_amount = inventory.get(stone, 0)
        missing_amount = required_amount - current_amount
        if missing_amount <= 0:
            continue
        for tile in vision:
            if tile.get(stone, 0) > 0:
                return True
    return False

def can_start_incantation(level, inventory, vision):
    """Check if current tile meets incantation requirements."""

    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:
        return False
    if len(vision) == 0:
        return False
    requirements = REQUIREMENTS[level_to_reach]
    current_tile = vision[0]
    if current_tile.get("player", 0) < requirements.get("players", 0):
        return False
    for stone in STONES:
        if current_tile.get(stone, 0) < requirements.get(stone, 0):
            return False
    return True

def distance_from_tile_index(tile_index):
    """Estimate distance between player and a visible tile index."""

    distance = 0
    while (distance + 1) * (distance + 1) <= tile_index:
        distance += 1
    return distance

def find_best_tile(vision, inventory, level):
    """Find the most valuable visible tile for the current state."""

    if len(vision) == 0:
        return 0
    best_tile_index = 0
    best_score = -9999
    level_to_reach = level + 1
    requirements = REQUIREMENTS.get(level_to_reach, {})
    food = inventory.get("food", 0)
    for tile_index, tile in enumerate(vision):
        score = 0
        distance = distance_from_tile_index(tile_index)
        if food <= FOOD_LOW_LIMIT:
            score += tile.get("food", 0) * HIGH_FOOD_PRIORITY
        else:
            score += tile.get("food", 0) * NORMAL_FOOD_PRIORITY
        for stone in STONES:
            required_amount = requirements.get(stone, 0)
            current_amount = inventory.get(stone, 0)
            missing_amount = required_amount - current_amount
            if missing_amount > 0:
                score += tile.get(stone, 0) * STONE_PRIORITY
        score -= distance * DISTANCE_PENALTY
        if tile_index == 0:
            score += CURRENT_TILE_BONUS
        if score > best_score:
            best_score = score
            best_tile_index = tile_index
    return best_tile_index

def choose_state(inventory, vision, level):
    """Choose the current AI state from inventory, vision and level."""

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
    """Update trantorian memory using the last server response."""

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

def build_move_plan(tile_index):
    """Build a movement plan to reach a visible tile index."""

    plan = []
    if tile_index == 0:
        return plan
    row = distance_from_tile_index(tile_index)
    row_start = row * row
    row_center = row_start + row
    offset = tile_index - row_center
    if offset < 0:
        plan.append("Left")
        for _ in range(abs(offset)):
            plan.append("Forward")
        plan.append("Right")
    elif offset > 0:
        plan.append("Right")
        for _ in range(offset):
            plan.append("Forward")
        plan.append("Left")
    for _ in range(row):
        plan.append("Forward")
    return plan

def build_take_plan(tile, inventory, level):
    """Build a plan to take useful resources from current tile."""

    plan = []
    if len(tile) == 0:
        return plan
    for _ in range(tile.get("food", 0)):
        plan.append("Take food")
    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:
        return plan
    requirements = REQUIREMENTS[level_to_reach]
    for stone in STONES:
        required_amount = requirements.get(stone, 0)
        current_amount = inventory.get(stone, 0)
        missing_amount = required_amount - current_amount
        if missing_amount <= 0:
            continue
        available_amount = tile.get(stone, 0)
        while available_amount > 0 and missing_amount > 0:
            plan.append("Take " + stone)
            available_amount -= 1
            missing_amount -= 1
    return plan

def build_prepare_ritual_plan(level, inventory, current_tile):
    """Build a plan to set required stones before incantation."""

    plan = []
    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:
        return plan
    requirements = REQUIREMENTS[level_to_reach]
    for stone in STONES:
        required_amount = requirements.get(stone, 0)
        amount_on_tile = current_tile.get(stone, 0)
        amount_in_inventory = inventory.get(stone, 0)
        missing_on_tile = required_amount - amount_on_tile
        if missing_on_tile <= 0:
            continue
        while missing_on_tile > 0 and amount_in_inventory > 0:
            plan.append("Set " + stone)
            missing_on_tile -= 1
            amount_in_inventory -= 1
    plan.append("Look")
    return plan

def build_plan(trantorian):
    """Build the next action plan from current AI state."""

    inventory = trantorian.inventory
    vision = trantorian.vision
    level = trantorian.level
    if not inventory:
        return ["Inventory"]
    if not vision:
        return ["Look"]
    state = choose_state(inventory, vision, level)
    trantorian.state = state
    current_tile = vision[0]
    if state == SURVIVE:
        take_plan = build_take_plan(current_tile, inventory, level)
        if take_plan:
            return take_plan
        best_tile = find_best_tile(vision, inventory, level)
        if best_tile != 0:
            plan = build_move_plan(best_tile)
            plan.append("Look")
            return plan
        return ["Forward", "Look"]
    if state == INCANT:
        return ["Incantation"]
    if state == PREPARE_RITUAL:
        plan = build_prepare_ritual_plan(level, inventory, current_tile)
        if plan:
            return plan
        return ["Look"]
    if state == COLLECT_STONE:
        take_plan = build_take_plan(current_tile, inventory, level)
        if take_plan:
            return take_plan
        best_tile = find_best_tile(vision, inventory, level)
        if best_tile != 0:
            plan = build_move_plan(best_tile)
            plan.append("Look")
            return plan
        return ["Forward", "Look"]
    if state == EXPLORE:
        return ["Forward", "Look"]
    return ["Look"]

def run_ai(trantorian: Trantorian, socket_file):
    """Run the main AI loop and communicate with the server."""

    plan = []
    while True:
        if not plan:
            plan = build_plan(trantorian)
        command = plan.pop(0)
        parts = command.split()
        verb = parts[0]
        if len(parts) > 1:
            object_name = parts[1] 
        else:
            object_name = None
        send_command(socket_file, command)
        trantorian.pending_commands += 1
        response = socket_file.readline().strip()
        while is_async_message(response):
            print(f"[async] {response}")
            response = socket_file.readline().strip()
        if verb == "Incantation" and response == "Elevation underway":
            second_response = socket_file.readline().strip()
            response = f"{response} / {second_response}"
        trantorian.pending_commands -= 1
        print(f"[Buffer: {trantorian.pending_commands}/10] {verb:<11} -> {response}")
        if response == "dead":
            print("The trantorian is dead")
            break
        update_memory(trantorian, verb, response, object_name)